#include "Source/WSimRenderer/ColorRenderer.h"
#include "Source/WSimSimulation/Simulation/Simulation.h"

struct FpsCallback {
    using Clock = std::chrono::steady_clock;
    Clock::time_point lastPrintTime = {};

    void operator()(unsigned int deltaTime) {
        const Clock::time_point now = Clock::now();
        if (now - lastPrintTime > std::chrono::milliseconds(700)) {
            std::cout << "FPS: " << deltaTime << '\n';
            lastPrintTime = now;
        }
    }
};

class ColorRendererCallbacksImpl : public ColorRendererCallbacks {
public:
    ColorRendererCallbacksImpl(Simulation &simulation) : simulation(simulation) {}

    size_t getSubImagesCount2D() override {
        return 1u;
    }

    SubImageInfo getSubImageInfo2D(size_t subImageIndex) override {
        SubImageInfo info;
        info.xOffset = 0;
        info.yOffset = 0;
        info.width = simulation.getSimulationSize().x;
        info.height = simulation.getSimulationSize().y;
        info.valid = true;
        return info;
    }

    void getSubImage2D(size_t subImageIndex, void *data) override {
        wsimErrorIf(subImageIndex != 0);
        OCL::Vec3 offset = simulation.getBorderOffset();
        offset.z = 0;
        OCL::Vec3 size = simulation.getSimulationSize();
        size.z = 1;
        OCL::enqueueReadImage3D(simulation.getCommandQueue(), simulation.getColor().getSource(), CL_BLOCKING, offset, size, data);
    }

    void applyForce(float positionX, float positionY, float changeX, float changeY, float radius) override {
        simulation.applyForce(positionX, positionY, changeX, changeY, radius);
    }

    void stop() override {
        simulation.stop();
    }

    void reset() override {
        simulation.reset();
    }

    void stepSimulation(float deltaTime) override {
        simulation.stepSimulation(deltaTime);
    }

private:
    Simulation &simulation;
};

int main(int argc, char **argv) {
    // Parse arguments
    size_t clPlatformIndex = 0u;
    size_t clDeviceIndex = 0u;
    int argIndex = 1;
    bool useFpsCounter = true;
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
        if (currArg == "-f" || currArg == "--noFpsCounter") {
            useFpsCounter = false;
        }
        argIndex++;
    }
    std::cout << "Using clPlatformIndex=" << clPlatformIndex << '\n';
    std::cout << "Using clDeviceIndex=" << clDeviceIndex << '\n';

    // Create simulation
    const OCL::Vec3 imageSize{200, 200, 1};
    const size_t borderWidth = 13;
    const OCL::Vec3 gridId{0, 0, 0};
    const OCL::Vec3 gridSize{1, 1, 1};
    const PositionInGrid positionInGrid{gridId, gridSize};
    Simulation simulation{clPlatformIndex, clDeviceIndex, imageSize, borderWidth, positionInGrid};
    simulation.addObstacleAllWalls();

    // Create renderer
    ColorRendererCallbacksImpl rendererCallbacks{simulation};
    ColorRenderer renderer{rendererCallbacks};
    FpsCallback fpsCallback;
    if (useFpsCounter) {
        renderer.setFpsCallback(fpsCallback);
    }

    // Main loop
    renderer.mainLoop();
}
