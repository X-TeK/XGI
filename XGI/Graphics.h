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

typedef enum PresentMode
{
	/// Images aren't waited till the next v-blanking to be presented, they are presented immediately. Tearing may be visible.
	PresentModeImmediate = VK_PRESENT_MODE_IMMEDIATE_KHR,
	/// Images are placed into a queue that waits for the next v-blanking to present each image. Tearing isn't visible.
	PresentModeVSync = VK_PRESENT_MODE_FIFO_KHR,
	/// Images are mostly waited until the next v-blanking, unless it's late then it's presented immediately. Tearing may be visible.
	PresentModeRelaxedVSync = VK_PRESENT_MODE_FIFO_RELAXED_KHR,
	/// Images are presented each v-blanking except there is no queue to wait on, the next image replaces the previous image waiting. Tearing isn't visible.
	PresentModeImmediateVsync = VK_PRESENT_MODE_MAILBOX_KHR,
} PresentMode;

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
	/// Anything higher than 1 won't make much of a difference if the present mode is VSync or RelaxedVsync
	int FrameResourceCount;
	/// The present mode to use, if  possible.
	/// The only one guarenteed to be available is PresentModeVSync
	PresentMode TargetPresentMode;
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
	bool ComputeQueueSupported;
	VkQueue ComputeQueue;
	unsigned int ComputeQueueIndex;
	
	struct GraphicsSwapchain
	{
		VkSwapchainKHR Instance;
		PresentMode TargetPresentMode;
		PresentMode PresentMode;
		VkExtent2D TargetExtent;
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
		VkCommandBuffer ComputeCommandBuffer;
		VkSemaphore ComputeFinished;
		VkFence ComputeFence;
		List DestroyVertexBufferQueue;
		List DestroyUniformBufferQueue;
		List DestroyFrameBufferQueue;
		List DestroyStorageBufferQueue;
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

/// Gets the present mode that is currently in use
/// \return The present mode that was chosen
PresentMode GraphicsPresentMode(void);

/// Attempts to set the present mode to the specified one, if it's not supported then it's set to PresentModeVsync
/// \param presentMode The desired present mode to set
void GraphicsSetPresentMode(PresentMode presentMode);

/// Begins recording commands for use on a compute pipeline.
/// This should be called once a frame and all compute pipeline dispatches should be called after this.
void GraphicsStartCompute(void);

/// Dispatches a compute pipeline based off of 3 work group dimensions.
/// This should be called within GraphicsStartCompute and GraphicsEndCompute.
/// \param pipeline The compute pipeline to dispatch
/// \param xGroups The number of work groups in the x direction
/// \param yGroups The number of work groups in the y direction
/// \param zGroups The number of work groups in the z direction
void GraphicsDispatch(ComputePipeline pipeline, int xGroups, int yGroups, int zGroups);

/// Ends recording of compute pipeline dispatches.
/// This should be called once a frame and should only be called after GraphicsStartCompute.
void GraphicsEndCompute(void);

/// Advances the engine to the next frame resource.
/// Also processes anything in queues at this point (like VertexBufferQueueDestroy).
/// This should be called once per a frame, before any graphics or compute operations are done.
void GraphicsUpdate(void);

/// Acquires the next swapchain image for rendering.
/// This should be called once per a frame, specifically before any rendering operations are called
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
