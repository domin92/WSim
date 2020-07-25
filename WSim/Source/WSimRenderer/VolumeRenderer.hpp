#pragma once

#include "Source/WSimRenderer/Renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

struct VolumeRendererCallbacks {
    virtual void stepSimulation(float deltaTimeSeconds) = 0;
    virtual char *getVolumeBuffers() = 0;
};

class VolumeRenderer : public Renderer {
public:
    VolumeRenderer(VolumeRendererCallbacks &callbacks, int nodeSizeInVoxels, int gridSizeInNodes, int screenSize);
    ~VolumeRenderer() override;

protected:
    glm::mat4 createMvp();
    void loadBuffers();
    void loadShaders();

    void processInput(int button, int action, int mods) override;
    void processKeyboardInput(int key, int scancode, int action, int mods) override;
    void processScroll(double xoffset, double yoffset) override;
    void processMouseMove(double x, double y) override;
    void update(float deltaTimeSeconds) override;
    void render() override;

    

    VolumeRendererCallbacks &callbacks;
    const int nodeSizeInVoxels;
    const int gridSizeInNodes;
    const int screenSize;

    glm::mat4 mvp;

    unsigned int VAO, VBO, EBO;
    OGL::ShaderProgram shaderProgram;

    GLuint mvpVertexUniformLocation;
    GLuint mvpFragmentUniformLocation;
    GLuint nodeSizeUniformLocation;
    GLuint gridSizeUniformLocation;

    GLuint waterTexture;

    glm::vec3 cameraPos;
    glm::vec3 cameraFront; 
    glm::vec3 cameraUp;

    bool firstMouse = true;
    float yaw = -90.0f;
    float pitch = 0.0f;
    float lastX = 800.0f / 2.0;
    float lastY = 600.0 / 2.0;
    float fov = 45.0f;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
};
