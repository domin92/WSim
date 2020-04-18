#define FREEGLUT_LIB_PRAGMAS 0

#include <GL/freeglut.h>
#include <cassert>
#include <chrono>
#include <memory>

class Simulation;

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
    GLuint textures[2];
    Simulation *simulation;
    float lastMouseX; // simulation space
    float lastMouseY; // simulation space
    bool clicked = false;
};
extern RenderData renderData;

void init(int windowWidth, int windowHeight);
float transformCoordsFromAbsoluteSpaceToSimulationSpaceX(int x);
float transformCoordsFromAbsoluteSpaceToSimulationSpaceY(int y);
void mainLoop(OnUpdateFunction onUpdate, OnMouseMoveFunction onMouseMove, OnMouseClickFunction onMouseClick);
void drawFrame(float* pixels, size_t width, size_t height);
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