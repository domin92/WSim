#include "Source/WSimCommon/ArgumentParser.hpp"
#include "Source/WSimCommon/DefaultFpsCallback.hpp"
#include "Source/WSimRenderer/FpsCounter.hpp"
#include "Source/WSimSimulation/Simulation/Simulation.hpp"
#include "Source/WSimStandalone/ColorRendererCallbacks.hpp"
#include "Source/WSimStandalone/TextRenderer.hpp"
#include "Source/WSimStandalone/VolumeRendererCallbacks.hpp"

struct Mode {
    enum class ModeEnum {
        Graphical2D,
        Graphical3D,
        Text,
    } value;
    Mode(ModeEnum value) : value(value) {}

    static std::unique_ptr<Mode> fromString(const std::string &modeString) {
        if (modeString == "graphical2d") {
            return std::make_unique<Mode>(ModeEnum::Graphical2D);
        } else if (modeString == "graphical3d") {
            return std::make_unique<Mode>(ModeEnum::Graphical3D);
        } else if (modeString == "text") {
            return std::make_unique<Mode>(ModeEnum::Text);
        } else {
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

    DefaultFpsCallback fpsCallback;

    switch (mode->value) {
    case Mode::ModeEnum::Graphical2D: {
        ColorRendererCallbacksImpl rendererCallbacks{simulation};
        ColorRenderer renderer{rendererCallbacks, Simulation::colorVoxelSize};
        renderer.setFpsCallback(fpsCallback);
        renderer.mainLoop();
        break;
    }

    case Mode::ModeEnum::Graphical3D: {
        VolumeRendererCallbacksImpl rendererCallbacks{simulation};
        VolumeRenderer renderer{rendererCallbacks, static_cast<int>(simulation.getSimulationSize().x), 1, 600};
        renderer.setFpsCallback(fpsCallback);
        renderer.mainLoop();
        break;
    }

    case Mode::ModeEnum::Text: {
        TextRenderer renderer{simulation};
        renderer.setFpsCallback(fpsCallback);
        renderer.mainLoop();
        break;
    }
    }
}
