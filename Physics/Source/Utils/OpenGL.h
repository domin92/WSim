#define FREEGLUT_STATIC
#define FREEGLUT_LIB_PRAGMAS 0
#include <GL/freeglut.h>
#include "Simulation.h"
#include <memory>
#include <cassert>

#define ASSERT_GL_NO_ERROR()        \
    {                               \
        auto error = glGetError();  \
        if (error != GL_NO_ERROR) { \
            assert(false);          \
        }                           \
    }

namespace OGL::detail {
void display();
}

namespace OGL {
struct RenderData {
    std::unique_ptr<float[]> colorPixels;
    std::unique_ptr<float[]> velocityPixels;
    GLuint colorTexture;
    GLuint velocityTexture;
    Simulation* simulation;
    OCL::Vec3 imageSize;
};
extern RenderData renderData;
using OnUpdateFunction = void(*)(float);

void init(int windowWidth, int windowHeight);
void mainLoop(OnUpdateFunction onUpdate);
} // namespace OGL
