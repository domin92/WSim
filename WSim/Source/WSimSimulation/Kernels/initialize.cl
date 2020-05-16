__kernel void initializeColor(int4 inImageSize,
                              int4 inOffset,
                              __write_only image3d_t outColor) {
    const int4 threadPosition = (int4)((int)get_global_id(0), (int)get_global_id(1), (int)get_global_id(2), 0);
    const float squaresPerLine = 4;
    const float divider = (float)(inImageSize.x) / squaresPerLine;

    const int x = inImageSize.x + (threadPosition.x - inOffset.x) / divider;
    const int y = inImageSize.y + (threadPosition.y - inOffset.y) / divider;
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
