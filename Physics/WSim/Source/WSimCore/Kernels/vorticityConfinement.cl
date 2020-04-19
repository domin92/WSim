__kernel void calculateVorticity(__read_only image3d_t inVelocity,
                                 int4 inVelocityOffset,
                                 __write_only image3d_t outVorticity) {
    // Get current position
    const int4 threadPosition = (int4)((int)get_global_id(0), (int)get_global_id(1), (int)get_global_id(2), 0);

    // Sample neighbouring velocities
    // TODO possible optimization: we never use velocityL.x, velocityU.y, velocityF.z and so on. Maybe discard them early to save registers?
    const sampler_t nearestSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
    const float4 samplePosition = convert_float4(threadPosition + inVelocityOffset) + 0.5f;
    const float4 velocityL = read_imagef(inVelocity, nearestSampler, samplePosition - (float4)(1, 0, 0, 0)); // Left
    const float4 velocityR = read_imagef(inVelocity, nearestSampler, samplePosition + (float4)(1, 0, 0, 0)); // Right
    const float4 velocityD = read_imagef(inVelocity, nearestSampler, samplePosition - (float4)(0, 1, 0, 0)); // Down
    const float4 velocityU = read_imagef(inVelocity, nearestSampler, samplePosition + (float4)(0, 1, 0, 0)); // Up
    const float4 velocityB = read_imagef(inVelocity, nearestSampler, samplePosition - (float4)(0, 0, 1, 0)); // Back
    const float4 velocityF = read_imagef(inVelocity, nearestSampler, samplePosition + (float4)(0, 0, 1, 0)); // Front

    // Compute vorticity (which is the curl of velocity)
    const float curlX = (velocityU.z - velocityD.z) - (velocityF.y - velocityB.y);
    const float curlY = (velocityF.x - velocityB.x) - (velocityR.z - velocityL.z);
    const float curlZ = (velocityR.y - velocityL.y) - (velocityU.x - velocityD.x);
    const float4 curl = (float4)(curlX, curlY, curlZ, 0) / 2;
    write_imagef(outVorticity, threadPosition, curl);
}

__kernel void applyVorticityConfinement(__read_only image3d_t inVelocity,
                                        __read_only image3d_t inVorticity,
                                        int4 inVelocityOffset,
                                        int4 inVorticityOffset,
                                        float inVorticityStrength,
                                        __write_only image3d_t outVelocity) {

    // Get current positions
    const int4 threadPosition = (int4)((int)get_global_id(0), (int)get_global_id(1), (int)get_global_id(2), 0);
    const int4 velocityPosition = threadPosition + inVelocityOffset;
    const int4 vorticityPosition = threadPosition + inVorticityOffset;

    // Sample neighbouring vorticities
    const sampler_t nearestSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
    const float4 samplePosition = convert_float4(vorticityPosition) + 0.5f;
    const float vorticityL = read_imagef(inVorticity, nearestSampler, samplePosition - (float4)(1, 0, 0, 0)).x; // Left
    const float vorticityR = read_imagef(inVorticity, nearestSampler, samplePosition + (float4)(1, 0, 0, 0)).x; // Right
    const float vorticityD = read_imagef(inVorticity, nearestSampler, samplePosition - (float4)(0, 1, 0, 0)).x; // Down
    const float vorticityU = read_imagef(inVorticity, nearestSampler, samplePosition + (float4)(0, 1, 0, 0)).x; // Up
    const float vorticityB = read_imagef(inVorticity, nearestSampler, samplePosition - (float4)(0, 0, 1, 0)).x; // Back
    const float vorticityF = read_imagef(inVorticity, nearestSampler, samplePosition + (float4)(0, 0, 1, 0)).x; // Front

    // Get vorticity and vorticity gradient at current point
    const float4 vorticityGradient = normalize((float4)(vorticityR - vorticityL, vorticityU - vorticityD, vorticityF - vorticityB, 0));
    const float4 vorticity = read_imagef(inVorticity, nearestSampler, samplePosition);

    // Calculate and apply vorticity confinement force
    const float4 force = inVorticityStrength * cross(vorticityGradient, vorticity);
    const float4 oldVelocity = read_imagef(inVelocity, velocityPosition);
    const float4 newVelocity = oldVelocity + force;
    write_imagef(outVelocity, velocityPosition, newVelocity);
}
