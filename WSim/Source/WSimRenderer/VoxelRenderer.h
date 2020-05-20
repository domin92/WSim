#pragma once

#include "Source/WSimRenderer/Renderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

struct VoxelRendererCallbacks {
    virtual void stepSimulation(float deltaTimeSeconds) = 0;
    virtual char* getVoxelBuffers() = 0;
};

class VoxelRenderer : public Renderer {
public:
    VoxelRenderer(VoxelRendererCallbacks &callbacks, int nodeSizeInVoxels, int gridSizeInNodes, int screenSize);
    ~VoxelRenderer() override;

protected:
    static glm::mat4 createMvp(int screenSize);
    void loadBuffers();
    void loadShaders();

    void processInput(int button, int action, int mods) override;
    void processMouseMove(double x, double y) override {}
    void update(float deltaTimeSeconds) override;
    void render() override;

    VoxelRendererCallbacks &callbacks;
    const int nodeSizeInVoxels;
    const int gridSizeInNodes;
    const int gridSizeInVoxels;
    const int screenSize;

    const glm::mat4 mvp;

    unsigned int VAO, VBO, EBO;
    OGL::ShaderProgram shaderProgram;

    GLuint mvpUniformLocation;
    GLuint nodeSizeUniformLocation;
    GLuint gridSizeUniformLocation;

    GLuint waterTexture;
};
