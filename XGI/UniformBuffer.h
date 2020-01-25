#ifndef Uniforms_h
#define Uniforms_h

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"
#include "FrameBuffer.h"
#include "LinearMath.h"

typedef struct uniform
{
	Matrix4x4 Transform;
	Matrix4x4 Camera;
	Vector4 Dimensions;
	Vector4 Time;
} uniform_t;

typedef struct UniformBuffer
{
	VkBuffer Buffer;
	VmaAllocation Allocation;
	VkDescriptorSet * DescriptorSets;
} * UniformBuffer_T;

struct UniformBufferInterface
{
	UniformBuffer_T (*Create)(uniform_t uniform);
	void (*Update)(UniformBuffer_T uniformBuffer, uniform_t uniform);
	void (*Destroy)(UniformBuffer_T uniformBuffer);
} extern UniformBuffer;

#endif
