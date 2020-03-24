#define FREEGLUT_LIB_PRAGMAS 0
#include "Simulation.h"

#include <GL/freeglut.h>
#include <cassert>
#include <chrono>
#include <memory>

#define ASSERT_GL_NO_ERROR()        \
    {                               \
        auto error = glGetError();  \
        if (error != GL_NO_ERROR) { \
            assert(false);          \
        }                           \
    }

namespace OGL {
using OnUpdateFunction = void (*)(float);
using OnMouseMoveFunction = void (*)(int, int);
using OnMouseClickFunction = void (*)(int, int, int, int);
struct RenderData {
    // Used by application
    std::unique_ptr<float[]> colorPixels;
    std::unique_ptr<float[]> velocityPixels;
    GLuint colorTexture;
    GLuint velocityTexture;
    Simulation *simulation;
    OCL::Vec3 imageSize;
    float lastMouseX; // simulation space
    float lastMouseY; // simulation space
    bool clicked = false;
};
extern RenderData renderData;

void init(int windowWidth, int windowHeight);
float transformCoordsFromAbsoluteSpaceToSimulationSpaceX(int x);
float transformCoordsFromAbsoluteSpaceToSimulationSpaceY(int y);
void mainLoop(OnUpdateFunction onUpdate, OnMouseMoveFunction onMouseMove, OnMouseClickFunction onMouseClick);
} // namespace OGL

namespace OGL::detail {
void idle();
void display();

struct PrivateRenderData {
    using clock = std::chrono::steady_clock;
    clock::time_point lastFrameTime;
    OnUpdateFunction onUpdate;
};
} // namespace OGL::detail
