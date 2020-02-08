#ifndef Swapchain_h
#define Swapchain_h

#include <vulkan/vulkan.h>
#include "FrameBuffer.h"

struct Swapchain
{	
	VkSwapchainKHR Instance;
	VkExtent2D Extent;
	VkFormat ColorFormat;
	VkRenderPass RenderPass;
	unsigned int ImageCount;
	VkImage * Images;
	unsigned int CurrentImageIndex;
} extern Swapchain;

void SwapchainCreate(int width, int height);
void SwapchainAquireNextImage(void);
void SwapchainPresent(void);
void SwapchainDestroy(void);

#endif
