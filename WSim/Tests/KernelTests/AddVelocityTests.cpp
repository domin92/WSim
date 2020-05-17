#include "Tests/KernelTests/KernelTest.h"

struct AddVelocityTest : KernelTest {
    void SetUp() override {
        KernelTest::SetUp();
        kernelAddVelocity = createKernelFromFile("addVelocity.cl", "addVelocity");
    }
    cl_kernel kernelAddVelocity;

    void performTest(Vec3 imageSize, const float center[], const float velocityChange[], float radius,
                     const float *inputData, const float *expectedOutputData, const Vec3 *verifiedImageSize = nullptr) {
        auto velocitySrc = OCL::createReadWriteImage3D(context, imageSize, vectorFieldFormat);
        auto velocityDst = OCL::createReadWriteImage3D(context, imageSize, vectorFieldFormat);
        OCL::enqueueWriteImage3D(queue, velocitySrc, CL_FALSE, imageSize, inputData);
        OCL::setKernelArgMem(kernelAddVelocity, 0, velocitySrc);                                             // inVelocity
        OCL::setKernelArgVec(kernelAddVelocity, 1, 0.f, 0.f, 0.f);                                           // inVelocityOffset
        OCL::setKernelArgVec(kernelAddVelocity, 2, center[0], center[1], center[2]);                         // inCenter
        OCL::setKernelArgVec(kernelAddVelocity, 3, velocityChange[0], velocityChange[1], velocityChange[2]); // inVelocityChange
        OCL::setKernelArgFlt(kernelAddVelocity, 4, radius);                                                  // inRadius
        OCL::setKernelArgMem(kernelAddVelocity, 5, velocityDst);                                             // outVelocity
        OCL::enqueueKernel3D(queue, kernelAddVelocity, imageSize);

        if (verifiedImageSize == nullptr) {
            verifiedImageSize = &imageSize;
        }

        const auto requiredBufferSize = verifiedImageSize->getRequiredBufferSize(4u);
        auto outputData = std::make_unique<float[]>(requiredBufferSize);
        OCL::enqueueReadImage3D(queue, velocityDst, CL_TRUE, *verifiedImageSize, outputData.get());
        EXPECT_FLOATS_EQ(expectedOutputData, outputData.get(), requiredBufferSize, 0.00001f);
    }
};

TEST_F(AddVelocityTest, givenCenterAtOriginWhenAddingVelocityThenGiveCorrectResults) {
    const Vec3 testImageSize{4, 4, 1};
    const float center[] = {0, 0, 0};
    const float velocityChange[] = {3, -8, 0};
    const float radius = 1;
    const float inputData[] = {
        0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0,
        0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0,
        0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0,
        0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0};
    const float expectedOutputData[] = {
        +3.0000000f, -8.0000000f, 0, 0, /**/ +1.1036391f, -2.9430375f, 0, 0, /**/ +0.4060064f, -1.0826837f, 0, 0, /**/ +0.1493615f, -0.3982974f, 0, 0,
        +1.1036391f, -2.9430375f, 0, 0, /**/ +0.7293509f, -1.9449357f, 0, 0, /**/ +0.3206343f, -0.8550247f, 0, 0, /**/ +0.1269879f, -0.3386345f, 0, 0,
        +0.4060064f, -1.0826837f, 0, 0, /**/ +0.3206343f, -0.8550247f, 0, 0, /**/ +0.1773176f, -0.4728469f, 0, 0, /**/ +0.0815176f, -0.2173802f, 0, 0,
        +0.1493615f, -0.3982974f, 0, 0, /**/ +0.1269879f, -0.3386345f, 0, 0, /**/ +0.0815176f, -0.2173802f, 0, 0, /**/ +0.0431089f, -0.1149571f, 0, 0};
    performTest(testImageSize, center, velocityChange, radius, inputData, expectedOutputData);
}

TEST_F(AddVelocityTest, givenSomeValuesPresentInVelocityWhenAddingVelocityThenCalculatedChangeIsSummedWithPreviousValues) {
    const Vec3 testImageSize{4, 4, 1};
    const float center[] = {0, 0, 0};
    const float velocityChange[] = {3, -8, 0};
    const float radius = 1;
    const float inputData[] = {
        1, 1, 0, 0, /**/ 1, 1, 0, 0, /**/ 1, 1, 0, 0, /**/ 1, 1, 0, 0,
        1, 1, 0, 0, /**/ 1, 1, 0, 0, /**/ 1, 1, 0, 0, /**/ 1, 1, 0, 0,
        1, 1, 0, 0, /**/ 1, 1, 0, 0, /**/ 1, 1, 0, 0, /**/ 1, 1, 0, 0,
        1, 1, 0, 0, /**/ 1, 1, 0, 0, /**/ 1, 1, 0, 0, /**/ 1, 1, 0, 0};
    const float expectedOutputData[] = {
        +4.0000000f, -7.0000000f, 0, 0, /**/ +2.1036391f, -1.9430375f, 0, 0, /**/ +1.4060064f, -0.0826837f, 0, 0, /**/ +1.1493615f, +0.6017026f, 0, 0,
        +2.1036391f, -1.9430375f, 0, 0, /**/ +1.7293509f, -0.9449357f, 0, 0, /**/ +1.3206343f, +0.1449753f, 0, 0, /**/ +1.1269879f, +0.6613655f, 0, 0,
        +1.4060064f, -0.0826837f, 0, 0, /**/ +1.3206343f, +0.1449753f, 0, 0, /**/ +1.1773176f, +0.5271531f, 0, 0, /**/ +1.0815176f, +0.7826198f, 0, 0,
        +1.1493615f, +0.6017026f, 0, 0, /**/ +1.1269879f, +0.6613655f, 0, 0, /**/ +1.0815176f, +0.7826198f, 0, 0, /**/ +1.0431089f, +0.8850429f, 0, 0};
    performTest(testImageSize, center, velocityChange, radius, inputData, expectedOutputData);
}

TEST_F(AddVelocityTest, givenSelectedRadiusWhenAddingVelocityThenGiveCorrectResults) {
    const Vec3 testImageSize{4, 4, 1};
    const float center[] = {0, 0, 0};
    const float velocityChange[] = {3, -8, 0};
    const float radius = 5;
    const float inputData[] = {
        0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0,
        0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0,
        0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0,
        0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0};
    const float expectedOutputData[] = {
        +3.0000000f, -8.0000000f, 0, 0, /**/ +2.4561926f, -6.5498469f, 0, 0, /**/ +2.0109607f, -5.3625618f, 0, 0, /**/ +1.6464356f, -4.3904949f, 0, 0,
        +2.4561926f, -6.5498469f, 0, 0, /**/ +2.2609154f, -6.0291077f, 0, 0, /**/ +1.9182225f, -5.1152601f, 0, 0, /**/ +1.5938575f, -4.2502867f, 0, 0,
        +2.0109607f, -5.3625618f, 0, 0, /**/ +1.9182225f, -5.1152601f, 0, 0, /**/ +1.7039128f, -4.5437674f, 0, 0, /**/ +1.4586371f, -3.8896990f, 0, 0,
        +1.6464356f, -4.3904949f, 0, 0, /**/ +1.5938575f, -4.2502867f, 0, 0, /**/ +1.4586371f, -3.8896990f, 0, 0, /**/ +1.2841342f, -3.4243579f, 0, 0};
    performTest(testImageSize, center, velocityChange, radius, inputData, expectedOutputData);
}

TEST_F(AddVelocityTest, givenNonZeroCenterWhenAddingVelocityThenGiveCorrectResults) {
    const Vec3 testImageSize{4, 4, 1};
    const float center[] = {2.5f, 3.2f, 0};
    const float velocityChange[] = {3, -8, 0};
    const float radius = 1;
    const float inputData[] = {
        0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0,
        0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0,
        0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0,
        0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0};
    const float expectedOutputData[] = {
        +0.0517064f, -0.1378838f, 0, 0, /**/ +0.0875535f, -0.2334761f, 0, 0, /**/ +0.1176298f, -0.3136796f, 0, 0, /**/ +0.1176298f, -0.3136796f, 0, 0,
        +0.1073618f, -0.2862982f, 0, 0, /**/ +0.2092781f, -0.5580749f, 0, 0, /**/ +0.3142743f, -0.8380649f, 0, 0, /**/ +0.3142743f, -0.8380649f, 0, 0,
        +0.1874073f, -0.4997529f, 0, 0, /**/ +0.4394094f, -1.1717584f, 0, 0, /**/ +0.8175961f, -2.1802563f, 0, 0, /**/ +0.8175961f, -2.1802563f, 0, 0,
        +0.2442963f, -0.6514569f, 0, 0, /**/ +0.6605640f, -1.7615040f, 0, 0, /**/ +1.7508409f, -4.6689090f, 0, 0, /**/ +1.7508409f, -4.6689090f, 0, 0};
    performTest(testImageSize, center, velocityChange, radius, inputData, expectedOutputData);
}

TEST_F(AddVelocityTest, given3DVelocityWhenAddingVelocityThenZerothPlaneIsTheSameAsIfItWas2D) {
    const Vec3 testImageSize{4, 4, 3};
    const Vec3 verifiedImageSize{4, 4, 1};
    const float center[] = {2.5f, 3.2f, 0};
    const float velocityChange[] = {3, -8, 0};
    const float radius = 1;
    const float inputData[] = {
        0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0,
        0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0,
        0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0,
        0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0,
        /*           **               **               **/
        0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0,
        0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0,
        0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0,
        0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0,
        /*           **               **               **/
        0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0,
        0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0,
        0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0,
        0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0};
    const float expectedOutputData[] = {
        +0.0517064f, -0.1378838f, 0, 0, /**/ +0.0875535f, -0.2334761f, 0, 0, /**/ +0.1176298f, -0.3136796f, 0, 0, /**/ +0.1176298f, -0.3136796f, 0, 0,
        +0.1073618f, -0.2862982f, 0, 0, /**/ +0.2092781f, -0.5580749f, 0, 0, /**/ +0.3142743f, -0.8380649f, 0, 0, /**/ +0.3142743f, -0.8380649f, 0, 0,
        +0.1874073f, -0.4997529f, 0, 0, /**/ +0.4394094f, -1.1717584f, 0, 0, /**/ +0.8175961f, -2.1802563f, 0, 0, /**/ +0.8175961f, -2.1802563f, 0, 0,
        +0.2442963f, -0.6514569f, 0, 0, /**/ +0.6605640f, -1.7615040f, 0, 0, /**/ +1.7508409f, -4.6689090f, 0, 0, /**/ +1.7508409f, -4.6689090f, 0, 0};
    performTest(testImageSize, center, velocityChange, radius, inputData, expectedOutputData, &verifiedImageSize);
}
