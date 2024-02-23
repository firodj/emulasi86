#pragma once

#include <glm/glm.hpp>
#include <string>

class Shader
{
public:
  // constructor reads and builds the shader
  Shader();
  void Load(const char* vertexPath, const char* fragmentPath);
  void Compile();

  // use/activate the shader
  void Activate();

  // utility uniform functions
  void SetBool(const std::string &name, bool value) const;
  void SetInt(const std::string &name, int value) const;
  void SetFloat(const std::string &name, float value) const;
  void SetMatrix4(const std::string &name, bool transpose, glm::mat4 &value) const;

  int shaderProgram() { return m_shaderProgram; }

protected:
  // the program ID
  unsigned int m_shaderProgram;

  std::string m_vertexCode;
  std::string m_fragmentCode;
};
