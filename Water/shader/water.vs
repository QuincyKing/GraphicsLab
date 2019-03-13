#version 430 core

layout(location = 0)in vec3 position;
layout(location = 1)in vec2 tex;

out vec4 clipSpace;
out vec2 textureCoords;
out vec3 toCamera;
out vec3 fromLight;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 cameraPos;
uniform vec3 lightPos;

void main() 
{
	vec4 worldPos = model * vec4(position.x, 0.0, position.y, 1.0);
	clipSpace = projection * view * model * vec4(position.x, 0.0, position.y, 1.0);
	gl_Position = clipSpace;
	textureCoords = tex;
	toCamera = cameraPos - worldPos.xyz;
	fromLight = worldPos.xyz - lightPos;
}