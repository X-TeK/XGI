#version 450

layout (location = 0) in vec4 VertexColor;
layout (location = 1) in vec2 VertexUV;

layout (binding = 0) uniform sampler2D Texture;

layout (location = 0) out vec4 FragColor;

void main()
{
	FragColor = VertexColor * texture(Texture, VertexUV);
}