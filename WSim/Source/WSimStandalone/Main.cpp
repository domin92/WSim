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
    ColorRenderer renderer{simulation};
    FpsCallback fpsCallback;
    if (useFpsCounter) {
        renderer.setFpsCallback(fpsCallback);
    }

    // Main loop
    renderer.mainLoop();
}
