#include <SDL2/SDL_vulkan.h>
#include <string.h>

#include "Graphics.h"
#include "Swapchain.h"
#include "Window.h"
#include "VertexBuffer.h"
#include "LinearMath.h"

struct Graphics Graphics = { 0 };

static bool CheckValidationLayerSupport()
{
	printf("\n[Log] Checking validation layer support...\n");
	unsigned int availableLayerCount;
	vkEnumerateInstanceLayerProperties(&availableLayerCount, NULL);
	VkLayerProperties * availableLayers = malloc(availableLayerCount * sizeof(VkLayerProperties));
	vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers);

	bool supported = false;
	printf("\tSupported layers:\n");
	for (int i = 0; i < availableLayerCount; i++)
	{
		printf("\t\t%s\n", availableLayers[i].layerName);
		if (strcmp(availableLayers[i].layerName, "VK_LAYER_KHRONOS_validation") == 0)
		{
			supported = true;
		}
	}
	supported ? printf("[Log] Validation layer is supported\n") : printf("[Warning] Validation layer is not supported\n");
	free(availableLayers);
	return supported;
}

static void CheckExtensionSupport()
{
	printf("\n[Log] Checking extension support...\n");
	unsigned int supportedExtensionCount;
	vkEnumerateInstanceExtensionProperties(NULL, &supportedExtensionCount, NULL);
	VkExtensionProperties * supportedExtensions = malloc(supportedExtensionCount * sizeof(VkExtensionProperties));
	vkEnumerateInstanceExtensionProperties(NULL, &supportedExtensionCount, supportedExtensions);
	printf("\tSupported Extensions:\n");
	for (int i = 0; i < supportedExtensionCount; i++)
	{
		printf("\t\t%s %i\n", supportedExtensions[i].extensionName, supportedExtensions[i].specVersion);
	}
	
	unsigned int requiredExtensionCount;
	SDL_Vulkan_GetInstanceExtensions(Window.Handle, &requiredExtensionCount, NULL);
	char ** requiredExtensionNames = (char ** )malloc(requiredExtensionCount * sizeof(char * ));
	SDL_Vulkan_GetInstanceExtensions(Window.Handle, &requiredExtensionCount, (const char ** )requiredExtensionNames);
	printf("\tRequired Extensions:\n");
	for (int i = 0; i < requiredExtensionCount; i++)
	{
		printf("\t\t%s\n", requiredExtensionNames[i]);
	}
	
	bool supported = true;
	for (int i = 0; i < requiredExtensionCount; i++)
	{
		bool extensionSupported = false;
		for (int j = 0; j < supportedExtensionCount; j++)
		{
			if (strcmp(supportedExtensions[j].extensionName, requiredExtensionNames[i]) == 0)
			{
				extensionSupported = true;
				break;
			}
		}
		if (!extensionSupported)
		{
			printf("\t\t%s\n", requiredExtensionNames[i]);
			supported = false;
			break;
		}
	}
	if (!supported)
	{
		printf("[Error] Platform does not support all required extensions\n");
		exit(-1);
	}
	else
	{
		printf("[Log] All extensions supported\n");
	}
	free(supportedExtensions);
	free(requiredExtensionNames);
}

static void CreateInstance(bool vulkanValidation)
{
	bool useValidations = true && CheckValidationLayerSupport();
	const char * validationLayer = "VK_LAYER_KHRONOS_validation";

	printf("\n[Log] Creating Vulkan instance...\n");
	
	VkApplicationInfo appInfo =
	{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = Window.Title,
		.applicationVersion = VK_MAKE_VERSION(0, 0, 0),
		.pEngineName = "XGI",
		.engineVersion = VK_MAKE_VERSION(0, 0, 0),
		.apiVersion = VK_API_VERSION_1_0,
	};
	
	unsigned int extensionCount;
	SDL_Vulkan_GetInstanceExtensions(Window.Handle, &extensionCount, NULL);
	char ** extensionNames = (char ** )malloc(extensionCount * sizeof(char * ));
	SDL_Vulkan_GetInstanceExtensions(Window.Handle, &extensionCount, (const char ** )extensionNames);
	
	VkInstanceCreateInfo createInfo =
	{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &appInfo,
		.enabledExtensionCount = extensionCount,
		.ppEnabledExtensionNames = (const char * const *)extensionNames,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = NULL,
	};
	if (useValidations)
	{
		createInfo.enabledLayerCount = 1;
		createInfo.ppEnabledLayerNames = &validationLayer;
	}
	
	VkResult result = vkCreateInstance(&createInfo, NULL, &Graphics.Instance);
	if (result != VK_SUCCESS) { printf("[Error] Failed to create Vulkan instance: %i\n", result); exit(-1); }
	free(extensionNames);
}

static void CreateSurface()
{
	printf("\n[Log] Creating SurfaceKHR...\n");
	if (!SDL_Vulkan_CreateSurface(Window.Handle, Graphics.Instance, &Graphics.Surface))
	{
		printf("[Error] Failed to create SurfaceKHR\n");
		exit(-1);
	}
}

static void ChoosePhysicalDevice(bool useIntegrated)
{
	printf("\n[Log] Choosing physical device...\n");
	Graphics.PhysicalDevice = VK_NULL_HANDLE;
	
	unsigned int deviceCount = 0;
	vkEnumeratePhysicalDevices(Graphics.Instance, &deviceCount, NULL);
	if (deviceCount == 0) { printf("[Error] No GPU with Vulkan support\n"); exit(-1); }
	VkPhysicalDevice * devices = (VkPhysicalDevice * )malloc(deviceCount * sizeof(VkPhysicalDevice));
	vkEnumeratePhysicalDevices(Graphics.Instance, &deviceCount, devices);
	
	Graphics.PhysicalDevice = devices[0];
	int graphicsQueueIndex = -1;
	int presentQueueIndex = -1;
	bool suitableDevice = false;
	printf("\tPhysical Devices:\n");
	for (int i = 0; i < deviceCount; i++)
	{
		
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
		printf("\t\t%s\n", deviceProperties.deviceName);
		
		unsigned int queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueFamilyCount, NULL);
		VkQueueFamilyProperties * queueFamilies = malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
		vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueFamilyCount, queueFamilies);
		for (int j = 0; j < queueFamilyCount; j++)
		{
			if (queueFamilies[j].queueCount > 0 && queueFamilies[j].queueFlags & VK_QUEUE_GRAPHICS_BIT && graphicsQueueIndex == -1)
			{
				printf("Graphics %i\n", j);
				graphicsQueueIndex = j;
			}
			VkBool32 presentSupported = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(devices[i], j, Graphics.Surface, &presentSupported);
			if (queueFamilies[j].queueCount > 0 && presentSupported && presentQueueIndex == -1)
			{
				printf("Present %i\n", j);
				presentQueueIndex = j;
			}
		}
		free(queueFamilies);
		
		bool graphicsQueueSupported = graphicsQueueIndex > -1;
		bool presentQueueSupported = presentQueueIndex > -1;
		bool swapchainSupported = false;
		
		unsigned int availableExtensionCount;
		vkEnumerateDeviceExtensionProperties(devices[i], NULL, &availableExtensionCount, NULL);
		VkExtensionProperties * availableExtensions = malloc(availableExtensionCount * sizeof(VkExtensionProperties));
		vkEnumerateDeviceExtensionProperties(devices[i], NULL, &availableExtensionCount, availableExtensions);
		printf("\t\tAvailable Extensions:\n");
		for (int i = 0; i < availableExtensionCount; i++)
		{
			printf("\t\t\t%s\n", availableExtensions[i].extensionName);
			if (strcmp(availableExtensions[i].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
			{
				swapchainSupported = true;
			}
		}
		free(availableExtensions);
		
		if (graphicsQueueSupported && presentQueueSupported && swapchainSupported)
		{
			suitableDevice = true;
			Graphics.GraphicsQueueIndex = graphicsQueueIndex;
			Graphics.PresentQueueIndex = presentQueueIndex;
			Graphics.PhysicalDevice = devices[i];
			if (!useIntegrated && deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				break;
			}
			if (useIntegrated && deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
			{
				break;
			}
		}
	}
	if (!suitableDevice)
	{
		printf("[Error] No suitable device\n");
		exit(-1);
	}
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(Graphics.PhysicalDevice, &deviceProperties);
	printf("[Log] Chose device: %s\n", deviceProperties.deviceName);
	free(devices);
}

static void CreateLogicalDevice()
{
	printf("\n[Log] Creating logical device...\n");
	
	float queuePriority = 1.0f;
	VkDeviceQueueCreateInfo graphicsQueueInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = Graphics.GraphicsQueueIndex,
		.queueCount = 1,
		.pQueuePriorities = &queuePriority,
	};
	VkDeviceQueueCreateInfo presentQueueInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = Graphics.PresentQueueIndex,
		.queueCount = 1,
		.pQueuePriorities = &queuePriority,
	};
	VkDeviceQueueCreateInfo queues[2] = { graphicsQueueInfo, presentQueueInfo };
	
	VkPhysicalDeviceFeatures deviceFeatures = { 0 };
	
	const char * extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	
	int queueCount = Graphics.GraphicsQueueIndex == Graphics.PresentQueueIndex ? 1 : 2;
	VkDeviceCreateInfo _DeviceInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = queueCount,
		.pQueueCreateInfos = queues,
		.pEnabledFeatures = &deviceFeatures,
		.enabledExtensionCount = 1,
		.ppEnabledExtensionNames = extensions,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = NULL,
	};
	
	VkResult result = vkCreateDevice(Graphics.PhysicalDevice, &_DeviceInfo, NULL, &Graphics.Device);
	if (result != VK_SUCCESS)
	{
		printf("[Error] Failed to create logical device: %i\n", result);
		exit(-1);
	}
	
	vkGetDeviceQueue(Graphics.Device, Graphics.GraphicsQueueIndex, 0, &Graphics.GraphicsQueue);
	vkGetDeviceQueue(Graphics.Device, Graphics.PresentQueueIndex, 0, &Graphics.PresentQueue);
}

static void CreateCommandPool()
{
	VkCommandPoolCreateInfo createInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = Graphics.GraphicsQueueIndex,
	};
	
	VkResult result = vkCreateCommandPool(Graphics.Device, &createInfo, NULL, &Graphics.CommandPool);
	if (result != VK_SUCCESS)
	{
		printf("[Error] Failed to create command pool: %i\n", result);
		exit(-1);
	}
}

static void CreateAllocator()
{
	VmaAllocatorCreateInfo allocatorInfo =
	{
		.physicalDevice = Graphics.PhysicalDevice,
		.device = Graphics.Device,
		.preferredLargeHeapBlockSize = 128 * 1024 * 1024,
		.frameInUseCount = 1,
	};
	vmaCreateAllocator(&allocatorInfo, &Graphics.Allocator);
}

static void CreateDescriptorPool()
{
	VkDescriptorPoolSize poolSizes[] =
	{
		{
			.descriptorCount = 128 * Graphics.FrameResourceCount,
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		},
		{
			.descriptorCount = 16 * Graphics.FrameResourceCount,
			.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		}
	};
	VkDescriptorPoolCreateInfo poolInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
		.maxSets = 128 * Graphics.FrameResourceCount,
		.poolSizeCount = 2,
		.pPoolSizes = poolSizes,
	};
	VkResult result = vkCreateDescriptorPool(Graphics.Device, &poolInfo, NULL, &Graphics.DescriptorPool);
	if (result != VK_SUCCESS)
	{
		printf("[Error] failed to create descriptor pool: %i\n", result);
		exit(-1);
	}
	
}

static void CreateFrameResources()
{
	Graphics.FrameResources = malloc(Graphics.FrameResourceCount * sizeof(*Graphics.FrameResources));
	for (int i = 0; i < Graphics.FrameResourceCount; i++)
	{
		VkCommandBufferAllocateInfo allocateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandPool = Graphics.CommandPool,
			.commandBufferCount = 1,
		};
		vkAllocateCommandBuffers(Graphics.Device, &allocateInfo, &Graphics.FrameResources[i].CommandBuffer);
		
		VkSemaphoreCreateInfo semaphoreInfo =
		{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		};
		vkCreateSemaphore(Graphics.Device, &semaphoreInfo, NULL, &Graphics.FrameResources[i].ImageAvailable);
		vkCreateSemaphore(Graphics.Device, &semaphoreInfo, NULL, &Graphics.FrameResources[i].RenderFinished);
		
		VkFenceCreateInfo fenceInfo =
		{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT,
		};
		vkCreateFence(Graphics.Device, &fenceInfo, NULL, &Graphics.FrameResources[i].FrameReady);
		
		Graphics.FrameResources[i].DestroyVertexBufferQueue = ListCreate();
		Graphics.FrameResources[i].UpdateDescriptorQueue = ListCreate();
	}
	Graphics.FrameIndex = 0;
	Graphics.PreRenderSemaphores = ListCreate();
}

void GraphicsInitialize(GraphicsConfigure config)
{
	printf("\n[Log] Initializing Graphics...\n");
	Graphics.FrameResourceCount = config.FrameResourceCount;
	CheckExtensionSupport();
	CreateInstance(config.VulkanValidation);
	CreateSurface();
	ChoosePhysicalDevice(config.TargetIntegratedDevice);
	CreateLogicalDevice();
	CreateCommandPool();
	CreateAllocator();
	CreateDescriptorPool();
	CreateFrameResources();
	printf("\n[Log] Successfully initialized vulkan\n");
}

void GraphicsBegin(FrameBuffer frameBuffer)
{
	Graphics.BoundFrameBuffer = frameBuffer;
	
	VkRenderPassBeginInfo renderPassBegin =
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = Swapchain.RenderPass,
		.framebuffer = Graphics.BoundFrameBuffer->Instance,
		.renderArea = (VkRect2D)
		{
			.offset = { 0, 0 },
			.extent = { frameBuffer->Width, frameBuffer->Height },
		},
		.clearValueCount = 0,
		.pClearValues = NULL,
	};
	vkCmdBeginRenderPass(Graphics.FrameResources[Graphics.FrameIndex].CommandBuffer, &renderPassBegin, VK_SUBPASS_CONTENTS_INLINE);
}

void GraphicsClearColor(Color clearColor)
{
	Vector4 color = ColorToVector4(clearColor);
	VkClearRect rect =
	{
		.baseArrayLayer = 0,
		.layerCount = 1,
		.rect =
		{
			.offset = { 0, 0 },
			.extent = { Graphics.BoundFrameBuffer->Width, Graphics.BoundFrameBuffer->Height },
		},
	};
	VkClearAttachment clear =
	{
		.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.colorAttachment = 0,
		.clearValue = { .color = { color.X, color.Y, color.Z, color.W } },
	};
	vkCmdClearAttachments(Graphics.FrameResources[Graphics.FrameIndex].CommandBuffer, 1, &clear, 1, &rect);
}

void GraphicsClearDepth(float depth)
{
	VkClearRect rect =
	{
		.baseArrayLayer = 0,
		.layerCount = 1,
		.rect =
		{
			.offset = { 0, 0 },
			.extent = { Graphics.BoundFrameBuffer->Width, Graphics.BoundFrameBuffer->Height },
		},
	};
	VkClearAttachment clear =
	{
		.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
		.clearValue = { .depthStencil = { .depth = depth } },
	};
	vkCmdClearAttachments(Graphics.FrameResources[Graphics.FrameIndex].CommandBuffer, 1, &clear, 1, &rect);
}

void GraphicsClearStencil(int stencil)
{
	VkClearRect rect =
	{
		.baseArrayLayer = 0,
		.layerCount = 1,
		.rect =
		{
			.offset = { 0, 0 },
			.extent = { Graphics.BoundFrameBuffer->Width, Graphics.BoundFrameBuffer->Height },
		},
	};
	VkClearAttachment clear =
	{
		.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT,
		.clearValue = { .depthStencil = { .stencil = stencil } },
	};
	vkCmdClearAttachments(Graphics.FrameResources[Graphics.FrameIndex].CommandBuffer, 1, &clear, 1, &rect);
}

void GraphicsClear(Color clearColor, float depth, int stencil)
{
	GraphicsClearColor(clearColor);
	GraphicsClearDepth(depth);
	GraphicsClearStencil(stencil);
}

void GraphicsBindPipeline(Pipeline pipeline)
{
	Graphics.BoundPipeline = pipeline;
	vkCmdBindPipeline(Graphics.FrameResources[Graphics.FrameIndex].CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->Instance);
	VkViewport viewport =
	{
		.x = 0.0f,
		.y = 0.0f,
		.width = Swapchain.Extent.width,
		.height = Swapchain.Extent.height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};
	VkRect2D scissor =
	{
		.offset = { 0, 0 },
		.extent = Swapchain.Extent,
	};
	vkCmdSetViewport(Graphics.FrameResources[Graphics.FrameIndex].CommandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(Graphics.FrameResources[Graphics.FrameIndex].CommandBuffer, 0, 1, &scissor);
	
	if (pipeline->UsesPushConstant)
	{
		vkCmdPushConstants(Graphics.FrameResources[Graphics.FrameIndex].CommandBuffer, pipeline->Layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, pipeline->PushConstantSize, pipeline->PushConstantData);
	}
	if (pipeline->UsesDescriptors)
	{
		vkCmdBindDescriptorSets(Graphics.FrameResources[Graphics.FrameIndex].CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->Layout, 0, 1, &pipeline->DescriptorSet[Graphics.FrameIndex], 0, NULL);
	}
}

void GraphicsRenderVertexBuffer(VertexBuffer vertexBuffer)
{	
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(Graphics.FrameResources[Graphics.FrameIndex].CommandBuffer, 0, 1, &vertexBuffer->VertexBuffer, &offset);
	vkCmdDraw(Graphics.FrameResources[Graphics.FrameIndex].CommandBuffer, vertexBuffer->VertexCount, 1, 0, 0);
}

void GraphicsEnd()
{
	vkCmdEndRenderPass(Graphics.FrameResources[Graphics.FrameIndex].CommandBuffer);
	Graphics.BoundFrameBuffer = NULL;
}

void GraphicsCopyToSwapchain(FrameBuffer frameBuffer)
{
	unsigned int i = Graphics.FrameIndex;
	
	VkImageMemoryBarrier memoryBarrier =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = Swapchain.Images[Swapchain.CurrentImageIndex],
		.subresourceRange =
		{
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
	};
	vkCmdPipelineBarrier(Graphics.FrameResources[i].CommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &memoryBarrier);

	VkImageCopy copyInfo =
	{
		.srcSubresource =
		{
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.mipLevel = 0,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
		.srcOffset = { .x = 0, .y = 0, .z = 0, },
		.dstSubresource =
		{
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.mipLevel = 0,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
		.dstOffset = { .x = 0, .y = 0, .z = 0, },
		.extent =
		{
			.width = MIN(Swapchain.Extent.width, frameBuffer->Width),
			.height = MIN(Swapchain.Extent.height, frameBuffer->Height),
			.depth = 1,
		},
	};
	vkCmdCopyImage(Graphics.FrameResources[i].CommandBuffer, frameBuffer->ColorTexture->Image, VK_IMAGE_LAYOUT_GENERAL, Swapchain.Images[Swapchain.CurrentImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyInfo);
	
	memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	memoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	vkCmdPipelineBarrier(Graphics.FrameResources[i].CommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, 1, &memoryBarrier);
}

void GraphicsStopOperations()
{
	vkDeviceWaitIdle(Graphics.Device);
}

void GraphicsDeinitialize()
{
	vkDeviceWaitIdle(Graphics.Device);
	ListDestroy(Graphics.PreRenderSemaphores);
	for (int i = 0; i < Graphics.FrameResourceCount; i++)
	{
		for (int j = 0; j < Graphics.FrameResources[i].DestroyVertexBufferQueue->Count; j++)
		{
			VertexBuffer vertexBuffer = ListIndex(Graphics.FrameResources[i].DestroyVertexBufferQueue, j);
			VertexBufferDestroy(vertexBuffer);
		}
		ListDestroy(Graphics.FrameResources[i].DestroyVertexBufferQueue);
		ListDestroy(Graphics.FrameResources[i].UpdateDescriptorQueue);
		vkDestroyFence(Graphics.Device, Graphics.FrameResources[i].FrameReady, NULL);
		vkDestroySemaphore(Graphics.Device, Graphics.FrameResources[i].RenderFinished, NULL);
		vkDestroySemaphore(Graphics.Device, Graphics.FrameResources[i].ImageAvailable, NULL);
		vkFreeCommandBuffers(Graphics.Device, Graphics.CommandPool, 1, &Graphics.FrameResources[i].CommandBuffer);
	}
	free(Graphics.FrameResources);
	vkDestroyDescriptorPool(Graphics.Device, Graphics.DescriptorPool, NULL);
	vmaDestroyAllocator(Graphics.Allocator);
	vkDestroyCommandPool(Graphics.Device, Graphics.CommandPool, NULL);
	vkDestroyDevice(Graphics.Device, NULL);
	vkDestroySurfaceKHR(Graphics.Instance, Graphics.Surface, NULL);
	vkDestroyInstance(Graphics.Instance, NULL);
}
