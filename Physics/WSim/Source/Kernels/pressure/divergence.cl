__kernel void calculate_divergence(__read_only image3d_t inVelocity,
                                   __write_only image3d_t outDivergence) {
    // Get current position
    const int4 threadPosition = (int4)((int)get_global_id(0), (int)get_global_id(1), (int)get_global_id(2), 0);

    // Sample neighbouring velocities
    const sampler_t nearestSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
    const float4 samplePosition = convert_float4(threadPosition) + 0.5f;
    const float velocityL = read_imagef(inVelocity, nearestSampler, samplePosition - (float4)(1, 0, 0, 0)).x; // Left
    const float velocityR = read_imagef(inVelocity, nearestSampler, samplePosition + (float4)(1, 0, 0, 0)).x; // Right
    const float velocityD = read_imagef(inVelocity, nearestSampler, samplePosition - (float4)(0, 1, 0, 0)).y; // Down
    const float velocityU = read_imagef(inVelocity, nearestSampler, samplePosition + (float4)(0, 1, 0, 0)).y; // Up
    const float velocityB = read_imagef(inVelocity, nearestSampler, samplePosition - (float4)(0, 0, 1, 0)).z; // Back
    const float velocityF = read_imagef(inVelocity, nearestSampler, samplePosition + (float4)(0, 0, 1, 0)).z; // Front

    // Compute divergence
    const float divergence = ((velocityR - velocityL) + (velocityU - velocityD) + (velocityF - velocityB)) / 2;
    write_imagef(outDivergence, threadPosition, divergence);
}
