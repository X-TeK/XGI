#ifndef Vertexbuffer_h
#define Vertexbuffer_h

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"
#include "LinearMath.h"

typedef struct vertex
{
	Vector3 Position;
	Color Color;
	Vector3 Normal;
} Vertex;

typedef struct VertexBuffer
{
	int VertexCount;
	VkBuffer StagingBuffer;
	VmaAllocation StagingAllocation;
	VkBuffer VertexBuffer;
	VmaAllocation VertexAllocation;
	VkCommandBuffer CommandBuffer;
	VkFence Fence;
	VkSemaphore Semaphore;
} * VertexBuffer_T;

struct VertexBufferInterface
{
	VertexBuffer_T (*Create)(int vertexCount);
	Vertex * (*MapVertices)(VertexBuffer_T vertexBuffer);
	void (*UnmapVertices)(VertexBuffer_T vertexBuffer);
	void (*UploadStagingBuffer)(VertexBuffer_T vertexBuffer);
	void (*QueueDestroy)(VertexBuffer_T vertexBuffer);
	void (*Destroy)(VertexBuffer_T vertexBuffer);
} extern VertexBuffer;

#endif
