#pragma once

#include "Source/WSimCommon/EdgeHelpers.h"
#include "Source/WSimSimulation/Simulation/AbstractSimulation.h"
#include "Source/WSimSimulation/Simulation/BorderMaths.h"
#include "Source/WSimSimulation/Simulation/SimulationStep.h"
#include "Source/WSimSimulation/Utils/ImagePair.h"
#include "Source/WSimSimulation/Utils/KernelManager.h"
#include "Source/WSimSimulation/Utils/OpenCL.h"

#include <memory>
#include <vector>

const static cl_image_format vectorFieldFormat = {CL_RGBA, CL_FLOAT};
const static cl_image_format scalarFieldFormat = {CL_R, CL_FLOAT};
class Simulation : public AbstractSimulation {
public:
    Simulation(size_t platformIndex, size_t deviceIndex, OCL::Vec3 simulationSize, size_t borderWidth, PositionInGrid positionInGrid);
    void stepSimulation(float deltaTime) override;
    void applyForce(float positionX, float positionY, float changeX, float changeY, float radius) override;
    void stop() override;
    void reset() override;
    void addObstacleWall(Dim dimension, End end);
    void addObstacleAllWalls();

    auto getBorderOffset() const { return borderOffset; }
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
    const OCL::Vec3 simulationSize;           // size for which simulation kernels are launched
    const OCL::Vec3 simulationSizeWithBorder; // simulationSize increased by additional border space
    const OCL::Vec3 borderOffset;             // offset to be applied to extended images

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
    OCL::Kernel kernelFillVelocity;
    OCL::Kernel kernelFillColor;
    OCL::Kernel kernelAddVelocity;
};
