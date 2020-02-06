#version 450

layout (location = 0) in vec4 VertexColor;

layout (push_constant) uniform PushConstant
{
	mat4 Transform;
	vec4 Color;
} Input;

layout (binding = 1) uniform sampler2D Textures[2];

layout (location = 0) out vec4 FragColor;

void main()
{
	FragColor = VertexColor * Input.Color;
}
