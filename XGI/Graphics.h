#ifndef Graphics_h
#define Graphics_h

#include <vulkan/vulkan.h>
#include <shaderc/shaderc.h>
#include <vk_mem_alloc.h>
#include "Pipeline.h"
#include "VertexBuffer.h"
#include "LinearMath.h"
#include "UniformBuffer.h"
#include "FrameBuffer.h"
#include "EventHandler.h"

typedef struct GraphicsConfigure
{
	/// Whether or not vulkan validation layers should be enabled.
	/// Recommend true for debug and false for release
	bool VulkanValidation;
	/// Whether or not an integrated or dedicated graphics card should be used if possible
	/// Recommended false
	bool TargetIntegratedDevice;
	/// The number of frame resources to use
	/// Recommended 3 for maximum cpu and gpu balance
	/// Anything higher than 1 won't make a difference if the device doesn't support tripple buffering
	int FrameResourceCount;
} GraphicsConfigure;

struct Graphics
{
	VkInstance Instance;
	VkSurfaceKHR Surface;
	VkPhysicalDevice PhysicalDevice;
	VkDevice Device;
	VkQueue GraphicsQueue;
	unsigned int GraphicsQueueIndex;
	VkQueue PresentQueue;
	unsigned int PresentQueueIndex;
	
	struct GraphicsSwapchain
	{
		VkSwapchainKHR Instance;
		VkExtent2D Extent;
		VkFormat ColorFormat;
		unsigned int ImageCount;
		VkImage * Images;
		unsigned int CurrentImageIndex;
	} Swapchain;
	
	VkRenderPass RenderPass;
	VkCommandPool CommandPool;
	VmaAllocator Allocator;
	shaderc_compiler_t ShaderCompiler;
	List PreRenderSemaphores;
	
	int FrameResourceCount;
	struct GraphicsFrameResource
	{
		VkCommandBuffer CommandBuffer;
		VkSemaphore ImageAvailable;
		VkSemaphore RenderFinished;
		VkFence FrameReady;
		List DestroyVertexBufferQueue;
		List DestroyUniformBufferQueue;
		List DestroyFrameBufferQueue;
		List DestroyPipelineQueue;
		List DestroyTextureQueue;
		List UpdateDescriptorQueue;
	} * FrameResources;
	int FrameIndex;
	
	FrameBuffer BoundFrameBuffer;
	Pipeline BoundPipeline;
} extern Graphics;

/// This should not be called by the user, it is called in the XGIInitialize function
void GraphicsInitialize(GraphicsConfigure config);

/// This should not be called, it is automatically called at initialization, and when the window is resized.
void GraphicsCreateSwapchain(int width, int height);

/// Acquires the next swapchain image for rendering.
/// This should be called once per a frame, before any rendering operations are done
void GraphicsAquireNextImage(void);

/// Presents the acquired image to the screen
/// This should be called once per a frame, after all render calls have been finished
void GraphicsPresent(void);

/// This should not be called, it is automatically called at deinitialization, and when the window is resized.
void GraphicsDestroySwapchain(void);

/// Begins rendering on a given framebuffer.
/// This should only be called after SwapchainAquireNextImage and before SwapchainPresent
/// \param frameBuffer the framebuffer to start rendering on
void GraphicsBegin(FrameBuffer frameBuffer);

/// Clears the color of the currently bound framebuffer.
/// This shoud only be called after GraphicsBegin and before GraphicsEnd
/// \param clearColor Color to clear to
void GraphicsClearColor(Color clearColor);

/// Clears the depth  of the currently bound framebuffer.
/// This shoud only be called after GraphicsBegin and before GraphicsEnd
/// \param depth Depth value to clear to
void GraphicsClearDepth(Scalar depth);

/// Clears the stencil of the currently bound framebuffer.
/// This shoud only be called after GraphicsBegin and before GraphicsEnd
/// \param stencil Stencil value to clear to
void GraphicsClearStencil(unsigned int stencil);

/// Clears the color, depth and stencil of the currently bound framebuffer.
/// This shoud only be called after GraphicsBegin and before GraphicsEnd
/// \param clearColor Color to clear to
/// \param depth Depth value to clear to
/// \param stencil Stencil value to clear to
void GraphicsClear(Color clearColor, Scalar depth, int stencil);

/// Binds a pipeline to use for rendering.
/// This shoud only be called after GraphicsBegin and before GraphicsEnd.
/// Make sure all bindings that the shaders use are set with the pipeline before using it.
/// \param pipeline The pipeline to bind
void GraphicsBindPipeline(Pipeline pipeline);

/// Renders a vertexbuffer to the currently bound framebuffer using the currently bound pipeline.
/// This shoud only be called after GraphicsBegin and before GraphicsEnd.
/// A pipeline must be bound before calling this
void GraphicsRenderVertexBuffer(VertexBuffer vertexBuffer);

/// Ends rendering to a framebuffer.
/// This should be called after GraphicsBegin and before SwapchainPresent
void GraphicsEnd(void);

/// Copies a framebuffer to the swapchain for rendering,
/// This should only be called after GraphicsEnd and before SwapchainPresent
void GraphicsCopyToSwapchain(FrameBuffer frameBuffer);

/// Syncs all graphics operations with the cpu.
/// This should be called right before deinitializing the application
void GraphicsStopOperations(void);

/// The user shouldn't call this, it is called in XGIDeinitialize
void GraphicsDeinitialize(void);

#endif
