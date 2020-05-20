#include "Source/WSimCommon/ArgumentParser.h"
#include "Source/WSimRenderer/FpsCounter.h"
#include "Source/WSimSimulation/Simulation/Simulation.h"
#include "Source/WSimStandalone/ColorRendererCallbacks.h"

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

int main(int argc, char **argv) {
    // Parse arguments
    ArgumentParser argumentParser{argc, argv};
    const auto clPlatformIndex = argumentParser.getArgumentValue<size_t>({"-p", "--platform"}, 0u);
    const auto clDeviceIndex = argumentParser.getArgumentValue<size_t>({"-d", "--device"}, 0u);
    const auto simulationSize = argumentParser.getArgumentValue<size_t>({"-s", "--size"}, 200);
    const auto modeString = argumentParser.getArgumentValue<std::string>({"-m", "--mode"}, "graphical");

    // Get mode
    enum class Mode { Graphical,
                      Text };
    Mode mode;
    if (modeString == "graphical") {
        mode = Mode::Graphical;
    } else if (modeString == "text") {
        mode = Mode::Text;
    } else {
        std::cout << "Unknown mode: " << modeString << '\n';
        return 1;
    }

    // Print arguments
    std::cout << "Used parameters:\n";
    std::cout << "\tclPlatformIndex=" << clPlatformIndex << '\n';
    std::cout << "\tclDeviceIndex=" << clDeviceIndex << '\n';
    std::cout << "\tsimulationSize=" << simulationSize << '\n';
    std::cout << "\tmode=" << ((mode == Mode::Graphical) ? "graphical" : "text") << '\n';

    // Create simulation
    const Vec3 imageSize{simulationSize, simulationSize, 1};
    Simulation simulation{clPlatformIndex, clDeviceIndex, imageSize};
    simulation.addObstacleAllWalls();

    FpsCallback fpsCallback;

    if (mode == Mode::Graphical) {
        ColorRendererCallbacksImpl rendererCallbacks{simulation};
        ColorRenderer renderer{rendererCallbacks};
        renderer.setFpsCallback(fpsCallback);
        renderer.mainLoop();
    } else {
        DefaultFpsCounter fpsCounter;
        using Clock = std::chrono::steady_clock;
        auto lastFrameTime = Clock::now();
        while (true) {
            const auto currentFrameTime = Clock::now();
            const auto deltaTime = currentFrameTime - lastFrameTime;
            fpsCounter.push(deltaTime);
            fpsCallback(fpsCounter.getFps());
            lastFrameTime = currentFrameTime;

            simulation.stepSimulation(deltaTime);
            OCL::finish(simulation.getCommandQueue());
        }
    }
}
