#version 450

layout (push_constant) uniform PushConstant
{
	vec2 Dimensions;
	mat4 Transform;
	vec4 Color;
} Input;

layout (binding = 0) uniform sampler2D Texture;

layout (location = 0) out vec4 FragColor;

void main()
{
	FragColor = Input.Color;
}
