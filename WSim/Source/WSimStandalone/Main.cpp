#include "Source/WSimRenderer/ColorRenderer.h"
#include "Source/WSimSimulation/Simulation/Simulation.h"

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

    // Create simulation
    const OCL::Vec3 imageSize{100, 100, 1};
    const size_t borderWidth = 13;
    const OCL::Vec3 gridId{ 0, 0, 0 };
    const OCL::Vec3 gridSize{ 1, 1, 1 };
    const PositionInGrid positionInGrid{gridId, gridSize};
    Simulation simulation{clPlatformIndex, clDeviceIndex, imageSize, borderWidth, positionInGrid};

    // Create renderer
    ColorRenderer renderer{simulation};

    // Main loop
    renderer.mainLoop();
}
