#include <stdlib.h>
#include <stdio.h>
#include "FrameBuffer.h"
#include "Graphics.h"
#include "Swapchain.h"

FrameBuffer FrameBufferCreate(unsigned int width, unsigned int height, bool useStencil)
{
	FrameBuffer frameBuffer = malloc(sizeof(struct FrameBuffer));
	*frameBuffer = (struct FrameBuffer)
	{
		.Width = width,
		.Height = height,
		.ColorTexture = TextureCreate(width, height, TextureFormatColor),
		.DepthTexture = TextureCreate(width, height, TextureFormatDepth),
		.UseStencil = useStencil,
	};
	if (useStencil) { frameBuffer->StencilTexture = TextureCreate(width, height, TextureFormatStencil); }

	VkImageView attachments[] = { frameBuffer->ColorTexture->ImageView, frameBuffer->DepthTexture->ImageView };
	VkFramebufferCreateInfo createInfo =
	{
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.renderPass = Swapchain.RenderPass,
		.attachmentCount = 2,
		.pAttachments = attachments,
		.width = width,
		.height = height,
		.layers = 1,
	};
	VkResult result = vkCreateFramebuffer(Graphics.Device, &createInfo, NULL, &frameBuffer->Instance);
	if (result != VK_SUCCESS)
	{
		printf("[Error] Failed to create frame buffer: %i", result);
		exit(-1);
	}
	
	frameBuffer->DescriptorSets = malloc(Graphics.FrameResourceCount * sizeof(VkDescriptorSet));
	for (int i = 0; i < Graphics.FrameResourceCount; i++)
	{
		VkDescriptorSetAllocateInfo setInfo =
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = Graphics.DescriptorPool,
			.descriptorSetCount = 1,
			.pSetLayouts = &Graphics.DescriptorSetLayout1,
		};
		vkAllocateDescriptorSets(Graphics.Device, &setInfo, frameBuffer->DescriptorSets + i);
		
		VkDescriptorImageInfo colorInfo =
		{
			.imageLayout = VK_IMAGE_LAYOUT_GENERAL,
			.imageView = frameBuffer->ColorTexture->ImageView,
			.sampler = frameBuffer->ColorTexture->Sampler,
		};
		VkDescriptorImageInfo depthInfo =
		{
			.imageLayout = VK_IMAGE_LAYOUT_GENERAL,
			.imageView = frameBuffer->DepthTexture->ImageView,
			.sampler = frameBuffer->DepthTexture->Sampler,
		};
		VkWriteDescriptorSet writeInfos[] =
		{
			{
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.descriptorCount = 1,
				.dstArrayElement = 0,
				.dstBinding = 0,
				.dstSet = frameBuffer->DescriptorSets[i],
				.pImageInfo = &colorInfo,
			},
			{
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.descriptorCount = 1,
				.dstArrayElement = 0,
				.dstBinding = 1,
				.dstSet = frameBuffer->DescriptorSets[i],
				.pImageInfo = &depthInfo,
			}
		};
		vkUpdateDescriptorSets(Graphics.Device, 2, writeInfos, 0, NULL);
	}
	
	return frameBuffer;
}

FrameBuffer FrameBufferResize(FrameBuffer frameBuffer, unsigned int width, unsigned int height)
{
	GraphicsStopOperations();
	FrameBuffer resized = FrameBufferCreate(width, height, frameBuffer->UseStencil);
	FrameBufferDestroy(frameBuffer);
	return resized;
}

void FrameBufferDestroy(FrameBuffer frameBuffer)
{
	vkFreeDescriptorSets(Graphics.Device, Graphics.DescriptorPool, Graphics.FrameResourceCount, frameBuffer->DescriptorSets);
	free(frameBuffer->DescriptorSets);
	TextureDestroy(frameBuffer->ColorTexture);
	TextureDestroy(frameBuffer->DepthTexture);
	if (frameBuffer->UseStencil) { TextureDestroy(frameBuffer->StencilTexture); }
	vkDestroyFramebuffer(Graphics.Device, frameBuffer->Instance, NULL);
	free(frameBuffer);
}
