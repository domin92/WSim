#pragma once

#include "Source/WSimRenderer/Renderer.hpp"

struct LevelSetRendererCallbacks {
    virtual void updateSimulation(float deltaTimeSeconds) = 0;
    virtual float *getData() = 0;
};

class LevelSetRenderer : public Renderer {
public:
    LevelSetRenderer(LevelSetRendererCallbacks &callbacks, int screenWidth, int screenHeight);

protected:
    void processKeyboardInput(int key, int scancode, int action, int mods) override;
    void update(float deltaTimeSeconds) override;
    void render() override;

private:
    LevelSetRendererCallbacks &callbacks;
};
