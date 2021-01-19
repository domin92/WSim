#pragma once

#include "Source/WSimCommon/Vec3.hpp"
#include "Source/WSimRenderer/Renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

struct LevelSetRendererCallbacks {
    virtual void updateSimulation(float deltaTimeSeconds) = 0;
    virtual float *getData() = 0;
};

class LevelSetRenderer : public Renderer {
public:
    LevelSetRenderer(LevelSetRendererCallbacks &callbacks, int screenWidth, int screenHeight, int nodeSizeInVoxels, int gridSizeInNodes, bool benchmark);

    glm::mat4 createMvp();

    void loadBuffers();

    void loadShaders();

    void updateCameraFront();

protected:
    // Basic data
    LevelSetRendererCallbacks &callbacks;
    const int nodeSizeInVoxels;
    const int gridSizeInNodes;
    const int screenWidth;
    const int screenHeight;

    bool benchmark;
    double lastTime;

    // Buffers for cube
    GLuint VAO{};
    GLuint VBO{};
    GLuint EBO{};
    // Texture
    GLuint waterTexture{};

    // Shader
    OGL::ShaderProgram shaderProgram{};
    GLuint mvpVertexUniformLocation{};
    GLuint mvpFragmentUniformLocation{};
    GLuint nodeSizeUniformLocation{};
    GLuint gridSizeUniformLocation{};

    // Model-view-projection matrix
    bool mvpDirty = true;
    glm::mat4 mvp = {};
    glm::vec3 cameraPos = glm::vec3(2.0f, 2.0f, 2.0f);
    glm::vec3 cameraFront = {};
    const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    float fov = 45.0f;
    float yaw = -497.6f;
    float pitch = -35.1f;

    // Cached values for mouse movement
    bool firstMouse = true;
    float lastX = {};
    float lastY = {};

    void processInput(int button, int action, int mods) override;
    void processKeyboardInput(int key, int scancode, int action, int mods) override;
    void processScroll(double xoffset, double yoffset) override;
    void processMouseMove(double x, double y) override;
    void update(float deltaTimeSeconds) override;
    void render() override;
};
