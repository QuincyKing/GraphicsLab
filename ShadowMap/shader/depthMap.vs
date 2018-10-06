#version  430 core
layout(location = 0) in vec3 Position;

uniform mat4 lightSpace;
uniform mat4 model;

void main()
{
	gl_Position = lightSpace * model * vec4(Position, 1.0);
}