#include <stdlib.h>
#include <stdio.h>
#include <vk_mem_alloc.h>
#include <stb_image.h>
#include "Texture.h"
#include "Graphics.h"
#include "Swapchain.h"
#include "File.h"

TextureData TextureDataFromFile(const char * fileName)
{
	if (!FileExists(fileName)) { exit(-1); }
	int width, height, channels;
	stbi_uc * pixels = stbi_load(fileName, &width, &height, &channels, STBI_rgb_alpha);
	return (TextureData)
	{
		.Width = width,
		.Height = height,
		.Pixels = pixels,
	};
}
void TextureDataDestroy(TextureData data)
{
	stbi_image_free(data.Pixels);
}

static void CreateImage(Texture texture)
{
	VkFormat format = texture->Format == TextureFormatColor ? Swapchain.ColorFormat : (VkFormat)texture->Format;
	VkImageUsageFlags usage = texture->Format == TextureFormatColor ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT : VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
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
		.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | usage,
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

static void TransitionImageLayout(Texture texture)
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
	
	VkImageAspectFlags imageAspect = texture->Format == TextureFormatColor ? VK_IMAGE_ASPECT_COLOR_BIT : VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	VkImageMemoryBarrier barrier =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.newLayout = VK_IMAGE_LAYOUT_GENERAL,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = texture->Image,
		.subresourceRange =
		{
			.aspectMask = imageAspect,
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

static void CopyImageData(Texture texture, TextureConfigure config)
{
	texture->Width = config.Data.Width;
	texture->Height = config.Data.Height;
	unsigned int size = texture->Width * texture->Height * 4;
	
	VkBuffer stagingBuffer;
	VmaAllocation stagingAllocation;
	VkBufferCreateInfo stagingInfo =
	{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = size,
		.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	};
	VmaAllocationCreateInfo stagingAllocationInfo =
	{
		.usage = VMA_MEMORY_USAGE_CPU_ONLY,
	};
	vmaCreateBuffer(Graphics.Allocator, &stagingInfo, &stagingAllocationInfo, &stagingBuffer, &stagingAllocation, NULL);
	
	void * data;
	vmaMapMemory(Graphics.Allocator, stagingAllocation, &data);
	memcpy(data, config.Data.Pixels, size);
	vmaUnmapMemory(Graphics.Allocator, stagingAllocation);
	
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
	
	VkBufferImageCopy copy =
	{
		.bufferOffset = 0,
		.bufferImageHeight = 0,
		.bufferRowLength = 0,
		.imageOffset = { 0, 0, 0 },
		.imageExtent = { texture->Width, texture->Height, 1 },
		.imageSubresource =
		{
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.mipLevel = 0,
			.baseArrayLayer = 0,
			.layerCount = 1,
		}
	};
	vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, texture->Image, VK_IMAGE_LAYOUT_GENERAL, 1, &copy);
	
	vkEndCommandBuffer(commandBuffer);
	VkSubmitInfo submitInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers = &commandBuffer,
		.signalSemaphoreCount = 0,
		.pSignalSemaphores = NULL,
	};
	VkFence fence;
	VkFenceCreateInfo fenceInfo = { .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	vkCreateFence(Graphics.Device, &fenceInfo, NULL, &fence);
	vkQueueSubmit(Graphics.GraphicsQueue, 1, &submitInfo, fence);
	
	vkWaitForFences(Graphics.Device, 1, &fence, VK_TRUE, UINT64_MAX);
	vkDestroyFence(Graphics.Device, fence, NULL);
	vkFreeCommandBuffers(Graphics.Device, Graphics.CommandPool, 1, &commandBuffer);
	vmaDestroyBuffer(Graphics.Allocator, stagingBuffer, stagingAllocation);
}

static void CreateImageView(Texture texture)
{
	VkImageAspectFlags imageAspect = texture->Format == TextureFormatColor ? VK_IMAGE_ASPECT_COLOR_BIT : VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	VkFormat format = texture->Format == TextureFormatColor ? Swapchain.ColorFormat : (VkFormat)texture->Format;
	VkImageViewCreateInfo createInfo =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = texture->Image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = format,
		.subresourceRange =
		{
			.aspectMask = imageAspect,
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

static void CreateSampler(Texture texture, TextureConfigure config)
{
	VkSamplerCreateInfo samplerInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.magFilter = (VkFilter)config.Filter,
		.minFilter = (VkFilter)config.Filter,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		.addressModeU = (VkSamplerAddressMode)config.AddressMode,
		.addressModeV = (VkSamplerAddressMode)config.AddressMode,
		.addressModeW = (VkSamplerAddressMode)config.AddressMode,
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

Texture TextureCreate(TextureConfigure config)
{
	Texture texture = malloc(sizeof(struct Texture));
	*texture = (struct Texture)
	{
		.Width = config.LoadFromData ? config.Data.Width : config.Width,
		.Height = config.LoadFromData ? config.Data.Height : config.Height,
		.Format = config.Format,
	};
	
	CreateImage(texture);
	TransitionImageLayout(texture);
	if (config.LoadFromData) { CopyImageData(texture, config); }
	CreateImageView(texture);
	CreateSampler(texture, config);
	
	return texture;
}

void TextureQueueDestroy(Texture texture)
{
	ListPush(Graphics.FrameResources[Graphics.FrameIndex].DestroyTextureQueue, texture);
}

void TextureDestroy(Texture texture)
{
	vkDestroySampler(Graphics.Device, texture->Sampler, NULL);
	vkDestroyImageView(Graphics.Device, texture->ImageView, NULL);
	vmaDestroyImage(Graphics.Allocator, texture->Image, texture->Allocation);
	free(texture);
}
