#include "Source/WSimCore/Simulation/Simulation.h"
#include "Source/WSimCore/Utils/OpenCL.h"
#include "Source/WSimCore/Utils/OpenGL.h"

void update(float deltaTime) {
    // Play simulation
    auto &simulation = *OGL::renderData.simulation;
    simulation.stepSimulation(deltaTime);

    // Upload color in simulation + border
    OCL::Vec3 offset{};
    OCL::Vec3 imageSize = simulation.getSimulationSizeWithBorder();
    OCL::enqueueReadImage3D(OGL::renderData.simulation->getCommandQueue(),
                            simulation.getColor().getSource(),
                            CL_TRUE, offset, imageSize,
                            OGL::renderData.velocityPixels.get());
    glBindTexture(GL_TEXTURE_2D, OGL::renderData.textures[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(imageSize.x), static_cast<GLsizei>(imageSize.y), 0, GL_RGBA, GL_FLOAT, OGL::renderData.velocityPixels.get());
    glBindTexture(GL_TEXTURE_2D, 0);
    ASSERT_GL_NO_ERROR();

    // Upload color in simulation
    offset = simulation.getBorderOffset();
    imageSize = simulation.getSimulationSize();
    OCL::enqueueReadImage3D(simulation.getCommandQueue(),
                            simulation.getColor().getSource(),
                            CL_TRUE, offset, imageSize,
                            OGL::renderData.colorPixels.get());
    glBindTexture(GL_TEXTURE_2D, OGL::renderData.textures[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(imageSize.x), static_cast<GLsizei>(imageSize.y), 0, GL_RGBA, GL_FLOAT, OGL::renderData.colorPixels.get());
    glBindTexture(GL_TEXTURE_2D, 0);
    ASSERT_GL_NO_ERROR();
}

void mouseClick(int button, int state, int x, int y) {
    switch (button) {
    case GLUT_LEFT_BUTTON:
        OGL::renderData.clicked = (state == GLUT_DOWN);
        if (OGL::renderData.clicked) {
            OGL::renderData.lastMouseX = OGL::transformCoordsFromAbsoluteSpaceToSimulationSpaceX(x);
            OGL::renderData.lastMouseY = OGL::transformCoordsFromAbsoluteSpaceToSimulationSpaceY(y);
        }
        break;

    case GLUT_RIGHT_BUTTON:
        OGL::renderData.simulation->stop();
        break;
    }
}

void mouseMove(int mouseX, int mouseY) {
    auto &simulation = *OGL::renderData.simulation;

    // Calculate position in simulation space and abort if it's out of bounds
    const float x = OGL::transformCoordsFromAbsoluteSpaceToSimulationSpaceX(mouseX);
    const float y = OGL::transformCoordsFromAbsoluteSpaceToSimulationSpaceY(mouseY);
    if (!OGL::renderData.clicked || x < 0 || y < 0 || x > simulation.getSimulationSize().x || y > simulation.getSimulationSize().y) {
        return;
    }

    // Calculate the mouse movement and abort if mouse did not move
    const float deltaX = OGL::renderData.lastMouseX - x;
    const float deltaY = OGL::renderData.lastMouseY - y;
    OGL::renderData.lastMouseX = x;
    OGL::renderData.lastMouseY = y;
    if (deltaX == 0 && deltaY == 0) {
        return;
    }

    // Apply force at given point
    const float radius = static_cast<float>(simulation.getSimulationSize().x) / 10.f;
    OGL::renderData.simulation->applyForce(x, y, deltaX, deltaY, radius);
}

int main(int argc, char **argv) {
    // Parse arguments
    size_t clPlatformIndex = 0u;
    size_t clDeviceIndex = 0u;
    int argIndex = 1;
    while (argIndex < argc) {
        const bool hasNextArg = (argIndex + 1 < argc);
        const std::string currArg = argv[argIndex];
        const std::string nextArg = hasNextArg ? argv[argIndex + 1] : "";

        if (hasNextArg && (currArg == "-p" || currArg == "--platform")) {
            clPlatformIndex = std::atoi(nextArg.c_str());
            argIndex += 2;
            continue;
        }
        if (hasNextArg && (currArg == "-d" || currArg == "--device")) {
            clDeviceIndex = std::atoi(nextArg.c_str());
            argIndex += 2;
            continue;
        }
        argIndex++;
    }
    std::cout << "Using clPlatformIndex=" << clPlatformIndex << '\n';
    std::cout << "Using clDeviceIndex=" << clDeviceIndex << '\n';

    // Mocked grid parameters
    const OCL::Vec3 gridId{0,0, 0};
    const OCL::Vec3 gridSize{1,1, 1};

    // Create simulation
    const OCL::Vec3 imageSize{100, 100, 1};
    const size_t borderWidth = 5;
    const PositionInGrid positionInGrid{gridId, gridSize};
    Simulation simulation{clPlatformIndex, clDeviceIndex, imageSize, borderWidth, positionInGrid};

    // Initialize rendering
    OGL::init(1100, 600);
    OGL::renderData.simulation = &simulation;
    OGL::renderData.colorPixels = std::make_unique<float[]>(imageSize.getRequiredBufferSize(4 * sizeof(float)));
    OGL::renderData.velocityPixels = std::make_unique<float[]>(imageSize.getRequiredBufferSize(4 * sizeof(float)));
    glGenTextures(2, OGL::renderData.textures);

    // Rendering loop
    OGL::mainLoop(update, mouseMove, mouseClick);
}
