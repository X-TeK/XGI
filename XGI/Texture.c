#include <stdlib.h>
#include <stdio.h>
#include "vk_mem_alloc.h"
#include "Texture.h"
#include "Graphics.h"
#include "Swapchain.h"

static void CreateImage(Texture texture, VkFormat format, VkImageUsageFlags usage)
{
	VkImageCreateInfo imageInfo =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = format,
		.extent =
		{
			.width = texture->Width,
			.height = texture->Height,
			.depth = 1,
		},
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	};
	VmaAllocationCreateInfo allocationInfo =
	{
		.usage = VMA_MEMORY_USAGE_GPU_ONLY,
	};
	VkResult result = vmaCreateImage(Graphics.Allocator, &imageInfo, &allocationInfo, &texture->Image, &texture->Allocation, NULL);
	if (result != VK_SUCCESS)
	{
		printf("[Error] Unable to create image: %i", result);
		exit(-1);
	}
}

static void TransitionImageLayout(Texture texture, VkImageLayout layout)
{
	VkCommandBuffer commandBuffer;
	VkCommandBufferAllocateInfo commandAllocateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandPool = Graphics.CommandPool,
		.commandBufferCount = 1,
	};
	vkAllocateCommandBuffers(Graphics.Device, &commandAllocateInfo, &commandBuffer);
	VkCommandBufferBeginInfo beginInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
	};
	
	vkBeginCommandBuffer(commandBuffer, &beginInfo);
	VkImageMemoryBarrier barrier =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.newLayout = layout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = texture->Image,
		.subresourceRange =
		{
			.aspectMask = texture->ImageAspect,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
	};
	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, 1, &barrier);
	vkEndCommandBuffer(commandBuffer);
	
	VkFence fence;
	VkFenceCreateInfo fenceInfo = { .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	vkCreateFence(Graphics.Device, &fenceInfo, NULL, &fence);
	VkSubmitInfo submitInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers = &commandBuffer,
	};
	vkQueueSubmit(Graphics.GraphicsQueue, 1, &submitInfo, fence);
	vkWaitForFences(Graphics.Device, 1, &fence, VK_TRUE, UINT64_MAX);
	vkDestroyFence(Graphics.Device, fence, NULL);
	vkFreeCommandBuffers(Graphics.Device, Graphics.CommandPool, 1, &commandBuffer);
}

static void CreateImageView(Texture texture, VkFormat format)
{
	VkImageViewCreateInfo createInfo =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = texture->Image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = format,
		.subresourceRange =
		{
			.aspectMask = texture->ImageAspect,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
	};
	VkResult result = vkCreateImageView(Graphics.Device, &createInfo, NULL, &texture->ImageView);
	if (result != VK_SUCCESS)
	{
		printf("[Error] Unable to create image view: %i", result);
		exit(-1);
	}
}

static void CreateSampler(Texture texture)
{
	VkSamplerCreateInfo samplerInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.magFilter = VK_FILTER_LINEAR,
		.minFilter = VK_FILTER_LINEAR,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
		.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
		.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
		.anisotropyEnable = VK_FALSE,
		.compareEnable = VK_FALSE,
		.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
		.unnormalizedCoordinates = VK_FALSE,
	};
	VkResult result = vkCreateSampler(Graphics.Device, &samplerInfo, NULL, &texture->Sampler);
	if (result != VK_SUCCESS)
	{
		printf("[Error] Unable to create sampler: %i", result);
		exit(-1);
	}
}

Texture TextureCreate(unsigned int width, unsigned int height, TextureFormat format)
{
	Texture texture = malloc(sizeof(struct Texture));
	*texture = (struct Texture){ .Width = width, .Height = height, .Format = format };
	
	if (format == TextureFormatColor)
	{
		texture->ImageAspect = VK_IMAGE_ASPECT_COLOR_BIT;
		CreateImage(texture, Swapchain.ColorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
		TransitionImageLayout(texture, VK_IMAGE_LAYOUT_GENERAL);
		CreateImageView(texture, Swapchain.ColorFormat);
		CreateSampler(texture);
	}
	else if (format == TextureFormatDepth)
	{
		texture->ImageAspect = VK_IMAGE_ASPECT_DEPTH_BIT;
		CreateImage(texture, Swapchain.DepthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
		TransitionImageLayout(texture, VK_IMAGE_LAYOUT_GENERAL);
		CreateImageView(texture, Swapchain.DepthFormat);
		CreateSampler(texture);
	}
	else
	{
		texture->ImageAspect = VK_IMAGE_ASPECT_STENCIL_BIT;
		CreateImage(texture, Swapchain.StencilFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
		TransitionImageLayout(texture, VK_IMAGE_LAYOUT_GENERAL);
		CreateImageView(texture, Swapchain.StencilFormat);
		CreateSampler(texture);
	}
	
	return texture;
}

void TextureDestroy(Texture texture)
{
	vkDestroySampler(Graphics.Device, texture->Sampler, NULL);
	vkDestroyImageView(Graphics.Device, texture->ImageView, NULL);
	vmaDestroyImage(Graphics.Allocator, texture->Image, texture->Allocation);
	free(texture);
}
