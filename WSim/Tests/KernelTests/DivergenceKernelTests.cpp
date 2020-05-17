#include "Tests/KernelTests/KernelTest.h"

#include <memory>

struct DivergenceKernelTest : KernelTest {
    void SetUp() override {
        KernelTest::SetUp();
        kernelDivergence = createKernelFromFile("pressure.cl", "calculateDivergence");
    }
    cl_kernel kernelDivergence;

    void performTest(Vec3 velocityOffset, Vec3 simulationSize, Vec3 simulationSizeWithBorders, const float *inputData, const float *expectedOutputData) {
        auto velocitySrc = OCL::createReadWriteImage3D(context, simulationSizeWithBorders, vectorFieldFormat);
        auto divergence = OCL::createReadWriteImage3D(context, simulationSize, scalarFieldFormat);
        OCL::enqueueWriteImage3D(queue, velocitySrc, CL_FALSE, simulationSizeWithBorders, inputData);
        OCL::setKernelArgMem(kernelDivergence, 0, velocitySrc);                                          // inVelocity
        OCL::setKernelArgVec(kernelDivergence, 1, velocityOffset.x, velocityOffset.y, velocityOffset.z); // inVelocityOffset
        OCL::setKernelArgMem(kernelDivergence, 2, divergence);                                           // outDivergence
        OCL::enqueueKernel3D(queue, kernelDivergence, simulationSize);

        const auto requiredBufferSize = simulationSize.getRequiredBufferSize(4u);
        auto outputData = std::make_unique<float[]>(requiredBufferSize);
        OCL::enqueueReadImage3D(queue, divergence, CL_TRUE, simulationSize, outputData.get());
        EXPECT_MEM_EQ(expectedOutputData, outputData.get(), requiredBufferSize);
    }
};

TEST_F(DivergenceKernelTest, divergenceX) {
    const Vec3 velocityOffset{};
    const Vec3 imageSize{4, 4, 1};
    const float inputData[] = {
        -2, 0, 0, 0, /**/ +1, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 1, 0, 0, 0,
        +2, 0, 0, 0, /**/ +2, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 3, 0, 0, 0,
        +1, 0, 0, 0, /**/ -1, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 1, 0, 0, 0,
        -1, 0, 0, 0, /**/ -1, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 1, 0, 0, 0};
    const float expectedOutputData[] = {
        +1.5, /**/ +1.0, /**/ +0.0, /**/ 0.5,
        +0.0, /**/ -1.0, /**/ +0.5, /**/ 1.5,
        -1.0, /**/ -0.5, /**/ +1.0, /**/ 0.5,
        +0.0, /**/ +0.5, /**/ +1.0, /**/ 0.5};
    performTest(velocityOffset, imageSize, imageSize, inputData, expectedOutputData);
}

TEST_F(DivergenceKernelTest, divergenceY) {
    const Vec3 velocityOffset{};
    const Vec3 imageSize{4, 4, 1};
    const float inputData[] = {
        0, -2, 0, 0, /**/ 0, +1, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 1, 0, 0,
        0, +2, 0, 0, /**/ 0, +2, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 3, 0, 0,
        0, +1, 0, 0, /**/ 0, -1, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 1, 0, 0,
        0, -1, 0, 0, /**/ 0, -1, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 1, 0, 0};
    const float expectedOutputData[] = {
        +2.0, /**/ +0.5, /**/ 0, /**/ +1,
        +1.5, /**/ -1.0, /**/ 0, /**/ +0,
        -1.5, /**/ -1.5, /**/ 0, /**/ -1,
        -1.0, /**/ +0.0, /**/ 0, /**/ +0};
    performTest(velocityOffset, imageSize, imageSize, inputData, expectedOutputData);
}

TEST_F(DivergenceKernelTest, divergenceXY) {
    const Vec3 velocityOffset{};
    const Vec3 imageSize{4, 4, 1};
    const float inputData[] = {
        -2, -2, 0, 0, /**/ +1, +1, 0, 0, /**/ 0, 0, 0, 0, /**/ 1, 1, 0, 0,
        +2, +2, 0, 0, /**/ +2, +2, 0, 0, /**/ 0, 0, 0, 0, /**/ 3, 3, 0, 0,
        +1, +1, 0, 0, /**/ -1, -1, 0, 0, /**/ 0, 0, 0, 0, /**/ 1, 1, 0, 0,
        -1, -1, 0, 0, /**/ -1, -1, 0, 0, /**/ 0, 0, 0, 0, /**/ 1, 1, 0, 0};
    const float expectedOutputData[] = {
        +3.5, /**/ +1.5, /**/ +0.0, /**/ +1.5,
        +1.5, /**/ -2.0, /**/ +0.5, /**/ +1.5,
        -2.5, /**/ -2.0, /**/ +1.0, /**/ -0.5,
        -1.0, /**/ +0.5, /**/ +1.0, /**/ +0.5};
    performTest(velocityOffset, imageSize, imageSize, inputData, expectedOutputData);
}

TEST_F(DivergenceKernelTest, divergenceLaminarFlow) {
    const Vec3 velocityOffset{};
    const Vec3 imageSize{4, 4, 3};
    const float inputData[] = {
        +6, 6, 6, 0, /**/ +6, 6, 6, 0, /**/ +6, 6, 6, 0, /**/ +6, 6, 6, 0,
        +6, 6, 6, 0, /**/ +6, 6, 6, 0, /**/ +6, 6, 6, 0, /**/ +6, 6, 6, 0,
        +6, 6, 6, 0, /**/ +6, 6, 6, 0, /**/ +6, 6, 6, 0, /**/ +6, 6, 6, 0,
        +6, 6, 6, 0, /**/ +6, 6, 6, 0, /**/ +6, 6, 6, 0, /**/ +6, 6, 6, 0,
        /*            **                **                **/
        +6, 6, 6, 0, /**/ +6, 6, 6, 0, /**/ +6, 6, 6, 0, /**/ +6, 6, 6, 0,
        +6, 6, 6, 0, /**/ +6, 6, 6, 0, /**/ +6, 6, 6, 0, /**/ +6, 6, 6, 0,
        +6, 6, 6, 0, /**/ +6, 6, 6, 0, /**/ +6, 6, 6, 0, /**/ +6, 6, 6, 0,
        +6, 6, 6, 0, /**/ +6, 6, 6, 0, /**/ +6, 6, 6, 0, /**/ +6, 6, 6, 0,
        /*            **                **                **/
        +6, 6, 6, 0, /**/ +6, 6, 6, 0, /**/ +6, 6, 6, 0, /**/ +6, 6, 6, 0,
        +6, 6, 6, 0, /**/ +6, 6, 6, 0, /**/ +6, 6, 6, 0, /**/ +6, 6, 6, 0,
        +6, 6, 6, 0, /**/ +6, 6, 6, 0, /**/ +6, 6, 6, 0, /**/ +6, 6, 6, 0,
        +6, 6, 6, 0, /**/ +6, 6, 6, 0, /**/ +6, 6, 6, 0, /**/ +6, 6, 6, 0};
    const float expectedOutputData[] = {
        0, /**/ 0, /**/ 0, /**/ 0,
        0, /**/ 0, /**/ 0, /**/ 0,
        0, /**/ 0, /**/ 0, /**/ 0,
        0, /**/ 0, /**/ 0, /**/ 0,
        /*  **      **      **/
        0, /**/ 0, /**/ 0, /**/ 0,
        0, /**/ 0, /**/ 0, /**/ 0,
        0, /**/ 0, /**/ 0, /**/ 0,
        0, /**/ 0, /**/ 0, /**/ 0,
        /*  **      **      **/
        0, /**/ 0, /**/ 0, /**/ 0,
        0, /**/ 0, /**/ 0, /**/ 0,
        0, /**/ 0, /**/ 0, /**/ 0,
        0, /**/ 0, /**/ 0, /**/ 0};
    performTest(velocityOffset, imageSize, imageSize, inputData, expectedOutputData);
}

TEST_F(DivergenceKernelTest, divergenceXWithOffset) {
    const Vec3 velocityOffset{1, 1, 0};
    const Vec3 simluationSize{3, 3, 1};
    const Vec3 simluationSizeWithBorders{4, 4, 1};
    const float inputData[] = {
        -2, 0, 0, 0, /**/ +1, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 1, 0, 0, 0,
        +2, 0, 0, 0, /**/ +2, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 3, 0, 0, 0,
        +1, 0, 0, 0, /**/ -1, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 1, 0, 0, 0,
        -1, 0, 0, 0, /**/ -1, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 1, 0, 0, 0};
    const float expectedOutputData[] = {
        +1.0, /**/ +0.0, /**/ 0.5,
        -1.0, /**/ +0.5, /**/ 1.5,
        -0.5, /**/ +1.0, /**/ 0.5};
    performTest(velocityOffset, simluationSize, simluationSizeWithBorders, inputData, expectedOutputData);
}
