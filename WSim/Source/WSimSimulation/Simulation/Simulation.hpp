#pragma once

#include "Source/WSimCommon/EdgeHelpers.hpp"
#include "Source/WSimSimulation/Simulation/BorderMaths.hpp"
#include "Source/WSimSimulation/Simulation/Step/SimulationStep.hpp"
#include "Source/WSimSimulation/Utils/ImagePair.hpp"
#include "Source/WSimSimulation/Utils/KernelManager.hpp"
#include "Source/WSimSimulation/Utils/OpenCL.hpp"

#include <chrono>
#include <memory>
#include <vector>

constexpr static cl_image_format vectorFieldFormat = {CL_RGBA, CL_FLOAT};
constexpr static cl_image_format scalarFieldFormat = {CL_R, CL_FLOAT};
constexpr static cl_image_format velocityFieldFormat = vectorFieldFormat;
constexpr static cl_image_format colorFieldFormat = scalarFieldFormat;

class Simulation {
public:
    constexpr static size_t colorVoxelSize = OCL::getSizeOfTexel(colorFieldFormat);
    constexpr static size_t velocityVoxelSize = OCL::getSizeOfTexel(velocityFieldFormat);

    Simulation(size_t platformIndex, size_t deviceIndex, Vec3 simulationSize);
    Simulation(size_t platformIndex, size_t deviceIndex, Vec3 simulationSize, size_t borderWidth, PositionInGrid positionInGrid);

    void stepSimulation(float deltaTimeSeconds);
    template <typename Rep, typename Period>
    void stepSimulation(std::chrono::duration<Rep, Period> deltaTime) {
        stepSimulation(std::chrono::duration_cast<std::chrono::duration<float>>(deltaTime).count());
    }

    void writeColor(const float* arg);
    void readColor(float* arg);

    void setGravityForce(float force) { this->gravityForce = force; }

    void applyForce(float positionX, float positionY, float changeX, float changeY, float radius);
    void applyForce(FloatVec3 position, FloatVec3 change, float radius);
    void stop();
    void reset();
    void addObstacleWall(Dim dimension, End end);
    void addObstacleAllWalls();

    auto getBorderOffset() const { return borderOffset; }
    auto getGravityForce() const { return gravityForce; }
    auto getSimulationSize() const { return simulationSize; }
    auto getSimulationSizeWithBorder() const { return simulationSizeWithBorder; }
    auto &getCommandQueue() { return commandQueue; }
    auto &getContext() { return context; }
    auto &getDevice() { return device; }
    auto &getPositionInGrid() { return positionInGrid; }
    auto &getColor() { return color; }
    auto &getVelocity() { return velocity; }
    auto &getObstacles() { return obstacles; }
    auto &getKernelManager() { return kernels; }

protected:
    // Sizes
    const PositionInGrid positionInGrid;
    const Vec3 simulationSize;           // size for which simulation kernels are launched
    const Vec3 simulationSizeWithBorder; // simulationSize increased by additional border space
    const Vec3 borderOffset;             // offset to be applied to extended images
    float gravityForce = 0.f;

    // Basic OCL objects
    cl_platform_id platform;
    OCL::Device device;
    OCL::Context context;
    OCL::CommandQueue commandQueue;

    // Simulation steps
    std::vector<std::unique_ptr<SimulationStep>> simulationSteps;

    // Images
    Image3DPair velocity; // extended
    Image3DPair color;    // extended
    OCL::Mem obstacles;

    // Kernels
    KernelManager kernels;
    OCL::Kernel kernelInitializeColor;
    OCL::Kernel kernelAddVelocity;
};
