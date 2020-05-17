#include "Tests/KernelTests/KernelTest.h"

struct AdvectionKernelTest : KernelTest {
    void SetUp() override {
        KernelTest::SetUp();
        kernelAdvection = createKernelFromFile("advection.cl", "advectVelocity");
    }
    cl_kernel kernelAdvection;

    void performTest(Vec3 imageSize, float deltaTime, const float *inputData, const float *expectedOutputData) {
        auto obstacles = OCL::createReadWriteImage3D(context, imageSize, scalarFieldFormat);
        OCL::enqueueZeroImage3D(queue, obstacles, imageSize);

        auto velocitySrc = OCL::createReadWriteImage3D(context, imageSize, vectorFieldFormat);
        auto velocityDst = OCL::createReadWriteImage3D(context, imageSize, vectorFieldFormat);
        OCL::enqueueWriteImage3D(queue, velocitySrc, CL_FALSE, imageSize, inputData);

        OCL::setKernelArgMem(kernelAdvection, 0, velocitySrc);   // inVelocity
        OCL::setKernelArgMem(kernelAdvection, 1, obstacles);     // inObstacles
        OCL::setKernelArgVec(kernelAdvection, 2, 0.f, 0.f, 0.f); // inVelocityOffset
        OCL::setKernelArgFlt(kernelAdvection, 3, deltaTime);     // inDeltaTime
        OCL::setKernelArgFlt(kernelAdvection, 4, 1.f);           // inDissipation
        OCL::setKernelArgMem(kernelAdvection, 5, velocityDst);   // outField
        OCL::enqueueKernel3D(queue, kernelAdvection, imageSize);

        const auto requiredBufferSize = imageSize.getRequiredBufferSize(4u);
        auto outputData = std::make_unique<float[]>(requiredBufferSize);
        OCL::enqueueReadImage3D(queue, velocityDst, CL_TRUE, imageSize, outputData.get());
        EXPECT_MEM_EQ(expectedOutputData, outputData.get(), requiredBufferSize);
    }
};

TEST_F(AdvectionKernelTest, velocityAdvectionSimple) {
    const Vec3 testImageSize{4, 4, 1};
    const float inputData[] = {
        -2, 0, 0, 0, /**/ +1, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 1, 0, 0, 0,
        +2, 0, 0, 0, /**/ +2, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 3, 0, 0, 0,
        +1, 0, 0, 0, /**/ -1, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 1, 0, 0, 0,
        -1, 0, 0, 0, /**/ -1, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 1, 0, 0, 0};
    const float expectedOutputData[] = {
        +0, 0, 0, 0, /**/ -2, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0,
        +2, 0, 0, 0, /**/ +2, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0,
        +1, 0, 0, 0, /**/ +0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0,
        -1, 0, 0, 0, /**/ +0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0};
    performTest(testImageSize, 1.f, inputData, expectedOutputData);
}

TEST_F(AdvectionKernelTest, velocityAdvectionBilinearInterpolation) {
    const Vec3 testImageSize{4, 4, 1};
    const float inputData[] = {
        -2, 0, 0, 0, /**/ +1, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 1, 0, 0, 0,
        +2, 0, 0, 0, /**/ +2, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 3, 0, 0, 0,
        +1, 0, 0, 0, /**/ -1, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 1, 0, 0, 0,
        -1, 0, 0, 0, /**/ -1, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 1, 0, 0, 0};
    const float expectedOutputData[] = {
        +1, 0, 0, 0, /**/ -0.5, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0.5, 0, 0, 0,
        +2, 0, 0, 0, /**/ +2.0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 1.0, 0, 0, 0,
        +1, 0, 0, 0, /**/ -0.5, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0.5, 0, 0, 0,
        -1, 0, 0, 0, /**/ -0.5, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0.5, 0, 0, 0};
    performTest(testImageSize, 0.5f, inputData, expectedOutputData);
}
