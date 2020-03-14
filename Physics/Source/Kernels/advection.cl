__kernel void advection3f(__read_only image3d_t inField,
                          __read_only image3d_t inVelocity,
                          float inDeltaTime,
                          float inDissipation,
                          __write_only image3d_t outField) {
    // Thread position
    const int4 threadPosition = (int4)((int)get_global_id(0), (int)get_global_id(1), (int)get_global_id(2), 0);

    // Calculate sample position
    const sampler_t nearestSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
    float4 samplePosition = convert_float4(threadPosition);
    samplePosition -= inDeltaTime * read_imagef(inVelocity, nearestSampler, threadPosition);
    samplePosition += 0.5f;

    // Bilinear sample input buffer and apply dissipation
    const sampler_t linearSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;
    float4 value = read_imagef(inField, linearSampler, samplePosition) * inDissipation;

    // Write to output
    write_imagef(outField, threadPosition, value);
}

__kernel void advection1f(__read_only image3d_t inField,
                          __read_only image3d_t inVelocity,
                          float inDeltaTime,
                          float inDissipation,
                          __write_only image3d_t outField) {
    // Thread position
    const int4 threadPosition = (int4)((int)get_global_id(0), (int)get_global_id(1), (int)get_global_id(2), 0);

    // Calculate sample position
    const sampler_t nearestSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
    float4 samplePosition = convert_float4(threadPosition);
    samplePosition -= inDeltaTime * read_imagef(inVelocity, nearestSampler, threadPosition);
    samplePosition += 0.5f;

    // Bilinear sample input buffer and apply dissipation
    const sampler_t linearSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;
    float value = read_imagef(inField, linearSampler, samplePosition).x * inDissipation;

    // Write to output
    write_imagef(outField, threadPosition, value);
}
