#include "Shader.h"

#include <glad/gl.h> // include glad to get all the required OpenGL headers
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader() {

}

void Shader::Load(const char* vertexPath, const char* fragmentPath) {
  // 1. retrieve the vertex/fragment source code from filePath
  std::string vertexCode;
  std::string fragmentCode;
  std::ifstream vShaderFile;
  std::ifstream fShaderFile;

  // ensure ifstream objects can throw exceptions:
  vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
  try
  {
      // open files
      std::cout << "opening vertex shader: " << vertexPath << " ..." << std::endl;
      vShaderFile.open(vertexPath);

      std::cout << "opening fragment shader: " << fragmentPath << " ..." << std::endl;
      fShaderFile.open(fragmentPath);

      std::stringstream vShaderStream, fShaderStream;
      // read file's buffer contents into streams
      vShaderStream << vShaderFile.rdbuf();
      fShaderStream << fShaderFile.rdbuf();
      // close file handlers
      vShaderFile.close();
      fShaderFile.close();
      // convert stream into string
      m_vertexCode   = vShaderStream.str();
      m_fragmentCode = fShaderStream.str();
  }
  catch(std::ifstream::failure e)
  {
      std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
  }
}

void Shader::Compile() {
  const char* vertexShaderSource = m_vertexCode.c_str();
  const char* fragmentShaderSource = m_fragmentCode.c_str();

  // build and compile our shader program
  // ------------------------------------
  // vertex shader
  int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  // check for shader compile errors
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  // fragment shader
  int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  // check for shader compile errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  // link shaders
  m_shaderProgram = glCreateProgram();
  glAttachShader(m_shaderProgram, vertexShader);
  glAttachShader(m_shaderProgram, fragmentShader);
  glLinkProgram(m_shaderProgram);
  // check for linking errors
  glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

void Shader::Activate() {
  glUseProgram(m_shaderProgram);
}

void Shader::SetBool(const std::string &name, bool value) const
{
  glUniform1i(glGetUniformLocation(m_shaderProgram, name.c_str()), (int)value);
}
void Shader::SetInt(const std::string &name, int value) const
{
  glUniform1i(glGetUniformLocation(m_shaderProgram, name.c_str()), value);
}
void Shader::SetFloat(const std::string &name, float value) const
{
  glUniform1f(glGetUniformLocation(m_shaderProgram, name.c_str()), value);
}

/**
 * Set Matrix 4x4
 */
void Shader::SetMatrix4(const std::string &name, bool transpose, glm::mat4 &value) const {
  glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, name.c_str()), 1,
    transpose ? GL_TRUE : GL_FALSE,
    glm::value_ptr(value)
  );
}