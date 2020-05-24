#if WSIM_TEXT_ONLY == 0
#include "Source/WSimRenderer/FpsCounter.h"
#include "Source/WSimStandalone/ColorRendererCallbacks.h"
#include "Source/WSimStandalone/VoxelRendererCallbacks.h"
#endif

#include "Source/WSimCommon/ArgumentParser.h"
#include "Source/WSimSimulation/Simulation/Simulation.h"

#include <iostream>

struct FpsCallback {
    using Clock = std::chrono::steady_clock;
    Clock::time_point lastPrintTime = {};

    void operator()(unsigned int fps) {
        const Clock::time_point now = Clock::now();
        if (now - lastPrintTime > std::chrono::milliseconds(700)) {
            std::cout << "FPS: " << fps << '\n';
            lastPrintTime = now;
        }
    }
};

struct Mode {
    enum class ModeEnum {
        Graphical2D,
        Graphical3D,
        Text,
    } value;
    Mode(ModeEnum value) : value(value) {}

    static std::unique_ptr<Mode> fromString(const std::string &modeString) {
        if (modeString == "text") {
            return std::make_unique<Mode>(ModeEnum::Text);
        }
#if WSIM_TEXT_ONLY == 0
        else if (modeString == "graphical2d") {
            return std::make_unique<Mode>(ModeEnum::Graphical2D);
        } else if (modeString == "graphical3d") {
            return std::make_unique<Mode>(ModeEnum::Graphical3D);
        }
#endif()
        else {
            return nullptr;
        }
    }

    bool is2D() {
        switch (value) {
        case ModeEnum::Graphical2D:
        case ModeEnum::Text:
            return true;
        default:
            return false;
        }
    }

    std::string toString() const {
        switch (value) {
        case ModeEnum::Graphical2D:
            return "Graphical2D";
        case ModeEnum::Graphical3D:
            return "Graphical3D";
        case ModeEnum::Text:
            return "Text";
        default:
            wsimError();
        }
    }
};

int main(int argc, char **argv) {
    // Parse arguments
    ArgumentParser argumentParser{argc, argv};
    const auto clPlatformIndex = argumentParser.getArgumentValue<size_t>({"-p", "--platform"}, 0u);
    const auto clDeviceIndex = argumentParser.getArgumentValue<size_t>({"-d", "--device"}, 0u);
    const auto simulationSize = argumentParser.getArgumentValue<size_t>({"-s", "--size"}, 200);
    const auto modeString = argumentParser.getArgumentValue<std::string>({"-m", "--mode"}, "graphical2d");

    // Get mode
    const auto mode = Mode::fromString(modeString);
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
    Simulation simulation{clPlatformIndex, clDeviceIndex, imageSize};
    simulation.addObstacleAllWalls();
    simulation.setGravityForce(1.f);

    FpsCallback fpsCallback;

    switch (mode->value) {
#if WSIM_TEXT_ONLY == 0
    case Mode::ModeEnum::Graphical2D: {
        ColorRendererCallbacksImpl rendererCallbacks{simulation};
        ColorRenderer renderer{rendererCallbacks, Simulation::colorVoxelSize};
        renderer.setFpsCallback(fpsCallback);
        renderer.mainLoop();
    }

    case Mode::ModeEnum::Graphical3D: {
        VoxelRendererCallbacksImpl rendererCallbacks{simulation};
        VoxelRenderer renderer{rendererCallbacks, static_cast<int>(simulation.getSimulationSize().x), 1, 600};
        renderer.setFpsCallback(fpsCallback);
        renderer.mainLoop();
    }
#endif

    case Mode::ModeEnum::Text: {
        //DefaultFpsCounter fpsCounter;
        using Clock = std::chrono::steady_clock;
        auto lastFrameTime = Clock::now();
        while (true) {
            const auto currentFrameTime = Clock::now();
            const auto deltaTime = currentFrameTime - lastFrameTime;
            //fpsCounter.push(deltaTime);
            //fpsCallback(fpsCounter.getFps());
            lastFrameTime = currentFrameTime;

            simulation.stepSimulation(deltaTime);
            OCL::finish(simulation.getCommandQueue());
        }
    }
    }
}
