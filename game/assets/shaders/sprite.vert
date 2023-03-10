#version 430 core

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTexCoord;

out vec2 texCoord;

uniform mat4 model;
uniform mat4 projection;

void main()
{
	texCoord = inTexCoord;
	gl_Position = projection * model * vec4(inPos, 0.0, 1.0);
}