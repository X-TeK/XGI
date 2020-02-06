#include <stdlib.h>
#include <stdio.h>
#include "FrameBuffer.h"
#include "Graphics.h"
#include "Swapchain.h"

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
	};
	frameBuffer->ColorTexture = TextureCreate(textureConfig);
	textureConfig.Format = TextureFormatDepth;
	frameBuffer->DepthTexture = TextureCreate(textureConfig);

	VkImageView attachments[] = { frameBuffer->ColorTexture->ImageView, frameBuffer->DepthTexture->ImageView };
	VkFramebufferCreateInfo createInfo =
	{
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.renderPass = Swapchain.RenderPass,
		.attachmentCount = 2,
		.pAttachments = attachments,
		.width = config.Width,
		.height = config.Height,
		.layers = 1,
	};
	VkResult result = vkCreateFramebuffer(Graphics.Device, &createInfo, NULL, &frameBuffer->Instance);
	if (result != VK_SUCCESS)
	{
		printf("[Error] Failed to create frame buffer: %i", result);
		exit(-1);
	}
	
	return frameBuffer;
}

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

void FrameBufferDestroy(FrameBuffer frameBuffer)
{
	TextureDestroy(frameBuffer->ColorTexture);
	TextureDestroy(frameBuffer->DepthTexture);
	vkDestroyFramebuffer(Graphics.Device, frameBuffer->Instance, NULL);
	free(frameBuffer);
}
