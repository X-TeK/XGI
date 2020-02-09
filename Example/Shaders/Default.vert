#version 450

layout (location = 0) in vec3 PositionAttribute;
layout (location = 1) in vec4 ColorAttribute;
layout (location = 2) in vec2 UVAttribute;

layout (push_constant) uniform PushConstant
{
	mat4 Transform;
} Input;

layout (location = 0) out vec4 VertexColor;
layout (location = 1) out vec2 VertexUV;

void main()
{
	VertexColor = ColorAttribute;
	VertexUV = UVAttribute;
	gl_Position = Input.Transform * vec4(PositionAttribute, 1.0);
}