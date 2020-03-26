__kernel void apply_pressure(__read_only image3d_t inVelocity,
                             __read_only image3d_t inPressure,
                             int4 inVelocityOffset,
                             __write_only image3d_t outVelocity) {
    // Get current position
    const int4 threadPosition = (int4)((int)get_global_id(0), (int)get_global_id(1), (int)get_global_id(2), 0);
    const int4 velocityPosition = threadPosition + inVelocityOffset;

    // Sample neighbouring pressures
    const sampler_t nearestSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
    const float4 samplePosition = convert_float4(threadPosition) + 0.5f;
    const float pressureL = read_imagef(inPressure, nearestSampler, samplePosition - (float4)(1, 0, 0, 0)).x; // Left
    const float pressureR = read_imagef(inPressure, nearestSampler, samplePosition + (float4)(1, 0, 0, 0)).x; // Right
    const float pressureD = read_imagef(inPressure, nearestSampler, samplePosition - (float4)(0, 1, 0, 0)).x; // Down
    const float pressureU = read_imagef(inPressure, nearestSampler, samplePosition + (float4)(0, 1, 0, 0)).x; // Up
    const float pressureB = read_imagef(inPressure, nearestSampler, samplePosition - (float4)(0, 0, 1, 0)).x; // Back
    const float pressureF = read_imagef(inPressure, nearestSampler, samplePosition + (float4)(0, 0, 1, 0)).x; // Front

    // Sample velocity
    const float4 velocity = read_imagef(inVelocity, velocityPosition);

    // Compute new velocity value (so it has zero divergence)
    const float4 pressureGradient = (float4)(pressureR - pressureL, pressureU - pressureD, pressureF - pressureB, 0) / 2;
    const float4 newVelocity = velocity - pressureGradient;
    write_imagef(outVelocity, velocityPosition, newVelocity);
}
