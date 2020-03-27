#include "OpenGL.h"

#include "Simulation/Simulation.h"

#include <chrono>
#include <thread>

namespace OGL::detail {
static PrivateRenderData privateRenderData{};

void idle() {
    const auto frameTime = PrivateRenderData::clock::now();
    const auto deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(frameTime - privateRenderData.lastFrameTime);
    if (std::chrono::milliseconds(16) > deltaTime) {
        auto lackingDeltaTime = std::chrono::milliseconds(16) - deltaTime;
        std::this_thread::sleep_for(lackingDeltaTime);
    }
    privateRenderData.onUpdate(deltaTime.count() / 1000000.f);
    privateRenderData.lastFrameTime = frameTime;
}

void display() {
    // Start rendering
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);

    // Render color texture
    glBindTexture(GL_TEXTURE_2D, renderData.textures[0]);
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);
    glVertex2i(100, 100);
    glTexCoord2i(0, 1);
    glVertex2i(100, 500);
    glTexCoord2i(1, 1);
    glVertex2i(500, 500);
    glTexCoord2i(1, 0);
    glVertex2i(500, 100);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);

    // Render velocity texture
    glBindTexture(GL_TEXTURE_2D, renderData.textures[1]);
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);
    glVertex2i(600, 100);
    glTexCoord2i(0, 1);
    glVertex2i(600, 500);
    glTexCoord2i(1, 1);
    glVertex2i(1000, 500);
    glTexCoord2i(1, 0);
    glVertex2i(1000, 100);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);

    // End Rendering
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFlush();
    glutPostRedisplay();
}

} // namespace OGL::detail

namespace OGL {
RenderData renderData{};

void init(int windowWidth, int windowHeight) {
    // General setup
    int argc = 1;
    char **argv = nullptr;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("WSim");
    glMatrixMode(GL_PROJECTION);
    glOrtho(0, windowWidth, 0, windowHeight, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

float transformCoordsFromAbsoluteSpaceToSimulationSpaceX(int x) {
    // To texture space
    x -= 100;

    // To simulation space
    const auto simulationWidth = static_cast<float>(OGL::renderData.simulation->getSimulationSize().x);
    const auto textureWidth = 400.f;
    return x * (simulationWidth / textureWidth);
}

float transformCoordsFromAbsoluteSpaceToSimulationSpaceY(int y) {
    // To texture space
    y -= 100;
    y = 400 - y;

    // To simulation space
    const auto simulationHeight = static_cast<float>(OGL::renderData.simulation->getSimulationSize().y);
    const auto textureHeight = 400.f;
    return y * (simulationHeight / textureHeight);
}

void mainLoop(OnUpdateFunction onUpdate, OnMouseMoveFunction onMouseMove, OnMouseClickFunction onMouseClick) {
    // Initialize render data
    detail::privateRenderData.onUpdate = onUpdate;
    detail::privateRenderData.lastFrameTime = detail::PrivateRenderData::clock::now();

    // Setup callbacks
    glutMotionFunc(onMouseMove);
    glutMouseFunc(onMouseClick);
    glutIdleFunc(detail::idle);
    glutDisplayFunc(detail::display);

    // Main loop
    glutMainLoop();
}

void drawFrame(float *pixels, size_t width, size_t height) {
    for (int x = 0; x < width; x++) {
        auto y1 = 0;
        auto y2 = height - 1;
        auto a1 = pixels + 4 * (y1 * height + x);
        auto a2 = pixels + 4 * (y2 * height + x);
        std::fill_n(a1, 4, 1.f);
        std::fill_n(a2, 4, 1.f);
    }
    for (int y = 0; y < width; y++) {
        auto x1 = 0;
        auto x2 = width - 1;
        auto a1 = pixels + 4 * (y * height + x1);
        auto a2 = pixels + 4 * (y * height + x2);
        std::fill_n(a1, 4, 1.f);
        std::fill_n(a2, 4, 1.f);
    }
}

} // namespace OGL
