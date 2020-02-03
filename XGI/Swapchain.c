#include <stdio.h>
#include <stdlib.h>
#include "Swapchain.h"
#include "Graphics.h"
#include "LinearMath.h"
#include "Window.h"

struct Swapchain Swapchain = { 0 };

static void CreateSwapchain(int width, int height)
{
	printf("\n[Log] Creating swapchain...\n");
	VkSurfaceCapabilitiesKHR availableCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Graphics.PhysicalDevice, Graphics.Surface, &availableCapabilities);
	
	unsigned int availableFormatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(Graphics.PhysicalDevice, Graphics.Surface, &availableFormatCount, NULL);
	VkSurfaceFormatKHR * availableFormats = (VkSurfaceFormatKHR * )malloc(availableFormatCount * sizeof(VkSurfaceFormatKHR));
	vkGetPhysicalDeviceSurfaceFormatsKHR(Graphics.PhysicalDevice, Graphics.Surface, &availableFormatCount, availableFormats);
	VkSurfaceFormatKHR surfaceFormat = availableFormats[0];
	VkFormat targetFormat = VK_FORMAT_B8G8R8A8_UNORM;
	printf("\tNumber of available formats: %i\n", availableFormatCount);
	for (int i = 0; i < availableFormatCount; i++)
	{
		printf("%i %i\n", availableFormats[i].format, targetFormat);
		if (availableFormats[i].format == targetFormat)
		{
			surfaceFormat = availableFormats[i];
		}
	}
	free(availableFormats);
	
	unsigned int availablePresentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(Graphics.PhysicalDevice, Graphics.Surface, &availablePresentModeCount, NULL);
	VkPresentModeKHR * availablePresentModes = (VkPresentModeKHR * )malloc(availablePresentModeCount * sizeof(VkPresentModeKHR));
	vkGetPhysicalDeviceSurfacePresentModesKHR(Graphics.PhysicalDevice, Graphics.Surface, &availablePresentModeCount, availablePresentModes);
	VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
	VkPresentModeKHR targetPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
	//VkPresentModeKHR targetPresentMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
	printf("\tNumber of available present modes: %i\n", availablePresentModeCount);
	for (int i = 0; i < availablePresentModeCount; i++)
	{
		if (availablePresentModes[i] == targetPresentMode)
		{
			presentMode = availablePresentModes[i];
		}
	}
	free(availablePresentModes);
	
	VkExtent2D extent =
	{
		.width = (unsigned int)width,
		.height = (unsigned int)height,
	};
	extent.width = MAX(availableCapabilities.minImageExtent.width, MIN(availableCapabilities.maxImageExtent.width, extent.width));
	extent.height = MAX(availableCapabilities.minImageExtent.height, MIN(availableCapabilities.maxImageExtent.height, extent.height));
	
	printf("\tMinimum image count: %i\n", availableCapabilities.minImageCount);
	printf("\tMaximum image count: %i\n", availableCapabilities.maxImageCount);
	uint32_t imageCount = availableCapabilities.minImageCount;
	if (availableCapabilities.maxImageCount > availableCapabilities.minImageCount || availableCapabilities.maxImageCount == 0)
	{
		imageCount++;
	}

	VkSwapchainCreateInfoKHR createInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = Graphics.Surface,
		.minImageCount = imageCount,
		.imageFormat = surfaceFormat.format,
		.imageColorSpace = surfaceFormat.colorSpace,
		.imageExtent = extent,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		.preTransform = availableCapabilities.currentTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = presentMode,
		.clipped = VK_TRUE,
		.oldSwapchain = VK_NULL_HANDLE,
	};
	
	const uint32_t queueFamilyIndices[] = { Graphics.GraphicsQueueIndex, Graphics.PresentQueueIndex };
	if (Graphics.GraphicsQueueIndex != Graphics.PresentQueueIndex)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
		createInfo.queueFamilyIndexCount = 2;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}
	
	VkResult result = vkCreateSwapchainKHR(Graphics.Device, &createInfo, NULL, &Swapchain.Instance);
	if (result != VK_SUCCESS)
	{
		printf("[Error] Failed to create swapchain: %i\n", result);
		exit(-1);
	}

	Swapchain.Extent = extent;
	Swapchain.ColorFormat = surfaceFormat.format;
	Swapchain.DepthFormat = VK_FORMAT_D32_SFLOAT;
	Swapchain.StencilFormat = VK_FORMAT_S8_UINT;
	Window.Width = Swapchain.Extent.width;
	Window.Height = Swapchain.Extent.height;
}

static void GetSwapchainImages()
{
	vkGetSwapchainImagesKHR(Graphics.Device, Swapchain.Instance, &Swapchain.ImageCount, NULL);
	Swapchain.Images = malloc(Swapchain.ImageCount * sizeof(VkImage));
	vkGetSwapchainImagesKHR(Graphics.Device, Swapchain.Instance, &Swapchain.ImageCount, Swapchain.Images);
	
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
	for (int i = 0; i < Swapchain.ImageCount; i++)
	{
		VkImageMemoryBarrier memoryBarrier =
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = Swapchain.Images[i],
			.subresourceRange =
			{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
			.srcAccessMask = 0,
			.dstAccessMask = 0,
		};
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, 1, &memoryBarrier);
	}
	vkEndCommandBuffer(commandBuffer);
	
	VkSubmitInfo submitInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers = &commandBuffer,
	};
	vkQueueSubmit(Graphics.GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkDeviceWaitIdle(Graphics.Device);
	
	vkFreeCommandBuffers(Graphics.Device, Graphics.CommandPool, 1, &commandBuffer);
}

static void CreateRenderPass()
{
	VkAttachmentDescription colorAttachment =
	{
		.format = Swapchain.ColorFormat,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_GENERAL,
		.finalLayout = VK_IMAGE_LAYOUT_GENERAL,
	};
	VkAttachmentReference colorAttachmentReference =
	{
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	};
	
	VkAttachmentDescription depthAttachment =
	{
		.format = Swapchain.DepthFormat,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_GENERAL,
		.finalLayout = VK_IMAGE_LAYOUT_GENERAL,
	};
	VkAttachmentReference depthAttachmentReference =
	{
		.attachment = 1,
		.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	};
	
	VkSubpassDescription subpass =
	{
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments = &colorAttachmentReference,
		.pDepthStencilAttachment = &depthAttachmentReference,
	};
	
	VkAttachmentDescription attachments[] = { colorAttachment, depthAttachment };
	VkRenderPassCreateInfo renderPassInfo =
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 2,
		.pAttachments = attachments,
		.subpassCount = 1,
		.pSubpasses = &subpass,
		.dependencyCount = 0,
		.pDependencies = NULL,
	};
	VkResult result = vkCreateRenderPass(Graphics.Device, &renderPassInfo, NULL, &Swapchain.RenderPass);
	if (result != VK_SUCCESS)
	{
		printf("[Error] Failed to create render pass: %i\n", result);
		exit(-1);
	}
}

void SwapchainCreate(int width, int height)
{
	CreateSwapchain(width, height);
	GetSwapchainImages();
	CreateRenderPass();
	printf("\n[Log] Swapchain created successfully\n");
}

void SwapchainAquireNextImage()
{
	Graphics.FrameIndex = (Graphics.FrameIndex + 1) % Graphics.FrameResourceCount;
	unsigned int i = Graphics.FrameIndex;
	vkWaitForFences(Graphics.Device, 1, &Graphics.FrameResources[i].FrameReady, VK_TRUE, UINT64_MAX);
	vkResetFences(Graphics.Device, 1, &Graphics.FrameResources[i].FrameReady);
	
	for (int j = 0; j < Graphics.FrameResources[i].DestroyVertexBufferQueue->Count; j++)
	{
		VertexBuffer vertexBuffer = ListIndex(Graphics.FrameResources[i].DestroyVertexBufferQueue, j);
		VertexBufferDestroy(vertexBuffer);
	}
	ListClear(Graphics.FrameResources[i].DestroyVertexBufferQueue);
	for (int j = 0; j < Graphics.FrameResources[i].UpdateDescriptorQueue->Count; j++)
	{
		VkWriteDescriptorSet * writeInfo = ListIndex(Graphics.FrameResources[i].UpdateDescriptorQueue, j);
		vkUpdateDescriptorSets(Graphics.Device, 1, writeInfo, 0, NULL);
		free((void *)writeInfo->pBufferInfo);
		free(writeInfo);
	}
	ListClear(Graphics.FrameResources[i].UpdateDescriptorQueue);
	
	vkAcquireNextImageKHR(Graphics.Device, Swapchain.Instance, UINT64_MAX, Graphics.FrameResources[i].ImageAvailable, VK_NULL_HANDLE, &Swapchain.CurrentImageIndex);
	
	vkResetCommandBuffer(Graphics.FrameResources[i].CommandBuffer, 0);
	VkCommandBufferBeginInfo beginInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
	};
	VkResult result = vkBeginCommandBuffer(Graphics.FrameResources[i].CommandBuffer, &beginInfo);
	if (result != VK_SUCCESS)
	{
		printf("[Error] Failed to begin command buffer: %i\n", result);
		exit(-1);
	}
}

void SwapchainPresent()
{
	unsigned int i = Graphics.FrameIndex;
	
	VkResult result = vkEndCommandBuffer(Graphics.FrameResources[i].CommandBuffer);
	if (result != VK_SUCCESS)
	{
		printf("[Error] Failed to record command buffer: %i\n", result);
		exit(-1);
	}

	VkSemaphore * waitSemaphores = malloc((1 + Graphics.PreRenderSemaphores->Count) * sizeof(VkSemaphore));
	VkPipelineStageFlags * waitStages = malloc((1 + Graphics.PreRenderSemaphores->Count) * sizeof(VkPipelineStageFlags));
	waitSemaphores[0] = Graphics.FrameResources[i].ImageAvailable;
	waitStages[0] = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	for (int i = 0; i < Graphics.PreRenderSemaphores->Count; i++)
	{
		waitSemaphores[i + 1] = *(VkSemaphore *)ListIndex(Graphics.PreRenderSemaphores, i);
		waitStages[i + 1] = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	}
	
	ListClear(Graphics.PreRenderSemaphores);
	VkSubmitInfo submitInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount = 1 + Graphics.PreRenderSemaphores->Count,
		.pWaitSemaphores = waitSemaphores,
		.pWaitDstStageMask = waitStages,
		.commandBufferCount = 1,
		.pCommandBuffers = &Graphics.FrameResources[i].CommandBuffer,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &Graphics.FrameResources[i].RenderFinished,
	};
	result = vkQueueSubmit(Graphics.GraphicsQueue, 1, &submitInfo, Graphics.FrameResources[i].FrameReady);
	if (result != VK_SUCCESS)
	{
		printf("[Error] Failed to submit queue: %i\n", result);
		exit(-1);
	}
	free(waitSemaphores);
	free(waitStages);
	
	VkPresentInfoKHR presentInfo =
	{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &Graphics.FrameResources[i].RenderFinished,
		.swapchainCount = 1,
		.pSwapchains = &Swapchain.Instance,
		.pImageIndices = &Swapchain.CurrentImageIndex,
	};
	vkQueuePresentKHR(Graphics.PresentQueue, &presentInfo);
}

void SwapchainDestroy()
{
	vkDeviceWaitIdle(Graphics.Device);
	vkDestroyRenderPass(Graphics.Device, Swapchain.RenderPass, NULL);
	free(Swapchain.Images);
	vkDestroySwapchainKHR(Graphics.Device, Swapchain.Instance, NULL);
}
