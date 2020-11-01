#include "Source/WSimCommon/ArgumentParser.hpp"
#include "Source/WSimCommon/DefaultFpsCallback.hpp"
#include "Source/WSimCommon/LevelSetHelper.h"
#include "Source/WSimCommon/SimulationMode.h"
#include "Source/WSimRenderer/FpsCounter.hpp"
#include "Source/WSimSimulation/Simulation/Simulation.hpp"
#include "Source/WSimStandalone/ColorRendererCallbacks.hpp"
#include "Source/WSimStandalone/LevelSetRendererCallbacks.hpp"
#include "Source/WSimStandalone/TextRenderer.hpp"
#include "Source/WSimStandalone/VolumeRendererCallbacks.hpp"

int main(int argc, char **argv) {
    // Parse arguments
    ArgumentParser argumentParser{argc, argv};
    const auto clPlatformIndex = argumentParser.getArgumentValue<size_t>({"-p", "--platform"}, 0u);
    const auto clDeviceIndex = argumentParser.getArgumentValue<size_t>({"-d", "--device"}, 0u);
    const auto simulationSize = argumentParser.getArgumentValue<size_t>({"-s", "--size"}, 100);
    const auto modeString = argumentParser.getArgumentValue<std::string>({"-m", "--mode"}, "levelset3d");

    // Get mode
    const auto mode = SimulationMode::fromString(modeString);
    if (mode == nullptr) {
        std::cerr << "ERROR: Invalid mode\n";
        return 1;
    }

    // Print arguments
    std::cout << "Used parameters:\n";
    std::cout << "\tclPlatformIndex=" << clPlatformIndex << '\n';
    std::cout << "\tclDeviceIndex=" << clDeviceIndex << '\n';
    std::cout << "\tsimulationSize=" << simulationSize << '\n';
    std::cout << "\tmode=" << mode->toString() << '\n';

    // Create simulation
    Vec3 imageSize{simulationSize, simulationSize, simulationSize};
    if (mode->is2D()) {
        imageSize.z = 1;
    }
    Simulation simulation{clPlatformIndex, clDeviceIndex, imageSize, mode->isLevelSet()};
    simulation.addObstacleAllWalls();
    simulation.setGravityForce(0.1f);

    DefaultFpsCallback fpsCallback;

    switch (mode->value) {
    case SimulationMode::Enum::Graphical2D: {
        ColorRendererCallbacksImpl rendererCallbacks{simulation};
        ColorRenderer renderer{rendererCallbacks, Simulation::colorVoxelSize};
        renderer.setFpsCallback(fpsCallback);
        renderer.mainLoop();
        break;
    }

    case SimulationMode::Enum::Graphical3D: {
        VolumeRendererCallbacksImpl rendererCallbacks{simulation};
        VolumeRenderer renderer{rendererCallbacks, static_cast<int>(simulation.getSimulationSize().x), 1, 600};
        renderer.setFpsCallback(fpsCallback);
        renderer.mainLoop();
        break;
    }

    case SimulationMode::Enum::LevelSet3D: {
        const float sphereRadius = static_cast<float>(simulationSize - 5) / 2;
        auto levelSet = std::make_unique<float[]>(imageSize.getRequiredBufferSize(1));
        LevelSetHelper::initializeToSphere(levelSet.get(), imageSize, sphereRadius);

        LevelSetRendererCallbacksImpl callbacks{simulation, levelSet.get()};
        LevelSetRenderer renderer{callbacks, 600, 600, static_cast<int>(simulation.getSimulationSize().x), 1};
        renderer.setFpsCallback(fpsCallback);
        renderer.mainLoop();
        break;
    }

    case SimulationMode::Enum::Text: {
        TextRenderer renderer{simulation};
        renderer.setFpsCallback(fpsCallback);
        renderer.mainLoop();
        break;
    }
    }
}
