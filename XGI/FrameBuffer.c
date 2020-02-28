#include <stdlib.h>
#include <stdio.h>
#include "FrameBuffer.h"
#include "Graphics.h"
#include "log.h"

static void ValidateFrameBufferObject(FrameBuffer frameBuffer)
{
	if (frameBuffer == NULL)
	{
		log_fatal("Trying to perform operations on an uninitialized FrameBuffer object.\n");
		exit(1);
	}
}

FrameBuffer FrameBufferCreate(FrameBufferConfigure config)
{
	if (config.Width <= 0 || config.Height <= 0)
	{
		log_fatal("Trying to create a FrameBuffer object with negative dimensions.\n");
		exit(1);
	}
	if (config.Filter < 0 || config.Filter >= TextureFilterCount)
	{
		log_fatal("Trying to create a FrameBuffer object, but config.Filter is outside the valid range of enumerations.\n");
		exit(1);
	}
	if (config.AddressMode < 0 || config.AddressMode >= TextureAddressModeCount)
	{
		log_fatal("Trying to create a FrameBuffer object, but config.AddressMode is outside the valid range of enumerations.\n");
		exit(1);
	}
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
		log_fatal("Trying to create FrameBuffer object, but failed to create VkFramebuffer: %i\n", result);
		exit(1);
	}
	
	return frameBuffer;
}

unsigned int FrameBufferWidth(FrameBuffer frameBuffer)
{
	ValidateFrameBufferObject(frameBuffer);
	return frameBuffer->Width;
}

unsigned int FrameBufferHeight(FrameBuffer frameBuffer)
{
	ValidateFrameBufferObject(frameBuffer);
	return frameBuffer->Height;
}

TextureFilter FrameBufferFilter(FrameBuffer frameBuffer)
{
	ValidateFrameBufferObject(frameBuffer);
	return frameBuffer->Filter;
}

TextureAddressMode FrameBufferAddressMode(FrameBuffer frameBuffer)
{
	ValidateFrameBufferObject(frameBuffer);
	return frameBuffer->AddressMode;
}

FrameBuffer FrameBufferResize(FrameBuffer frameBuffer, unsigned int width, unsigned int height)
{
	ValidateFrameBufferObject(frameBuffer);
	if (width <= 0 || height <= 0)
	{
		log_fatal("Trying to resize FrameBuffer object into a negative width or height.\n");
		exit(1);
	}
	GraphicsStopOperations();
	FrameBufferConfigure config =
	{
		.Width = width,
		.Height = height,
		.Filter = frameBuffer->Filter,
		.AddressMode = frameBuffer->AddressMode,
	};
	FrameBuffer resized = FrameBufferCreate(config);
	FrameBufferQueueDestroy(frameBuffer);
	return resized;
}

void FrameBufferQueueDestroy(FrameBuffer frameBuffer)
{
	ValidateFrameBufferObject(frameBuffer);
	if (ListContains(Graphics.FrameResources[Graphics.FrameIndex].Queues[GraphicsQueueDestroyFrameBuffer], frameBuffer))
	{
		log_fatal("Trying to queue destroy a FrameBuffer that was already placed into the queue to be destroyed.\n");
		exit(1);
	}
	ListPush(Graphics.FrameResources[Graphics.FrameIndex].Queues[GraphicsQueueDestroyFrameBuffer], frameBuffer);
}

void FrameBufferDestroy(FrameBuffer frameBuffer)
{
	ValidateFrameBufferObject(frameBuffer);
	TextureDestroy(frameBuffer->ColorTexture);
	TextureDestroy(frameBuffer->DepthTexture);
	vkDestroyFramebuffer(Graphics.Device, frameBuffer->Instance, NULL);
	free(frameBuffer);
}
