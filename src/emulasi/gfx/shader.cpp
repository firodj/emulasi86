#include "emulasi/gfx/shader.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "emulasi/util/log.hpp"

namespace emulasi {

void ShaderCode::LoadFromFile(const char* shaderPath, GLenum shaderType)
{
	std::ifstream fShader;
	fShader.exceptions (std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		fShader.open(shaderPath);
		std::stringstream sShader;
		sShader << fShader.rdbuf();
		fShader.close();
		m_code = sShader.str();
		m_type = shaderType;
	}
	catch(std::ifstream::failure e)
	{
		std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
}

void ShaderCode::Compile()
{
	const char* srcShader = m_code.c_str();
	m_id = glCreateShader(m_type);
	glShaderSource(m_id, 1, &srcShader, NULL);
	glCompileShader(m_id);

	int success;
	glGetShaderiv(m_id, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(m_id, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
}

void ShaderCode::Remove()
{
	glDeleteShader(m_id);
	m_id = {};
}

void ShaderProgram::LoadFromFile(std::string vertexPath, std::string fragmentPath) {
	m_vertexShader.LoadFromFile(vertexPath.c_str(), GL_VERTEX_SHADER);
	m_fragmentShader.LoadFromFile(fragmentPath.c_str(), GL_FRAGMENT_SHADER);
}

void ShaderProgram::Compile() {
	m_vertexShader.Compile();
	m_fragmentShader.Compile();

	m_id = glCreateProgram();
	glAttachShader(m_id, m_vertexShader.GetID());
	glAttachShader(m_id, m_fragmentShader.GetID());
	glLinkProgram(m_id);

	int success;
	glGetProgramiv(m_id, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(m_id, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	m_vertexShader.Remove();
	m_fragmentShader.Remove();
	m_isOk = true;
}

void ShaderProgram::Activate() {
	glUseProgram(m_id);
}

void ShaderProgram::SetBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value);
}

void ShaderProgram::SetInt(const std::string &name, int value) const
{

	glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void ShaderProgram::SetFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void ShaderProgram::SetVec3(const std::string &name, const glm::vec3 &value) const
{
	glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, glm::value_ptr(value));
}

void ShaderProgram::SetMatrix4(const std::string &name, bool transpose, const glm::mat4 &value) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1,
		transpose ? GL_TRUE : GL_FALSE,
		glm::value_ptr(value)
	);
}

}
