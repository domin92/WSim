#include "Utils/OpenCL.h"
#include "Utils/OpenGL.h"
#include "Simulation.h"

void update(float deltaTime) {
    auto &simulation = *OGL::renderData.simulation;
    auto imageSize = OGL::renderData.imageSize;

    simulation.stepSimulation(deltaTime);

    // Upload velocity
    OCL::enqueueReadImage3D(OGL::renderData.simulation->getCommandQueue(),
                            simulation.getVelocity().getSource(),
                            CL_TRUE, imageSize, 0, 0,
                            OGL::renderData.velocityPixels.get());
    glBindTexture(GL_TEXTURE_2D, OGL::renderData.velocityTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageSize.x, imageSize.y, 0, GL_RGBA, GL_FLOAT, OGL::renderData.velocityPixels.get());
    glBindTexture(GL_TEXTURE_2D, 0);
    ASSERT_GL_NO_ERROR();

    // Upload color
    OCL::enqueueReadImage3D(simulation.getCommandQueue(),
                            simulation.getColor().getSource(),
                            CL_TRUE, imageSize, 0, 0,
                            OGL::renderData.colorPixels.get());
    glBindTexture(GL_TEXTURE_2D, OGL::renderData.colorTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageSize.x, imageSize.y, 0, GL_RGBA, GL_FLOAT, OGL::renderData.colorPixels.get());
    glBindTexture(GL_TEXTURE_2D, 0);
    ASSERT_GL_NO_ERROR();
}

#include "Tests/GTest.h"

int main() {
    testing::runTests();

    // Create simulation
    OCL::Vec3 imageSize{100, 100, 1};
    Simulation simulation{imageSize};

    // Initialize rendering
    OGL::init(1100, 600);
    OGL::renderData.simulation = &simulation;
    OGL::renderData.colorPixels = std::make_unique<float[]>(imageSize.getRequiredBufferSize(4 * sizeof(float)));
    OGL::renderData.velocityPixels = std::make_unique<float[]>(imageSize.getRequiredBufferSize(4 * sizeof(float)));
    OGL::renderData.imageSize = imageSize;

    // Rendering loop
    OGL::mainLoop(update);
}
