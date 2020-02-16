#version 450

layout (location = 1) in vec2 VertexUV;

layout (binding = 0) uniform sampler2D Texture;

layout (location = 0) out vec4 FragColor;

void main()
{
	FragColor = texture(Texture, VertexUV);
}
