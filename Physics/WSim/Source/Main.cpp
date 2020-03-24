#include "Simulation.h"

#include "Utils/OpenCL.h"
#include "Utils/OpenGL.h"

void update(float deltaTime) {
    auto &simulation = *OGL::renderData.simulation;
    auto imageSize = OGL::renderData.imageSize;

    simulation.stepSimulation(deltaTime);

    // Upload velocity
    OCL::enqueueReadImage3D(OGL::renderData.simulation->getCommandQueue(),
                            simulation.getVelocity().getSource(),
                            CL_TRUE, imageSize,
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
                            CL_TRUE, imageSize,
                            OGL::renderData.colorPixels.get());
    glBindTexture(GL_TEXTURE_2D, OGL::renderData.colorTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageSize.x, imageSize.y, 0, GL_RGBA, GL_FLOAT, OGL::renderData.colorPixels.get());
    glBindTexture(GL_TEXTURE_2D, 0);
    ASSERT_GL_NO_ERROR();
}

void transformCoordsFromAbsoluteSpaceToRenderSpace(int &x, int &y) {
    // Translate
    x -= 100;
    y -= 100;

    // Invert y
    y = 400 - y;
}

void transformCoordsFromRenderSpaceToSimulationSpace(float &x, float &y) {
    x *= (OGL::renderData.imageSize.x / 400.f);
    y *= (OGL::renderData.imageSize.y / 400.f);
}

void mouseClick(int button, int state, int x, int y) {
    switch (button) {
    case GLUT_LEFT_BUTTON:
        OGL::renderData.clicked = (state == GLUT_DOWN);
        if (OGL::renderData.clicked) {
            transformCoordsFromAbsoluteSpaceToRenderSpace(x, y);
            OGL::renderData.lastMouseX = x;
            OGL::renderData.lastMouseY = y;
        }
        break;

    case GLUT_RIGHT_BUTTON:
        OGL::renderData.simulation->stop();
        break;
    }
}

void mouseMove(int x, int y) {
    transformCoordsFromAbsoluteSpaceToRenderSpace(x, y);
    if (!OGL::renderData.clicked || x < 0 || y < 0 || x > 400 || y > 400) {
        return;
    }

    float deltaX = OGL::renderData.lastMouseX - x;
    float deltaY = OGL::renderData.lastMouseY - y;
    OGL::renderData.lastMouseX = x;
    OGL::renderData.lastMouseY = y;
    if (deltaX == 0 && deltaY == 0) {
        return;
    }
    float centerX = x;
    float centerY = y;

    transformCoordsFromRenderSpaceToSimulationSpace(centerX, centerY);
    transformCoordsFromRenderSpaceToSimulationSpace(deltaX, deltaY);
    OGL::renderData.simulation->applyForce(centerX, centerY, deltaX, deltaY, 20);
}

int main() {
    // Create simulation
    OCL::Vec3 imageSize{200, 200, 1};
    Simulation simulation{imageSize};

    // Initialize rendering
    OGL::init(1100, 600);
    OGL::renderData.simulation = &simulation;
    OGL::renderData.colorPixels = std::make_unique<float[]>(imageSize.getRequiredBufferSize(4 * sizeof(float)));
    OGL::renderData.velocityPixels = std::make_unique<float[]>(imageSize.getRequiredBufferSize(4 * sizeof(float)));
    OGL::renderData.imageSize = imageSize;
    glGenTextures(1, &OGL::renderData.colorTexture);
    glGenTextures(1, &OGL::renderData.velocityTexture);

    // Rendering loop
    OGL::mainLoop(update, mouseMove, mouseClick);
}
