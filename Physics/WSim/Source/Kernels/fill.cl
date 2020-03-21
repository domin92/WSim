__kernel void fillVelocity(float inImageSize,
                           __write_only image3d_t outVelocity) {
    const int4 threadPosition = (int4)((int)get_global_id(0), (int)get_global_id(1), (int)get_global_id(2), 0);
    const float4 center = (float4)(inImageSize) / 2;
    float4 r = convert_float4(threadPosition) - center;
    r /= (inImageSize / 3);
    float4 velocity = cross(r, (float4)(0, 0, 1, 0));

    const float threshold = inImageSize / 10;
    if (threadPosition.x < threshold) {
        velocity.x += 3;
    }
    if (threadPosition.x > inImageSize - threshold) {
        velocity.x -= 3;
    }
    if (threadPosition.y < threshold) {
        velocity.y += 3;
    }
    if (threadPosition.y > inImageSize - threshold) {
        velocity.y -= 3;
    }

    velocity *= 0;
    write_imagef(outVelocity, threadPosition, velocity);
}

__kernel void fillColor(float inImageSize,
                        __write_only image3d_t outColor) {
    const int4 threadPosition = (int4)((int)get_global_id(0), (int)get_global_id(1), (int)get_global_id(2), 0);
    const float squaresPerLine = 4;
    const float divider = inImageSize / squaresPerLine;

    const int x = threadPosition.x / divider;
    const int y = threadPosition.y / divider;
    const int c = (int)(x + y);

    float4 color = (float4)(0, 0, 0, 0);
    if (c % 4 == 0) {
        color.y = 1;
    } else if (c % 4 == 1) {
        color.x = 1;
    } else if (c % 4 == 2) {
        color.x = 1;
        color.y = 1;
    } else {
        color.z = 1;
    }

    write_imagef(outColor, threadPosition, color);
}
