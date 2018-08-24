#include "Shader.h"
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

namespace learnogl
{

namespace ogl
{
	Shader::Shader(const char *vertexPath, const char *fragPath, const char *geometryPath)
	{
		std::ifstream vertexFile;
		std::ifstream fragFile;
		std::ifstream geometryFile;
		vertexFile.exceptions(std::ifstream::failbit|std::ifstream::badbit);
		fragFile.exceptions(std::ifstream::failbit|std::ifstream::badbit);
		geometryFile.exceptions(std::ifstream::failbit|std::ifstream::badbit);

		std::string vertexCode;
		std::string fragCode;
		std::string geometryCode;

		try
		{
			vertexFile.open(vertexPath);
			fragFile.open(fragPath);
			std::stringstream vSS, fSS;

			
			vSS << vertexFile.rdbuf();
			fSS << fragFile.rdbuf();

			vertexFile.close();
			fragFile.close();
			
			vertexCode = vSS.str();
			fragCode = fSS.str();
			
			if (geometryPath != nullptr)
			{
				geometryFile.open(geometryPath);
				std::stringstream  gSS;
				gSS << geometryFile.rdbuf();
				geometryFile.close();
				geometryCode = gSS.str();
			}
		}
		catch(std::ifstream::failure e)
		{
			std::cout << "ERROR:" << e.what();
		}

		const char *vCode = vertexCode.c_str();
		const char *fCode = fragCode.c_str();

		unsigned int vertex, fragment;

		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vCode, NULL);
		glCompileShader(vertex);
		CheckCompileErrors(vertex, "VERTEX");

		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fCode, NULL);
		glCompileShader(fragment);
		CheckCompileErrors(fragment, "FRAGMENT");

		unsigned int geometry;
		if (geometryPath != nullptr)
		{
			const char *gCode = geometryCode.c_str();
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gCode, NULL);
			glCompileShader(geometry);
			CheckCompileErrors(geometry, "GEOMETRY");
		}

		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		if (geometryPath != nullptr)
		{
			glAttachShader(ID, geometry);
		}
		glLinkProgram(ID);
		CheckCompileErrors(ID, "PROGRAM");

		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (geometryPath != nullptr)
			glDeleteShader(geometry);
	}

	Shader::~Shader()
	{
		glLinkProgram(0);
		glDeleteProgram(ID);
	}

	void Shader::CheckCompileErrors(GLuint shader, std::string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}

	void Shader::Use()
	{
		glUseProgram(ID);
	}

	void Shader::SetBool(const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	void Shader::SetInteger(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	void Shader::SetFloat(const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

	void Shader::SetVec2(const std::string &name, glm::vec2 &value) const
	{
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void Shader::SetVec2(const std::string &name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}

	void Shader::SetVec3(const std::string &name, glm::vec3 &value) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void Shader::SetVec3(const std::string &name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}

	void Shader::SetVec4(const std::string &name, glm::vec4 &value) const
	{
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void Shader::SetVec4(const std::string &name, float x, float y, float z, float w) const
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}

	void Shader::SetMat2(const std::string &name, glm::mat2 &value)  const
	{
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
	}

	void Shader::SetMat3(const std::string &name, glm::mat3 &value) const
	{
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
	}

	void Shader::SetMat4(const std::string &name, glm::mat4 &value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
	}

	void Shader::SetMat4s(const std::string &name, int count, glm::mat4 &value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), count, GL_FALSE, &value[0][0]);
	}
}

}