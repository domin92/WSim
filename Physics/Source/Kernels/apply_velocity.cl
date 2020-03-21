__kernel void applyVelocity(__read_only image3d_t inVelocity,
                            float4 inCenter,
                            float4 inVelocityChange,
                            float inRadius,
                            __write_only image3d_t outVelocity) {
    // Get current position
    const int4 threadPosition = (int4)((int)get_global_id(0), (int)get_global_id(1), (int)get_global_id(2), 0);

    // Sample current value
    const sampler_t nearestSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
    const float4 samplePosition = convert_float4(threadPosition) + 0.5f;
    const float4 currentVelocity = read_imagef(inVelocity, nearestSampler, samplePosition);

    // Calculate change of velocity to be applied
    const float distance = length(convert_float4(threadPosition) - inCenter);
    const float4 velocityChange = -inVelocityChange * exp(-distance / inRadius);

    // Write to output buffer
    float4 newVelocity = currentVelocity + velocityChange;
    write_imagef(outVelocity, threadPosition, newVelocity);
}
