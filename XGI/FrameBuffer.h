#ifndef FrameBuffer_h
#define FrameBuffer_h

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"
#include "Texture.h"
#include "Pipeline.h"

typedef struct FrameBufferConfigure
{
	unsigned int Width, Height;
	TextureFilter Filter;
	TextureAddressMode AddressMode;
	bool UseStencil;
} FrameBufferConfigure;

typedef struct FrameBuffer
{
	unsigned int Width, Height;
	TextureFilter Filter;
	TextureAddressMode AddressMode;
	Texture ColorTexture;
	Texture DepthTexture;
	VkFramebuffer Instance;
} * FrameBuffer;

FrameBuffer FrameBufferCreate(FrameBufferConfigure config);
FrameBuffer FrameBufferResize(FrameBuffer frameBuffer, unsigned int width, unsigned int height);
void FrameBufferDestroy(FrameBuffer frameBuffer);

#endif
