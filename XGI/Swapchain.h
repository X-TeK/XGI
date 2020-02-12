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

/// This should not be called, it is automatically called at initialization, and when the window is resized.
void SwapchainCreate(int width, int height);

/// Acquires the next swapchain image for rendering.
/// This should be called once per a frame, before any rendering operations are done
void SwapchainAquireNextImage(void);

/// Presents the acquired image to the screen
/// This should be called once per a frame, after all render calls have been finished
void SwapchainPresent(void);

/// This should not be called, it is automatically called at deinitialization, and when the window is resized.
void SwapchainDestroy(void);

#endif
