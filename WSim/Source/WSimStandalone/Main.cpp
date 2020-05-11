#include "Source/WSimRenderer/ColorRenderer.h"
#include "Source/WSimRenderer/FpsCounter.h"
#include "Source/WSimSimulation/Simulation/Simulation.h"

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
        Vec3 offset = simulation.getBorderOffset();
        offset.z = 0;
        Vec3 size = simulation.getSimulationSize();
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

    void stepSimulation(float deltaTimeSeconds) override {
        simulation.stepSimulation(deltaTimeSeconds);
    }

private:
    Simulation &simulation;
};

int main(int argc, char **argv) {
    enum class Mode {
        Graphical,
        Text,
    };

    // Parse arguments
    size_t clPlatformIndex = 0u;
    size_t clDeviceIndex = 0u;
    size_t simulationSize = 200u;
    Mode mode = Mode::Graphical;
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
        if (hasNextArg && (currArg == "-s" || currArg == "--size")) {
            simulationSize = std::atoi(nextArg.c_str());
            if (simulationSize == 0) {
                std::cout << "Incorrect simulation size\n";
                return 1;
            }
            argIndex += 2;
            continue;
        }
        if (hasNextArg && (currArg == "-m" || currArg == "--mode")) {
            if (nextArg == "graphical") {
                mode = Mode::Graphical;
            } else if (nextArg == "text") {
                mode = Mode::Text;
            } else {
                std::cout << "Unknown mode: " << nextArg << '\n';
                return 1;
            }
            argIndex += 2;
            continue;
        }
        argIndex++;
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
