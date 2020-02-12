#version 450

layout (location = 0) in vec3 PositionAttribute;
layout (location = 1) in vec2 UVAttribute;

layout (push_constant) uniform PushConstant
{
	mat4 Transform;
} Input;

layout (location = 1) out vec2 VertexUV;

void main()
{
	VertexUV = UVAttribute;
	gl_Position = Input.Transform * vec4(PositionAttribute, 1.0);
}
