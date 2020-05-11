#pragma once

#include "Source/WSimCommon/Error.h"

#include <iostream>
#include <string>

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#define ASSERT_GL_NO_ERROR()                                                         \
    {                                                                                \
        auto error = glGetError();                                                   \
        if (error != GL_NO_ERROR) {                                                  \
            std::cerr << "glGetError = 0x" << std::hex << error << std::dec << '\n'; \
            wsimError();                                                             \
        }                                                                            \
    }

#define DEFINE_RAII_WRAPPER(name, glType, releaseMethod)          \
    class name {                                                  \
    protected:                                                    \
        glType value;                                             \
                                                                  \
    public:                                                       \
        name() = default;                                         \
        name(glType value) : value(value) {}                      \
        name(const name &) = delete;                              \
        name &operator=(const name &) = delete;                   \
        name(name &&other) noexcept { *this = std::move(other); } \
        name &operator=(name &&other) noexcept {                  \
            this->value = other.value;                            \
            other.value = 0;                                      \
            return *this;                                         \
        }                                                         \
        ~name() {                                                 \
            if (value != 0) {                                     \
                releaseMethod(value);                             \
                ASSERT_GL_NO_ERROR();                             \
            }                                                     \
        }                                                         \
        operator glType() const { return value; }                 \
    }

namespace OGL {
DEFINE_RAII_WRAPPER(Shader, GLuint, glDeleteShader);
DEFINE_RAII_WRAPPER(ShaderProgram, GLuint, glDeleteProgram);

Shader createShaderFromFile(GLenum shaderType, const std::string &filePath);
Shader createShaderFromSource(GLenum shaderType, const std::string &source);
ShaderProgram createShaderProgram(Shader &shader1, Shader &shader2);
} // namespace OGL

#undef DEFINE_RAII_WRAPPER
