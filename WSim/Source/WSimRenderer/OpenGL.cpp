#include "OpenGL.h"

#include <fstream>

namespace OGL {

Shader createShaderFromFile(GLenum shaderType, const std::string &filePath) {
    std::ifstream file(std::string{SHADERS_DIR} + "/" + filePath);
    wsimErrorUnless(file.good());

    const std::string source{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
    return createShaderFromSource(shaderType, source);
}

Shader createShaderFromSource(GLenum shaderType, const std::string &source) {
    // Create shader object
    Shader shader = glCreateShader(shaderType);

    // Compile
    const auto sourceCstr = source.c_str();
    glShaderSource(shader, 1, &sourceCstr, NULL);
    glCompileShader(shader);

    // Get status
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << infoLog << '\n';
        wsimError();
    }

    // Return
    ASSERT_GL_NO_ERROR();
    return shader;
}

ShaderProgram createShaderProgram(Shader &shader1, Shader &shader2) {
    // Create program object
    ShaderProgram shaderProgram = glCreateProgram();

    // Link
    glAttachShader(shaderProgram, shader1);
    ASSERT_GL_NO_ERROR();
    glAttachShader(shaderProgram, shader2);
    ASSERT_GL_NO_ERROR();
    glLinkProgram(shaderProgram);
    ASSERT_GL_NO_ERROR();

    // Get status
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << infoLog << '\n';
        wsimError();
    }

    // Return
    ASSERT_GL_NO_ERROR();
    ASSERT_GL_NO_ERROR();
    ASSERT_GL_NO_ERROR();
    return shaderProgram;
}

} // namespace OGL
