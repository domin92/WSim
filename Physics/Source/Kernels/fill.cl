__kernel void fillVelocity(__write_only image3d_t out) {
    const int x = get_global_id(0);
    const int y = get_global_id(1);
    const int z = get_global_id(2);

    const int4 coords = (int4)(x,y,z,0);
    float4 velocity = 0;
    if (x == 0) velocity.x = -2;
    if (x == 1) velocity.x = 0;
    if (x == 2) velocity.x = 1;
    if (x == 3) velocity.x = 3;

    write_imagef(out, coords, velocity);
}