#pragma once

#include "Source/WSimMPI/Master/MasterRendererInterface.hpp"
#include "Source/WSimRenderer/LevelSetRenderer3D.hpp"
#include "Source/WSimSimulation/Simulation/Simulation.hpp"

class Renderer;

class MasterRendererInterfaceLevelSet3D : public MasterRendererInterface {
public:
    MasterRendererInterfaceLevelSet3D(Master &master);

    // For MPI
    virtual void mainLoop() override;

    // For Renderer
    class LevelSetRendererCallbacksImpl : public LevelSetRendererCallbacks {
    public:
        LevelSetRendererCallbacksImpl(Master &master);
        void updateSimulation(float deltaTimeSeconds) override;
        float *getData() override;

    private:
        Master &master;
    };

private:
    std::unique_ptr<Renderer> createRenderer(Master &master);

    LevelSetRendererCallbacksImpl rendererCallbacks;
    std::unique_ptr<Renderer> renderer;
};
