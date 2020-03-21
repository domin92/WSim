#include "KernelTests/KernelTest.h"
#include "TestUtils/TestUtils.h"

#include <memory>

struct DivergenceKernelTest : KernelTest {
    void SetUp() override {
        KernelTest::SetUp();
        kernelDivergence = createKernelFromFile("pressure/divergence.cl", "calculate_divergence");
    }
    cl_kernel kernelDivergence;

    void performTest(OCL::Vec3 imageSize, const float *inputData, const float *expectedOutputData) {
        auto velocitySrc = OCL::createReadWriteImage3D(context, imageSize, vectorFieldFormat);
        auto divergence = OCL::createReadWriteImage3D(context, imageSize, scalarFieldFormat);
        OCL::enqueueWriteImage3D(queue, velocitySrc, CL_FALSE, imageSize, inputData);
        OCL::setKernelArgMem(kernelDivergence, 0, velocitySrc); // inVelocity
        OCL::setKernelArgMem(kernelDivergence, 1, divergence);  // outDivergence
        OCL::enqueueKernel3D(queue, kernelDivergence, imageSize);

        const auto requiredBufferSize = imageSize.getRequiredBufferSize(4u);
        auto outputData = std::make_unique<float[]>(requiredBufferSize);
        OCL::enqueueReadImage3D(queue, divergence, CL_TRUE, imageSize, outputData.get());
        EXPECT_MEM_EQ(expectedOutputData, outputData.get(), requiredBufferSize);
    }
};

TEST_F(DivergenceKernelTest, divergenceX) {
    const OCL::Vec3 imageSize{4, 4, 1};
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
    performTest(imageSize, inputData, expectedOutputData);
}

TEST_F(DivergenceKernelTest, divergenceY) {
    const OCL::Vec3 imageSize{4, 4, 1};
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
    performTest(imageSize, inputData, expectedOutputData);
}

TEST_F(DivergenceKernelTest, divergenceXY) {
    const OCL::Vec3 imageSize{4, 4, 1};
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
    performTest(imageSize, inputData, expectedOutputData);
}

TEST_F(DivergenceKernelTest, divergenceLaminarFlow) {
    const OCL::Vec3 imageSize{4, 4, 3};
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
    performTest(imageSize, inputData, expectedOutputData);
}
