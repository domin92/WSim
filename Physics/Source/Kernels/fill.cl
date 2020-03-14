__kernel void fillVelocity(__write_only image3d_t out) {
    const int4 threadPosition = (int4)((int)get_global_id(0), (int)get_global_id(1), (int)get_global_id(2), 0);
    const float x = get_global_id(0);
    const float y = get_global_id(1);
    const float4 velocity = (float4)(   sin(6 * y), sin(6 * x), 0, 0  );
    write_imagef(out, threadPosition, velocity);
}
