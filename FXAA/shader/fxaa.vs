#version 430 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTex;

out vec2 Tex;

void main(void)
{
	Tex = aTex;
	gl_Position = vec4(aPos, 0.0, 1.0);
}
