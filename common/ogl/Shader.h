#ifndef Shader_H
#define Shader_H

#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include <fstream>
#include <sstream>
#include <memory>

#include <GL/glew.h>
#include <glm/glm.hpp>


namespace learnogl
{

namespace ogl
{

class Shader
{
public:
	Shader(const char *vertexPath, const char *fragPath, const char *geometryPath = nullptr);
	~Shader();

public:
	void Use();
	void SetBool(const std::string &name, bool value) const;
	void SetInteger(const std::string &name, int value) const;
	void SetFloat(const std::string &name, float value) const;
	void SetVec2(const std::string &name, glm::vec2 &value) const;
	void SetVec2(const std::string &name, float x, float y) const;
	void SetVec3(const std::string &name, glm::vec3 &value) const;
	void SetVec3(const std::string &name, float x, float y, float z) const;
	void SetVec4(const std::string &name, glm::vec4 &value) const;
	void SetVec4(const std::string &name, float x, float y, float z, float w) const;
	void SetMat2(const std::string &name, glm::mat2 &value)  const;
	void SetMat3(const std::string &name, glm::mat3 &value) const;
	void SetMat4(const std::string &name, glm::mat4 &value) const;
	void SetMat4s(const std::string &name, int count, glm::mat4 &value) const;

private:
	unsigned int ID;

	void CheckCompileErrors(GLuint shader, std::string type);
};


}

}
#endif

