#version 450

layout (location = 0) in vec4 VertexColor;
layout (location = 1) in vec2 VertexUV;

layout (push_constant) uniform PushConstant
{
	mat4 Transform;
	vec4 Color;
} Input;

layout (binding = 1) uniform sampler2D Texture;

layout (location = 0) out vec4 FragColor;

void main()
{
	FragColor = Input.Color * texture(Texture, VertexUV);
	FragColor = FragColor.bgra;
}
