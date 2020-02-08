#ifndef Graphics_h
#define Graphics_h

#include <vulkan/vulkan.h>
#include <libshaderc/shaderc.h>
#include <vk_mem_alloc.h>
#include "Pipeline.h"
#include "VertexBuffer.h"
#include "LinearMath.h"
#include "UniformBuffer.h"
#include "FrameBuffer.h"

typedef struct GraphicsConfigure
{
	bool VulkanValidation;
	bool TargetIntegratedDevice;
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
	
	int VertexBufferCount;
	int VertexCount;
} extern Graphics;

void GraphicsInitialize(GraphicsConfigure config);
void GraphicsBegin(FrameBuffer frameBuffer);
void GraphicsClearColor(Color clearColor);
void GraphicsClearDepth(Scalar depth);
void GraphicsClearStencil(unsigned int stencil);
void GraphicsClear(Color clearColor, Scalar depth, int stencil);
void GraphicsBindPipeline(Pipeline pipeline);
void GraphicsRenderVertexBuffer(VertexBuffer vertexBuffer);
void GraphicsEnd(void);
void GraphicsCopyToSwapchain(FrameBuffer frameBuffer);
void GraphicsStopOperations(void);
void GraphicsDeinitialize(void);

#endif
