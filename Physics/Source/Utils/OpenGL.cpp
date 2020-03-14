#include "OpenGL.h"

#include <chrono>

namespace OGL::detail {
void display() {
    // Start rendering
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);

    // Render color texture
    glBindTexture(GL_TEXTURE_2D, renderData.colorTexture);
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
    glBindTexture(GL_TEXTURE_2D, renderData.velocityTexture);
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
    glFlush(); //don't need this with GLUT_DOUBLE and glutSwapBuffers
    glutSwapBuffers();
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
    glutCreateWindow("Hello world :D");
    glMatrixMode(GL_PROJECTION);
    glOrtho(0, windowWidth, 0, windowHeight, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    // Create resources
    glGenTextures(1, &renderData.colorTexture);
    glGenTextures(1, &renderData.velocityTexture);
    ASSERT_GL_NO_ERROR();
}

void mainLoop(OnUpdateFunction onUpdate) {
    const auto minFrameTime = std::chrono::milliseconds(16);
    std::chrono::steady_clock clock{};
    decltype(clock)::time_point lastFrameTime = clock.now();
    while (1) {
        auto thisFrameTime = clock.now();
        auto deltaTime = thisFrameTime - lastFrameTime;
        if (minFrameTime > deltaTime) {
            auto lackingDeltaTime = minFrameTime - deltaTime;
            Sleep(std::chrono::duration_cast<std::chrono::milliseconds>(lackingDeltaTime).count());
        }
        lastFrameTime = thisFrameTime;

        const float deltaTimeFloat = std::chrono::duration_cast<std::chrono::microseconds>(deltaTime).count() / 1000000.f;
        onUpdate(deltaTimeFloat);
        detail::display();
    }
}

} // namespace OGL
