#ifndef Graphics_h
#define Graphics_h

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"
#include "Pipeline.h"
#include "VertexBuffer.h"
#include "LinearMath.h"
#include "UniformBuffer.h"
#include "FrameBuffer.h"

typedef struct GraphicsConfig
{
	bool VulkanValidation;
	bool TargetIntegratedDevice;
	int FrameResourceCount;
} GraphicsConfig;

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
	
	VkDescriptorSetLayout DescriptorSetLayout0;
	VkDescriptorSetLayout DescriptorSetLayout1;
	VkDescriptorPool DescriptorPool;
	
	List PreRenderSemaphores;
	int FrameResourceCount;
	struct
	{
		VkCommandBuffer CommandBuffer;
		VkSemaphore ImageAvailable;
		VkSemaphore RenderFinished;
		VkFence FrameReady;
		List DestroyVertexBufferQueue;
	} * FrameResources;
	int FrameIndex;
	
	VkVertexInputBindingDescription VertexBindingDescription;
	VkVertexInputAttributeDescription VertexPositionDescription;
	VkVertexInputAttributeDescription VertexColorDescription;
	VkVertexInputAttributeDescription VertexNormalDescription;
	
	FrameBuffer BoundFrameBuffer;
	Pipeline BoundPipeline;
	
	VertexBuffer_T Quad;
	UniformBuffer_T QuadUniform;
	
	int VertexBufferCount;
	int VertexCount;
} extern Graphics;

void GraphicsInitialize(GraphicsConfig config);
void GraphicsBegin(FrameBuffer frameBuffer);
void GraphicsClearColor(Color clearColor);
void GraphicsDepthStencil(float depth, int stencil);
void GraphicsClear(Color clearColor, float depth, int stencil);
void GraphicsBindPipeline(Pipeline pipeline, void * pushConstant);
void GraphicsRenderVertexBuffer(VertexBuffer_T vertexBuffer, UniformBuffer_T uniformBuffer, FrameBuffer sampler);
void GraphicsEnd(void);
void GraphicsCopyToSwapchain(FrameBuffer frameBuffer);
void GraphicsStopOperations(void);
void GraphicsDeinitialize(void);

#endif
