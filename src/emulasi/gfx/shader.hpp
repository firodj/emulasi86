#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <string>

namespace emulasi {

class ShaderCode
{
public:
    ShaderCode() {};
    void LoadFromFile(const char* shaderPath, GLenum shaderType);
    void Compile();
    void Remove();

    GLuint GetID() { return m_id; }

protected:
    std::string m_code{};
    GLuint m_id{};
    GLenum m_type{};
};

class ShaderProgram {
public:
    ShaderProgram() {};
    void LoadFromFile(std::string vertexPath, std::string fragmentPath);
    void Compile();
    void Activate();
    void SetBool(const std::string &name, bool value) const;
    void SetInt(const std::string &name, int value) const;
    void SetFloat(const std::string &name, float value) const;
    void SetMatrix4(const std::string &name, bool transpose, const glm::mat4 &value) const;
    void SetVec3(const std::string &name, const glm::vec3 &value) const;
    bool IsOk() { return m_isOk; }
    GLuint GetID() { return m_id; }

protected:
    bool m_isOk{};
    GLuint m_id{};
    ShaderCode m_vertexShader{};
    ShaderCode m_fragmentShader{};
};

