#version 430 core 
layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 Tex;

out vec2 Texcoords;

void main()
{
	Texcoords = Tex;
	gl_Position = vec4(Position, 1.0);
}