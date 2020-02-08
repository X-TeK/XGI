#version 450

layout (location = 0) in vec2 PositionAttribute;
layout (location = 1) in vec4 ColorAttribute;

layout (push_constant) uniform PushConstant
{
	mat4 Transform;
	vec4 Color;
} Input;

layout (binding = 0) uniform UBO
{
	vec2 Dimensions;
} UBOInput;

layout (location = 0) out vec4 VertexColor;
layout (location = 1) out vec2 VertexUV;

void main()
{
	VertexColor = ColorAttribute;
	float aspect = UBOInput.Dimensions.x / UBOInput.Dimensions.y;
	gl_Position = Input.Transform * vec4(PositionAttribute.xy, 0.0, 1.0);
	gl_Position.xy /= vec2(aspect, 1.0);
	VertexUV = gl_Position.xy;
}
