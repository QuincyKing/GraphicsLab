#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
void main()
{
	FragPos = (view * model * vec4(aPos, 1.0)).xyz;

	mat3 invertedMatrix = transpose(inverse(mat3(view * model)));
	Normal = invertedMatrix * aNormal;
	gl_Position = projection * vec4(FragPos, 1.0);
}