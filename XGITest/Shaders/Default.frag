#version 450

layout (location = 0) in vec4 VertexColor;

layout (push_constant) uniform PushConstant
{
	float Time;
	vec4 Dimensions;
	mat4 Transform;
	vec4 Color;
} Input;

layout (binding = 0) uniform sampler2D Texture;

layout (binding = 1) uniform UBO
{
	float Time;
} UBOInput;

layout (location = 0) out vec4 FragColor;

void main()
{
	FragColor = VertexColor * Input.Color;
}
