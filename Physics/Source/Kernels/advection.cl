__kernel void advection3f(__read_only image3d_t in,
                          __write_only image3d_t out,
                          __read_only image3d_t velocity,
                          float dt,
                          float dissipation) {
    // Thread position
    const int4 threadPosition = (int4)((int)get_global_id(0), (int)get_global_id(1), (int)get_global_id(2), 0);

    // Handle obstacle
    // if (_Obstacles[tid] > 0.1)
    // {
    //  	_WriteBuf3f[tid] = float3(0, 0, 0);
    //  	return;
    // }

    // Calculate sample position
    const sampler_t nearestSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
    float4 samplePosition = convert_float4(threadPosition);
    samplePosition -= dt * read_imagef(velocity, nearestSampler, threadPosition);
    samplePosition += 0.5;

    // Bilinear sample input buffer and apply dissipation
    const sampler_t linearSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;
    float4 value = read_imagef(in, linearSampler, samplePosition) * dissipation;

    // Write to output
    write_imagef(out, threadPosition, value);
}
