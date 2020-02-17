#include <stdlib.h>
#include <stdio.h>
#include "FrameBuffer.h"
#include "Graphics.h"
#include "log.h"

FrameBuffer FrameBufferCreate(FrameBufferConfigure config)
{
	FrameBuffer frameBuffer = malloc(sizeof(struct FrameBuffer));
	*frameBuffer = (struct FrameBuffer)
	{
		.Width = config.Width,
		.Height = config.Height,
		.Filter = config.Filter,
		.AddressMode = config.AddressMode,
	};
	TextureConfigure textureConfig =
	{
		.Width = config.Width,
		.Height = config.Height,
		.Format = TextureFormatColor,
		.Filter = config.Filter,
		.AddressMode = config.AddressMode,
		.AnisotropicFiltering = false,
		.LoadFromData = false,
	};
	frameBuffer->ColorTexture = TextureCreate(textureConfig);
	textureConfig.Format = TextureFormatDepthStencil;
	frameBuffer->DepthTexture = TextureCreate(textureConfig);

	VkImageView attachments[] = { frameBuffer->ColorTexture->ImageView, frameBuffer->DepthTexture->ImageView };
	VkFramebufferCreateInfo createInfo =
	{
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.renderPass = Graphics.RenderPass,
		.attachmentCount = 2,
		.pAttachments = attachments,
		.width = config.Width,
		.height = config.Height,
		.layers = 1,
	};
	VkResult result = vkCreateFramebuffer(Graphics.Device, &createInfo, NULL, &frameBuffer->Instance);
	if (result != VK_SUCCESS)
	{
		log_fatal("[Error] Failed to create frame buffer: %i\n", result);
		exit(1);
	}
	
	return frameBuffer;
}

unsigned int FrameBufferGetWidth(FrameBuffer frameBuffer) { return frameBuffer->Width; }

unsigned int FrameBufferGetHeight(FrameBuffer frameBuffer) { return frameBuffer->Height; }

TextureFilter FrameBufferGetFilter(FrameBuffer frameBuffer) { return frameBuffer->Filter; }

TextureAddressMode FrameBufferGetAddressMode(FrameBuffer frameBuffer) { return frameBuffer->AddressMode; }

FrameBuffer FrameBufferResize(FrameBuffer frameBuffer, unsigned int width, unsigned int height)
{
	GraphicsStopOperations();
	FrameBufferConfigure config =
	{
		.Width = width,
		.Height = height,
		.Filter = frameBuffer->Filter,
		.AddressMode = frameBuffer->AddressMode,
	};
	FrameBuffer resized = FrameBufferCreate(config);
	FrameBufferDestroy(frameBuffer);
	return resized;
}

void FrameBufferQueueDestroy(FrameBuffer frameBuffer)
{
	ListPush(Graphics.FrameResources[Graphics.FrameIndex].DestroyFrameBufferQueue, frameBuffer);
}

void FrameBufferDestroy(FrameBuffer frameBuffer)
{
	TextureDestroy(frameBuffer->ColorTexture);
	TextureDestroy(frameBuffer->DepthTexture);
	vkDestroyFramebuffer(Graphics.Device, frameBuffer->Instance, NULL);
	free(frameBuffer);
}
