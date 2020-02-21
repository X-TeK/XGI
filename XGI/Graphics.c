#include <SDL2/SDL_vulkan.h>
#include <string.h>
#include <stdio.h>
#include "log.h"
#include "Graphics.h"
#include "Window.h"
#include "VertexBuffer.h"
#include "LinearMath.h"

struct Graphics Graphics = { 0 };

static bool CheckValidationLayerSupport()
{
	unsigned int availableLayerCount;
	vkEnumerateInstanceLayerProperties(&availableLayerCount, NULL);
	VkLayerProperties * availableLayers = malloc(availableLayerCount * sizeof(VkLayerProperties));
	vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers);

	bool supported = false;
	for (int i = 0; i < availableLayerCount; i++)
	{
		if (strcmp(availableLayers[i].layerName, "VK_LAYER_KHRONOS_validation") == 0)
		{
			supported = true;
		}
	}
	if (!supported) { log_warn("Validation layers is not supported\n"); };
	free(availableLayers);
	return supported;
}

static void CheckExtensionSupport()
{
	unsigned int supportedExtensionCount;
	vkEnumerateInstanceExtensionProperties(NULL, &supportedExtensionCount, NULL);
	VkExtensionProperties * supportedExtensions = malloc(supportedExtensionCount * sizeof(VkExtensionProperties));
	vkEnumerateInstanceExtensionProperties(NULL, &supportedExtensionCount, supportedExtensions);
	
	unsigned int requiredExtensionCount;
	SDL_Vulkan_GetInstanceExtensions(Window.Handle, &requiredExtensionCount, NULL);
	char ** requiredExtensionNames = (char ** )malloc(requiredExtensionCount * sizeof(char * ));
	SDL_Vulkan_GetInstanceExtensions(Window.Handle, &requiredExtensionCount, (const char ** )requiredExtensionNames);
	
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
			log_fatal("Required extension is not supported: %s\n", requiredExtensionNames[i]);
			exit(1);
		}
	}
	free(supportedExtensions);
	free(requiredExtensionNames);
}

static void CreateInstance(bool vulkanValidation)
{
	bool useValidations = true && CheckValidationLayerSupport();
	const char * validationLayer = "VK_LAYER_KHRONOS_validation";
	
	VkApplicationInfo appInfo =
	{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = Window.Title,
		.applicationVersion = VK_MAKE_VERSION(0, 0, 0),
		.pEngineName = "XGI",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
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
	if (result != VK_SUCCESS) { log_fatal("Failed to create Vulkan instance: %i\n", result); exit(1); }
	free(extensionNames);
}

static void CreateSurface()
{
	if (!SDL_Vulkan_CreateSurface(Window.Handle, Graphics.Instance, &Graphics.Surface))
	{
		log_fatal("Failed to create SurfaceKHR\n");
		exit(1);
	}
}

static void ChoosePhysicalDevice(bool useIntegrated)
{
	Graphics.PhysicalDevice = VK_NULL_HANDLE;
	
	unsigned int deviceCount = 0;
	vkEnumeratePhysicalDevices(Graphics.Instance, &deviceCount, NULL);
	if (deviceCount == 0) { log_fatal("No GPU with Vulkan support\n"); exit(1); }
	VkPhysicalDevice * devices = (VkPhysicalDevice * )malloc(deviceCount * sizeof(VkPhysicalDevice));
	vkEnumeratePhysicalDevices(Graphics.Instance, &deviceCount, devices);
	
	Graphics.PhysicalDevice = devices[0];
	
	struct
	{
		bool GraphicsFound;
		int Graphics;
		bool PresentFound;
		int Present;
		bool ComputeFound;
		int Compute;
	} queues = { .GraphicsFound = false, .PresentFound = false, .ComputeFound = false };
	bool suitableDevice = false;
	for (int i = 0; i < deviceCount; i++)
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
		
		unsigned int queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueFamilyCount, NULL);
		VkQueueFamilyProperties * queueFamilies = malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
		vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueFamilyCount, queueFamilies);
		for (int j = 0; j < queueFamilyCount; j++)
		{
			if (queueFamilies[j].queueCount > 0 && queueFamilies[j].queueFlags & VK_QUEUE_GRAPHICS_BIT && !queues.GraphicsFound)
			{
				queues.GraphicsFound = true;
				queues.Graphics = j;
			}
			if (queueFamilies[j].queueCount > 0 && queueFamilies[j].queueFlags & VK_QUEUE_COMPUTE_BIT && !queues.ComputeFound)
			{
				queues.ComputeFound = true;
				queues.Compute = j;
			}
			VkBool32 presentSupported = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(devices[i], j, Graphics.Surface, &presentSupported);
			if (queueFamilies[j].queueCount > 0 && presentSupported && !queues.PresentFound)
			{
				queues.PresentFound = true;
				queues.Present = j;
			}
		}
		free(queueFamilies);
		
		bool swapchainSupported = false;
		unsigned int availableExtensionCount;
		vkEnumerateDeviceExtensionProperties(devices[i], NULL, &availableExtensionCount, NULL);
		VkExtensionProperties * availableExtensions = malloc(availableExtensionCount * sizeof(VkExtensionProperties));
		vkEnumerateDeviceExtensionProperties(devices[i], NULL, &availableExtensionCount, availableExtensions);
		for (int i = 0; i < availableExtensionCount; i++)
		{
			if (strcmp(availableExtensions[i].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
			{
				swapchainSupported = true;
			}
		}
		free(availableExtensions);
		
		Graphics.ComputeQueueSupported = false;
		if (queues.ComputeFound)
		{
			Graphics.ComputeQueueSupported = true;
			Graphics.ComputeQueueIndex = queues.Compute;
		}
		if (queues.GraphicsFound && queues.PresentFound && swapchainSupported)
		{
			suitableDevice = true;
			Graphics.GraphicsQueueIndex = queues.Graphics;
			Graphics.PresentQueueIndex = queues.Present;
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
		log_error("No suitable device found for Vulkan\n");
		exit(1);
	}
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(Graphics.PhysicalDevice, &deviceProperties);
	log_info("Using graphics device: %s\n", deviceProperties.deviceName);
	free(devices);
}

static void CreateLogicalDevice()
{
	float queuePriority = 1.0f;
	int queueCount = 0;
	VkDeviceQueueCreateInfo queueInfos[3];
	
	VkDeviceQueueCreateInfo graphicsQueueInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = Graphics.GraphicsQueueIndex,
		.queueCount = 1,
		.pQueuePriorities = &queuePriority,
	};
	queueInfos[queueCount] = graphicsQueueInfo;
	queueCount++;
	
	VkDeviceQueueCreateInfo presentQueueInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = Graphics.PresentQueueIndex,
		.queueCount = 1,
		.pQueuePriorities = &queuePriority,
	};
	if (Graphics.PresentQueueIndex != Graphics.GraphicsQueueIndex)
	{
		queueInfos[queueCount] = presentQueueInfo;
		queueCount++;
	}
	
	if (Graphics.ComputeQueueSupported)
	{
		VkDeviceQueueCreateInfo computeQueueInfo =
		{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = Graphics.ComputeQueueIndex,
			.queueCount = 1,
			.pQueuePriorities = &queuePriority,
		};
		if (Graphics.ComputeQueueIndex != Graphics.GraphicsQueueIndex && Graphics.ComputeQueueIndex != Graphics.ComputeQueueIndex)
		{
			queueInfos[queueCount] = computeQueueInfo;
			queueCount++;
		}
	}
	
	VkPhysicalDeviceFeatures deviceFeatures =
	{
		.fillModeNonSolid = true,
		.samplerAnisotropy = true,
	};
	const char * extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	
	VkDeviceCreateInfo deviceInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = queueCount,
		.pQueueCreateInfos = queueInfos,
		.pEnabledFeatures = &deviceFeatures,
		.enabledExtensionCount = 1,
		.ppEnabledExtensionNames = extensions,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = NULL,
	};
	
	VkResult result = vkCreateDevice(Graphics.PhysicalDevice, &deviceInfo, NULL, &Graphics.Device);
	if (result != VK_SUCCESS)
	{
		log_fatal("Failed to create logical device: %i\n", result);
		exit(1);
	}
	
	vkGetDeviceQueue(Graphics.Device, Graphics.GraphicsQueueIndex, 0, &Graphics.GraphicsQueue);
	vkGetDeviceQueue(Graphics.Device, Graphics.PresentQueueIndex, 0, &Graphics.PresentQueue);
	if (Graphics.ComputeQueueSupported) { vkGetDeviceQueue(Graphics.Device, Graphics.ComputeQueueIndex, 0, &Graphics.ComputeQueue); }
}

static void CreateSwapchain(int width, int height)
{
	VkSurfaceCapabilitiesKHR availableCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Graphics.PhysicalDevice, Graphics.Surface, &availableCapabilities);
	
	unsigned int availableFormatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(Graphics.PhysicalDevice, Graphics.Surface, &availableFormatCount, NULL);
	VkSurfaceFormatKHR * availableFormats = (VkSurfaceFormatKHR * )malloc(availableFormatCount * sizeof(VkSurfaceFormatKHR));
	vkGetPhysicalDeviceSurfaceFormatsKHR(Graphics.PhysicalDevice, Graphics.Surface, &availableFormatCount, availableFormats);
	VkSurfaceFormatKHR surfaceFormat = availableFormats[0];
	VkFormat targetFormat = VK_FORMAT_B8G8R8A8_UNORM;
	for (int i = 0; i < availableFormatCount; i++)
	{
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
	Graphics.Swapchain.PresentMode = (PresentMode)VK_PRESENT_MODE_FIFO_KHR;
	for (int i = 0; i < availablePresentModeCount; i++)
	{
		if (availablePresentModes[i] == (VkPresentModeKHR)Graphics.Swapchain.TargetPresentMode)
		{
			Graphics.Swapchain.PresentMode = (PresentMode)availablePresentModes[i];
		}
	}
	if (Graphics.Swapchain.PresentMode == VK_PRESENT_MODE_FIFO_KHR && Graphics.Swapchain.TargetPresentMode != VK_PRESENT_MODE_FIFO_KHR)
	{
		log_warn("Target present mode is not supported");
	}
	free(availablePresentModes);
	
	VkExtent2D extent =
	{
		.width = (unsigned int)width,
		.height = (unsigned int)height,
	};
	extent.width = MAX(availableCapabilities.minImageExtent.width, MIN(availableCapabilities.maxImageExtent.width, extent.width));
	extent.height = MAX(availableCapabilities.minImageExtent.height, MIN(availableCapabilities.maxImageExtent.height, extent.height));
	
	unsigned int imageCount = MAX(availableCapabilities.minImageCount, MIN(3, availableCapabilities.maxImageCount));
	
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
		.presentMode = (VkPresentModeKHR)Graphics.Swapchain.PresentMode,
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
	
	VkResult result = vkCreateSwapchainKHR(Graphics.Device, &createInfo, NULL, &Graphics.Swapchain.Instance);
	if (result != VK_SUCCESS)
	{
		log_fatal("Failed to create swapchain: %i\n", result);
		exit(1);
	}

	Graphics.Swapchain.Extent = extent;
	Graphics.Swapchain.ColorFormat = surfaceFormat.format;
	Window.Width = Graphics.Swapchain.Extent.width;
	Window.Height = Graphics.Swapchain.Extent.height;
}

static void GetSwapchainImages()
{
	vkGetSwapchainImagesKHR(Graphics.Device, Graphics.Swapchain.Instance, &Graphics.Swapchain.ImageCount, NULL);
	Graphics.Swapchain.Images = malloc(Graphics.Swapchain.ImageCount * sizeof(VkImage));
	vkGetSwapchainImagesKHR(Graphics.Device, Graphics.Swapchain.Instance, &Graphics.Swapchain.ImageCount, Graphics.Swapchain.Images);
	
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
	for (int i = 0; i < Graphics.Swapchain.ImageCount; i++)
	{
		VkImageMemoryBarrier memoryBarrier =
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = Graphics.Swapchain.Images[i],
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
		.format = (VkFormat)TextureFormatColor,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE,
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
		.format = (VkFormat)TextureFormatDepthStencil,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE,
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
	VkResult result = vkCreateRenderPass(Graphics.Device, &renderPassInfo, NULL, &Graphics.RenderPass);
	if (result != VK_SUCCESS)
	{
		log_fatal("Failed to create render pass: %i\n", result);
		exit(1);
	}
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
		log_fatal("Failed to create command pool: %i\n", result);
		exit(1);
	}
}

static void CreateAllocator()
{
	VmaAllocatorCreateInfo allocatorInfo =
	{
		.physicalDevice = Graphics.PhysicalDevice,
		.device = Graphics.Device,
		.preferredLargeHeapBlockSize = 32 * 1024 * 1024,
		.frameInUseCount = 1,
	};
	VkResult result = vmaCreateAllocator(&allocatorInfo, &Graphics.Allocator);
	if (result != VK_SUCCESS)
	{
		log_fatal("Failed to create memory allocator: %i\n", result);
		exit(1);
	}
}

static void CreateCompiler()
{
	Graphics.ShaderCompiler = shaderc_compiler_initialize();
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
		Graphics.FrameResources[i].DestroyTextureQueue = ListCreate();
		Graphics.FrameResources[i].DestroyPipelineQueue = ListCreate();
		Graphics.FrameResources[i].DestroyFrameBufferQueue = ListCreate();
		Graphics.FrameResources[i].DestroyUniformBufferQueue = ListCreate();
		Graphics.FrameResources[i].UpdateDescriptorQueue = ListCreate();
	}
	Graphics.FrameIndex = 0;
	Graphics.PreRenderSemaphores = ListCreate();
}

void GraphicsInitialize(GraphicsConfigure config)
{
	log_info("Initializing the graphics backend...\n");
	Graphics.FrameResourceCount = config.FrameResourceCount;
	Graphics.Swapchain.TargetPresentMode = config.TargetPresentMode;
	CheckExtensionSupport();
	CreateInstance(config.VulkanValidation);
	CreateSurface();
	ChoosePhysicalDevice(config.TargetIntegratedDevice);
	CreateLogicalDevice();
	CreateRenderPass();
	CreateCommandPool();
	CreateAllocator();
	CreateCompiler();
	CreateFrameResources();
	GraphicsCreateSwapchain(Window.Width, Window.Height);
	log_info("Successfully initialized the graphics backend.\n");
}

void GraphicsCreateSwapchain(int width, int height)
{
	log_info("Creating the swapchain...");
	Graphics.Swapchain.TargetExtent = (VkExtent2D) { .width = width, .height = height };
	CreateSwapchain(width, height);
	GetSwapchainImages();
	log_info("Successfully created the swapchain\n");
}

PresentMode GraphicsPresentMode()
{
	return Graphics.Swapchain.PresentMode;
}

void GraphicsSetPresentMode(PresentMode presentMode)
{
	Graphics.Swapchain.TargetPresentMode = presentMode;
	GraphicsDestroySwapchain();
	GraphicsCreateSwapchain(Graphics.Swapchain.TargetExtent.width, Graphics.Swapchain.Extent.height);
}

void GraphicsAquireNextImage()
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
	
	VkResult result = vkAcquireNextImageKHR(Graphics.Device, Graphics.Swapchain.Instance, UINT64_MAX, Graphics.FrameResources[i].ImageAvailable, VK_NULL_HANDLE, &Graphics.Swapchain.CurrentImageIndex);
	if (result != VK_SUCCESS) { log_info("Unsuccessful aquire image: %i\n", result); }
	while (result != VK_SUCCESS)
	{
		EventHandlerPoll();
		result = vkAcquireNextImageKHR(Graphics.Device, Graphics.Swapchain.Instance, UINT64_MAX, Graphics.FrameResources[i].ImageAvailable, VK_NULL_HANDLE, &Graphics.Swapchain.CurrentImageIndex);
	}
	
	vkResetCommandBuffer(Graphics.FrameResources[i].CommandBuffer, 0);
	VkCommandBufferBeginInfo beginInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
	};
	result = vkBeginCommandBuffer(Graphics.FrameResources[i].CommandBuffer, &beginInfo);
	if (result != VK_SUCCESS)
	{
		log_fatal("Failed to begin command buffer: %i\n", result);
		exit(1);
	}
}

void GraphicsPresent()
{
	unsigned int i = Graphics.FrameIndex;
	
	VkResult result = vkEndCommandBuffer(Graphics.FrameResources[i].CommandBuffer);
	if (result != VK_SUCCESS)
	{
		log_fatal("Failed to record command buffer: %i\n", result);
		exit(1);
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
		log_fatal("Failed to submit queue: %i\n", result);
		exit(1);
	}
	free(waitSemaphores);
	free(waitStages);
	
	VkPresentInfoKHR presentInfo =
	{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &Graphics.FrameResources[i].RenderFinished,
		.swapchainCount = 1,
		.pSwapchains = &Graphics.Swapchain.Instance,
		.pImageIndices = &Graphics.Swapchain.CurrentImageIndex,
	};
	vkQueuePresentKHR(Graphics.PresentQueue, &presentInfo);
}

void GraphicsDestroySwapchain()
{
	vkDeviceWaitIdle(Graphics.Device);
	free(Graphics.Swapchain.Images);
	vkDestroySwapchainKHR(Graphics.Device, Graphics.Swapchain.Instance, NULL);
}

void GraphicsBegin(FrameBuffer frameBuffer)
{
	Graphics.BoundFrameBuffer = frameBuffer;
	
	VkRenderPassBeginInfo renderPassBegin =
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = Graphics.RenderPass,
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

static void Clear(Color clearColor, float depth, int stencil, VkImageAspectFlagBits aspect)
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
	
	VkClearAttachment clear = aspect == VK_IMAGE_ASPECT_COLOR_BIT ? (VkClearAttachment)
	{
		.aspectMask = aspect,
		.clearValue = { .color = { color.X, color.Y, color.Z, color.W } },
	} : (VkClearAttachment)
	{
		.aspectMask = aspect,
		.clearValue = { .depthStencil = { .depth = depth, .stencil = stencil }, }
	};
	vkCmdClearAttachments(Graphics.FrameResources[Graphics.FrameIndex].CommandBuffer, 1, &clear, 1, &rect);
}

void GraphicsClearColor(Color clearColor)
{
	Clear(clearColor, 0.0, 0, VK_IMAGE_ASPECT_COLOR_BIT);
}

void GraphicsClearDepth(Scalar depth)
{
	Clear(ColorBlack, depth, 0, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void GraphicsClearStencil(unsigned int stencil)
{
	Clear(ColorBlack, 0.0, stencil, VK_IMAGE_ASPECT_STENCIL_BIT);
}

void GraphicsClear(Color clearColor, Scalar depth, int stencil)
{
	GraphicsClearColor(clearColor);
	Clear(clearColor, depth, stencil, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
}

void GraphicsBindPipeline(Pipeline pipeline)
{
	Graphics.BoundPipeline = pipeline;
	vkCmdBindPipeline(Graphics.FrameResources[Graphics.FrameIndex].CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->Instance);
	VkViewport viewport =
	{
		.x = 0.0f,
		.y = 0.0f,
		.width = Graphics.Swapchain.Extent.width,
		.height = Graphics.Swapchain.Extent.height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};
	VkRect2D scissor =
	{
		.offset = { 0, 0 },
		.extent = Graphics.Swapchain.Extent,
	};
	vkCmdSetViewport(Graphics.FrameResources[Graphics.FrameIndex].CommandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(Graphics.FrameResources[Graphics.FrameIndex].CommandBuffer, 0, 1, &scissor);
}

void GraphicsRenderVertexBuffer(VertexBuffer vertexBuffer)
{
	vkCmdSetLineWidth(Graphics.FrameResources[Graphics.FrameIndex].CommandBuffer, Graphics.BoundPipeline->LineWidth);
	vkCmdSetStencilReference(Graphics.FrameResources[Graphics.FrameIndex].CommandBuffer, VK_STENCIL_FACE_FRONT_BIT, Graphics.BoundPipeline->FrontStencilReference);
	vkCmdSetStencilReference(Graphics.FrameResources[Graphics.FrameIndex].CommandBuffer, VK_STENCIL_FACE_BACK_BIT, Graphics.BoundPipeline->BackStencilReference);
	
	if (Graphics.BoundPipeline->UsesPushConstant)
	{
		vkCmdPushConstants(Graphics.FrameResources[Graphics.FrameIndex].CommandBuffer, Graphics.BoundPipeline->Layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, Graphics.BoundPipeline->PushConstantSize, Graphics.BoundPipeline->PushConstantData);
	}
	if (Graphics.BoundPipeline->UsesDescriptors)
	{
		vkCmdBindDescriptorSets(Graphics.FrameResources[Graphics.FrameIndex].CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Graphics.BoundPipeline->Layout, 0, 1, &Graphics.BoundPipeline->DescriptorSet[Graphics.FrameIndex], 0, NULL);
	}
	
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(Graphics.FrameResources[Graphics.FrameIndex].CommandBuffer, 0, 1, &vertexBuffer->VertexBuffer, &offset);
	if (vertexBuffer->IndexCount > 0)
	{
		VkDeviceSize offset = vertexBuffer->VertexCount * vertexBuffer->VertexSize;
		vkCmdBindIndexBuffer(Graphics.FrameResources[Graphics.FrameIndex].CommandBuffer, vertexBuffer->VertexBuffer, offset, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(Graphics.FrameResources[Graphics.FrameIndex].CommandBuffer, vertexBuffer->IndexCount, 1, 0, 0, 0);
	}
	else
	{
		vkCmdDraw(Graphics.FrameResources[Graphics.FrameIndex].CommandBuffer, vertexBuffer->VertexCount, 1, 0, 0);
	}
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
		.image = Graphics.Swapchain.Images[Graphics.Swapchain.CurrentImageIndex],
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
	
	VkImageBlit blitInfo =
	{
		.srcOffsets = { { .x = 0, .y = 0, .z = 0}, { .x = frameBuffer->Width, .y = frameBuffer->Height, 1 } },
		.srcSubresource =
		{
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.mipLevel = 0,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
		.dstOffsets = { { .x = 0, .y = 0, .z = 0}, { .x = Window.Width - 1, .y = Window.Height, 1 } },
		.dstSubresource =
		{
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.mipLevel = 0,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
	};
	vkCmdBlitImage(Graphics.FrameResources[i].CommandBuffer, frameBuffer->ColorTexture->Image, VK_IMAGE_LAYOUT_GENERAL, Graphics.Swapchain.Images[Graphics.Swapchain.CurrentImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blitInfo, (VkFilter)frameBuffer->Filter);
	
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
	GraphicsDestroySwapchain();
	ListDestroy(Graphics.PreRenderSemaphores);
	for (int i = 0; i < Graphics.FrameResourceCount; i++)
	{
		for (int j = 0; j < Graphics.FrameResources[i].DestroyVertexBufferQueue->Count; j++)
		{
			VertexBuffer vertexBuffer = ListIndex(Graphics.FrameResources[i].DestroyVertexBufferQueue, j);
			VertexBufferDestroy(vertexBuffer);
		}
		ListDestroy(Graphics.FrameResources[i].DestroyVertexBufferQueue);
		for (int j = 0; j < Graphics.FrameResources[i].DestroyUniformBufferQueue->Count; j++)
		{
			UniformBuffer uniformBuffer = ListIndex(Graphics.FrameResources[i].DestroyUniformBufferQueue, j);
			UniformBufferDestroy(uniformBuffer);
		}
		ListDestroy(Graphics.FrameResources[i].DestroyUniformBufferQueue);
		for (int j = 0; j < Graphics.FrameResources[i].DestroyFrameBufferQueue->Count; j++)
		{
			FrameBuffer frameBuffer = ListIndex(Graphics.FrameResources[i].DestroyFrameBufferQueue, j);
			FrameBufferDestroy(frameBuffer);
		}
		ListDestroy(Graphics.FrameResources[i].DestroyFrameBufferQueue);
		for (int j = 0; j < Graphics.FrameResources[i].DestroyPipelineQueue->Count; j++)
		{
			Pipeline pipeline = ListIndex(Graphics.FrameResources[i].DestroyPipelineQueue, j);
			PipelineDestroy(pipeline);
		}
		ListDestroy(Graphics.FrameResources[i].DestroyPipelineQueue);
		for (int j = 0; j < Graphics.FrameResources[i].DestroyTextureQueue->Count; j++)
		{
			Texture texture = ListIndex(Graphics.FrameResources[i].DestroyTextureQueue, j);
			TextureDestroy(texture);
		}
		ListDestroy(Graphics.FrameResources[i].DestroyTextureQueue);
		
		ListDestroy(Graphics.FrameResources[i].UpdateDescriptorQueue);
		vkDestroyFence(Graphics.Device, Graphics.FrameResources[i].FrameReady, NULL);
		vkDestroySemaphore(Graphics.Device, Graphics.FrameResources[i].RenderFinished, NULL);
		vkDestroySemaphore(Graphics.Device, Graphics.FrameResources[i].ImageAvailable, NULL);
		vkFreeCommandBuffers(Graphics.Device, Graphics.CommandPool, 1, &Graphics.FrameResources[i].CommandBuffer);
	}
	free(Graphics.FrameResources);
	shaderc_compiler_release(Graphics.ShaderCompiler);
	vmaDestroyAllocator(Graphics.Allocator);
	vkDestroyCommandPool(Graphics.Device, Graphics.CommandPool, NULL);
	vkDestroyRenderPass(Graphics.Device, Graphics.RenderPass, NULL);
	vkDestroyDevice(Graphics.Device, NULL);
	vkDestroySurfaceKHR(Graphics.Instance, Graphics.Surface, NULL);
	vkDestroyInstance(Graphics.Instance, NULL);
}
