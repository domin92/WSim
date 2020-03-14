__kernel void solve_jacobi_iteration(__read_only image3d_t inDivergence,
                                     __read_only image3d_t inPressure,
                                     __write_only image3d_t outPressure) {
    // Get current position
    const int4 threadPosition = (int4)((int)get_global_id(0), (int)get_global_id(1), (int)get_global_id(2), 0);

    // Sample neighbouring pressures
    const sampler_t nearestSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
    const float4 samplePosition = convert_float4(threadPosition) + 0.5f;
    const float pressureL = read_imagef(inPressure, nearestSampler, samplePosition - (float4)(1, 0, 0, 0)).x; // Left
    const float pressureR = read_imagef(inPressure, nearestSampler, samplePosition + (float4)(1, 0, 0, 0)).x; // Right
    const float pressureD = read_imagef(inPressure, nearestSampler, samplePosition - (float4)(0, 1, 0, 0)).x; // Down
    const float pressureU = read_imagef(inPressure, nearestSampler, samplePosition + (float4)(0, 1, 0, 0)).x; // Up
    const float pressureB = read_imagef(inPressure, nearestSampler, samplePosition - (float4)(0, 0, 1, 0)).x; // Back
    const float pressureF = read_imagef(inPressure, nearestSampler, samplePosition + (float4)(0, 0, 1, 0)).x; // Front

    // Sample divergence
    const float divergence = read_imagef(inDivergence, threadPosition).x;

    // Compute new pressure value
    const float newPressure = (pressureL + pressureR + pressureD + pressureU + pressureB + pressureF - divergence) / 6.0;
    write_imagef(outPressure, threadPosition, newPressure);
}
