#ifndef Uniform_h
#define Uniform_h

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"
#include "FrameBuffer.h"
#include "LinearMath.h"
#include "Pipeline.h"

struct Pipeline;

typedef struct UniformBuffer
{
	VkBuffer Buffer;
	VmaAllocation Allocation;
	SpvReflectBlockVariable Info;
	unsigned int Size;
} * UniformBuffer;

UniformBuffer UniformBufferCreate(struct Pipeline * pipeline, int binding);
void UniformBufferSetVariable(UniformBuffer uniformBuffer, const char * variable, void * value);
void UniformBufferQueueDestroy(UniformBuffer uniformBuffer);
void UniformBufferDestroy(UniformBuffer uniformBuffer);

#endif
