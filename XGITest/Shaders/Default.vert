#version 450

layout (location = 0) in vec2 PositionAttribute;

layout (push_constant) uniform PushConstant
{
	vec2 Dimensions;
	mat4 Transform;
	vec4 Color;
} Input;

layout (binding = 0) uniform UBO
{
	mat4 Transform;
} UBOInput;

void main()
{
	float aspect = Input.Dimensions.x / Input.Dimensions.y;
	gl_Position = Input.Transform * vec4(PositionAttribute.xy, 0.0, 1.0);
	gl_Position.xy /= vec2(aspect, 1.0);
}
