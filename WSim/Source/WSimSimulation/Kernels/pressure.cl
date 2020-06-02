__kernel void calculateDivergence(__read_only image3d_t inVelocity,
                                  int4 inVelocityOffset,
                                  __write_only image3d_t outDivergence) {
    // Get current positions
    const int4 threadPosition = (int4)((int)get_global_id(0), (int)get_global_id(1), (int)get_global_id(2), 0);
    const int4 velocityPosition = threadPosition + inVelocityOffset;

    // Sample neighbouring velocities
    const sampler_t nearestSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
    const float4 samplePosition = convert_float4(velocityPosition) + 0.5f;
    const float velocityL = read_imagef(inVelocity, nearestSampler, samplePosition - (float4)(1, 0, 0, 0)).x; // Left
    const float velocityR = read_imagef(inVelocity, nearestSampler, samplePosition + (float4)(1, 0, 0, 0)).x; // Right
    const float velocityD = read_imagef(inVelocity, nearestSampler, samplePosition - (float4)(0, 1, 0, 0)).y; // Down
    const float velocityU = read_imagef(inVelocity, nearestSampler, samplePosition + (float4)(0, 1, 0, 0)).y; // Up
    const float velocityB = read_imagef(inVelocity, nearestSampler, samplePosition - (float4)(0, 0, 1, 0)).z; // Back
    const float velocityF = read_imagef(inVelocity, nearestSampler, samplePosition + (float4)(0, 0, 1, 0)).z; // Front

    // Compute divergence
    const float divergence = ((velocityR - velocityL) + (velocityU - velocityD) + (velocityF - velocityB)) / 2;
    write_imagef(outDivergence, threadPosition, divergence);
}

int getLocalMemIndex(int4 workGroupSize, int4 localPosition) {
    return localPosition.x * workGroupSize.y * workGroupSize.z +
           localPosition.y * workGroupSize.z +
           localPosition.z;
}

bool isWithin(int id, int minInclusive, int maxExclusive) {
    return minInclusive <= id && id < maxExclusive;
}

bool isWithinXYZ(int4 id, int4 minInclusive, int4 maxExclusive) {
    return isWithin(id.x, minInclusive.x, maxExclusive.x) &&
           isWithin(id.y, minInclusive.y, maxExclusive.y) &&
           isWithin(id.z, minInclusive.z, maxExclusive.z);
}

float sampleFromLocalOrGlobal(__read_only image3d_t image,
                              int4 offset,
                              int4 workGroupSize,
                              int4 threadPositionLocal,           // for localMem
                              __local float *localMem,            // for localMem
                              float4 samplePositionWithoutOffset, // for globalMem
                              sampler_t nearestSampler)           // for globalMem
{
    const int4 voxelPositionLocal = threadPositionLocal + offset;
    if (isWithinXYZ(voxelPositionLocal, (int4)(0), workGroupSize)) {
        return localMem[getLocalMemIndex(workGroupSize, voxelPositionLocal)];
    } else {
        const float4 samplePosition = samplePositionWithoutOffset + convert_float4(offset);
        return read_imagef(image, nearestSampler, samplePosition).x;
    }
}

__kernel void calculatePressureWithJacobiIteration(__read_only image3d_t inDivergence,
                                                   __read_only image3d_t inPressure,
                                                   int4 pressureAndDivergenceOffset,
                                                   __write_only image3d_t outPressure,
                                                   __local float *localMem) {
    // Get current positions
    const int4 threadPosition = (int4)((int)get_global_id(0), (int)get_global_id(1), (int)get_global_id(2), 0);
    const int4 pressurePosition = threadPosition + pressureAndDivergenceOffset;
    const int4 threadPositionLocal = (int4)((int)get_local_id(0), (int)get_local_id(1), (int)get_local_id(2), 0);
    const int4 workGroupSize = (int4)((int)get_local_size(0), (int)get_local_size(1), (int)get_local_size(2), 0);

    // Each threads samples own voxel
    const sampler_t nearestSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
    const float4 samplePosition = convert_float4(pressurePosition) + 0.5f;
    const float myPressure = read_imagef(inPressure, nearestSampler, samplePosition).x;

    // Voxel is stored in local memory
    localMem[getLocalMemIndex(workGroupSize, threadPositionLocal)] = myPressure;

    // Synchronize all writes to local Memory
    barrier(CLK_LOCAL_MEM_FENCE);

    // Get neighbouring pressures
    const float pressureL = sampleFromLocalOrGlobal(inPressure, (int4)(-1, 0, 0, 0), workGroupSize, threadPositionLocal, localMem, samplePosition, nearestSampler);
    const float pressureR = sampleFromLocalOrGlobal(inPressure, (int4)(+1, 0, 0, 0), workGroupSize, threadPositionLocal, localMem, samplePosition, nearestSampler);
    const float pressureD = sampleFromLocalOrGlobal(inPressure, (int4)(0, -1, 0, 0), workGroupSize, threadPositionLocal, localMem, samplePosition, nearestSampler);
    const float pressureU = sampleFromLocalOrGlobal(inPressure, (int4)(0, +1, 0, 0), workGroupSize, threadPositionLocal, localMem, samplePosition, nearestSampler);
    const float pressureB = sampleFromLocalOrGlobal(inPressure, (int4)(0, 0, -1, 0), workGroupSize, threadPositionLocal, localMem, samplePosition, nearestSampler);
    const float pressureF = sampleFromLocalOrGlobal(inPressure, (int4)(0, 0, +1, 0), workGroupSize, threadPositionLocal, localMem, samplePosition, nearestSampler);

    // Sample divergence
    const float divergence = read_imagef(inDivergence, pressurePosition).x;

    // Compute new pressure value
    const float newPressure = (pressureL + pressureR + pressureD + pressureU + pressureB + pressureF - divergence) / 6.0;
    write_imagef(outPressure, pressurePosition, newPressure);
}

__kernel void projectVelocityToDivergenceFree(__read_only image3d_t inVelocity,
                                              __read_only image3d_t inPressure,
                                              int4 inVelocityOffset,
                                              int4 inPressureOffset,
                                              __write_only image3d_t outVelocity) {
    // Get current positions
    const int4 threadPosition = (int4)((int)get_global_id(0), (int)get_global_id(1), (int)get_global_id(2), 0);
    const int4 velocityPosition = threadPosition + inVelocityOffset;
    const int4 pressurePosition = threadPosition + inPressureOffset;

    // Sample neighbouring pressures
    const sampler_t nearestSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
    const float4 samplePosition = convert_float4(pressurePosition) + 0.5f;
    const float pressureL = read_imagef(inPressure, nearestSampler, samplePosition - (float4)(1, 0, 0, 0)).x; // Left
    const float pressureR = read_imagef(inPressure, nearestSampler, samplePosition + (float4)(1, 0, 0, 0)).x; // Right
    const float pressureD = read_imagef(inPressure, nearestSampler, samplePosition - (float4)(0, 1, 0, 0)).x; // Down
    const float pressureU = read_imagef(inPressure, nearestSampler, samplePosition + (float4)(0, 1, 0, 0)).x; // Up
    const float pressureB = read_imagef(inPressure, nearestSampler, samplePosition - (float4)(0, 0, 1, 0)).x; // Back
    const float pressureF = read_imagef(inPressure, nearestSampler, samplePosition + (float4)(0, 0, 1, 0)).x; // Front

    // Sample velocity
    const float4 velocitySamplePosition = convert_float4(velocityPosition) + 0.5f;
    const float4 velocity = read_imagef(inVelocity, nearestSampler, velocitySamplePosition);

    // Compute new velocity value (so it has zero divergence)
    const float4 pressureGradient = (float4)(pressureR - pressureL, pressureU - pressureD, pressureF - pressureB, 0) / 2;
    const float4 newVelocity = velocity - pressureGradient;
    write_imagef(outVelocity, velocityPosition, newVelocity);
}
