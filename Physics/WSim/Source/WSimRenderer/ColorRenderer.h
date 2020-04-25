#include "Source/WSimRenderer/Renderer.h"
#include "Source/WSimSimulation/Simulation/AbstractSimulation.h"

#include <memory>

class ColorRenderer : public Renderer {
public:
    ColorRenderer(AbstractSimulation &simulation);

protected:
    void processInput(int button, int action, int mods) override;
    void processMouseMove(double x, double y) override;
    void update(float dt) override;
    void render() override;

private:
    float transformCoordsFromAbsoluteSpaceToSimulationSpaceX(double x);
    float transformCoordsFromAbsoluteSpaceToSimulationSpaceY(double y);

    // General data
    AbstractSimulation &simulation;

    // Texture data
    AbstractSimulation::ImageInfo texture1Info;
    std::unique_ptr<char[]> texture1Data;
    GLuint texture1;

    // Mouse data
    bool clicked = false;
    float lastMouseX;
    float lastMouseY;
};
