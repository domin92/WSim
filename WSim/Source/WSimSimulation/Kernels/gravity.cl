__kernel void applyGravity(__read_only image3d_t inVelocity,
                           __read_only image3d_t inColor,
                           int4 inVelocityOffset,
                           float4 inDownDirection,
                           float inGravityForce,
                           __write_only image3d_t outVelocity) {
    // Get current position
    const int4 threadPosition = (int4)((int)get_global_id(0), (int)get_global_id(1), (int)get_global_id(2), 0);
    const int4 velocityPosition = threadPosition + inVelocityOffset; // can be also used for accessing color

    // Sample current velocity and color
    const sampler_t nearestSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
    const float4 samplePosition = convert_float4(velocityPosition) + 0.5f;
    const float4 currentVelocity = read_imagef(inVelocity, nearestSampler, samplePosition);
    const float4 color = read_imagef(inColor, nearestSampler, samplePosition);

    // Calculate change of velocity to be applied
    float4 velocityChange = inDownDirection * inGravityForce;
    if (color.x > 0.3 || color.y > 0.3 || color.z < 0.2) {
        velocityChange = (float4)(0);
    }

    // Write to output buffer
    float4 newVelocity = currentVelocity + velocityChange;
    write_imagef(outVelocity, velocityPosition, newVelocity);
}
