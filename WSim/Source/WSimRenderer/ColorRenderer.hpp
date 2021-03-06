#pragma once

#include "Source/WSimRenderer/Renderer.hpp"

#include <memory>
#include <vector>

struct ColorRendererCallbacks {
    struct SubImageInfo {
        size_t xOffset;
        size_t yOffset;
        size_t width;
        size_t height;
        bool valid;
    };
    virtual size_t getSubImagesCount2D() = 0;
    virtual SubImageInfo getSubImageInfo2D(size_t subImageIndex) = 0;
    virtual void getSubImage2D(size_t subImageIndex, void *data) = 0;
    virtual void applyForce(float positionX, float positionY, float changeX, float changeY, float radius) = 0;
    virtual void stop() = 0;
    virtual void reset() = 0;
    virtual void stepSimulation(float deltaTimeSeconds) = 0;
};

class ColorRenderer : public Renderer {
public:
    ColorRenderer(ColorRendererCallbacks &callbacks, size_t voxelSize);

protected:
    void processInput(int button, int action, int mods) override;
    void processMouseMove(double x, double y) override;
    void processScroll(double xoffset, double yoffset) override {}
    void processKeyboardInput(int key, int scancode, int action, int mods) override;
    void update(float deltaTimeSeconds) override;
    void render() override;

private:
    float transformCoordsFromAbsoluteSpaceToSimulationSpaceX(double x);
    float transformCoordsFromAbsoluteSpaceToSimulationSpaceY(double y);

    // General data
    ColorRendererCallbacks &callbacks;
    size_t imageWidth = 0;
    size_t imageHeight = 0;

    // Texture data
    std::vector<ColorRendererCallbacks::SubImageInfo> subImagesInfo;
    std::vector<std::unique_ptr<char[]>> subImagesData;
    GLuint texture1;

    // Mouse data
    bool clicked = false;
    float lastMouseX = 0;
    float lastMouseY = 0;
};
