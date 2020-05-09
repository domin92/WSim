#pragma once

#include "Source/WSimRenderer/Renderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

struct VoxelRendererCallbacks {
    virtual void stepSimulation(float deltaTime) = 0;
};

class VoxelRenderer : public Renderer {
public:
    VoxelRenderer(VoxelRendererCallbacks &callbacks, int nodeSizeInVoxels, int gridSizeInNodes, int screenSize, char **voxelBuffers);
    ~VoxelRenderer() override;

protected:
    static glm::mat4 createMvp(int screenSize);
    void loadBuffers();
    void loadShaders();

    void processInput(int button, int action, int mods) override;
    void processMouseMove(double x, double y) override {}
    void update(float dt) override;
    void render() override;

    VoxelRendererCallbacks &callbacks;
    const int nodeSizeInVoxels;
    const int gridSizeInNodes;
    const int screenSize;
    char **const voxelBuffers;
    const float voxelSize;
    const glm::mat4 mvp;

    unsigned int VAO, VBO, EBO;
    OGL::ShaderProgram shaderProgram;

    GLuint positionUniformLocation;
    GLuint mvpUniformLocation;
};
