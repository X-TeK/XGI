#ifndef Vertexbuffer_h
#define Vertexbuffer_h

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"
#include "LinearMath.h"

typedef enum VertexAttribute
{
	VertexAttributeVector4 = VK_FORMAT_R32G32B32A32_SFLOAT,
	VertexAttributeByte4 = VK_FORMAT_R8G8B8A8_UNORM,
	VertexAttributeVector3 = VK_FORMAT_R32G32B32_SFLOAT,
	VertexAttributeVector2 = VK_FORMAT_R32G32_SFLOAT,
	VertexAttributeFloat = VK_FORMAT_R32_SFLOAT,
} VertexAttribute;

typedef struct VertexLayout
{
	VkVertexInputBindingDescription Binding;
	unsigned int AttributeCount;
	VkVertexInputAttributeDescription * Attributes;
	unsigned int Size;
} * VertexLayout;

VertexLayout VertexLayoutCreate(int attributeCount, VertexAttribute * attributes);
void VertexLayoutDestroy(VertexLayout layout);

typedef struct VertexBuffer
{
	int VertexCount;
	int VertexSize;
	VkBuffer StagingBuffer;
	VmaAllocation StagingAllocation;
	VkBuffer VertexBuffer;
	VmaAllocation VertexAllocation;
	VkCommandBuffer CommandBuffer;
	VkFence Fence;
	VkSemaphore Semaphore;
} * VertexBuffer;

VertexBuffer VertexBufferCreate(int vertexCount, int vertexSize);
void * VertexBufferMapVertices(VertexBuffer vertexBuffer);
void VertexBufferUnmapVertices(VertexBuffer vertexBuffer);
void VertexBufferUpload(VertexBuffer vertexBuffer);
void VertexBufferQueueDestroy(VertexBuffer vertexBuffer);
void VertexBufferDestroy(VertexBuffer vertexBuffer);

#endif
