#ifndef FrameBuffer_h
#define FrameBuffer_h

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"
#include "Texture.h"
#include "Pipeline.h"

typedef struct FrameBuffer
{
	unsigned int Width, Height;
	Texture ColorTexture;
	Texture DepthTexture;
	bool UseStencil;
	Texture StencilTexture;
	VkFramebuffer Instance;
	VkDescriptorSet * DescriptorSets;
} * FrameBuffer;

FrameBuffer FrameBufferCreate(unsigned int width, unsigned int height, bool useStencil);
FrameBuffer FrameBufferResize(FrameBuffer frameBuffer, unsigned int width, unsigned int height);
void FrameBufferDestroy(FrameBuffer frameBuffer);

#endif
