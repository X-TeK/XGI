#include <stdlib.h>
#include <stdio.h>
#include "Graphics.h"
#include "VertexBuffer.h"
#include "vk_mem_alloc.h"

static VertexBuffer_T Create(int vertexCount)
{
	VertexBuffer_T vertexBuffer = malloc(sizeof(struct VertexBuffer));
	*vertexBuffer = (struct VertexBuffer){ .VertexCount = vertexCount };
	
	size_t size = vertexCount * sizeof(Vertex);
	
	VkBufferCreateInfo stagingInfo =
	{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = size,
		.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	};
	VmaAllocationCreateInfo stagingAllocationInfo =
	{
		.usage = VMA_MEMORY_USAGE_CPU_ONLY,
	};
	vmaCreateBuffer(Graphics.Allocator, &stagingInfo, &stagingAllocationInfo, &vertexBuffer->StagingBuffer, &vertexBuffer->StagingAllocation, NULL);
	
	VkBufferCreateInfo bufferInfo =
	{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = size,
		.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	};
	VmaAllocationCreateInfo allocationInfo =
	{
		.usage = VMA_MEMORY_USAGE_GPU_ONLY,
	};
	vmaCreateBuffer(Graphics.Allocator, &bufferInfo, &allocationInfo, &vertexBuffer->VertexBuffer, &vertexBuffer->VertexAllocation, NULL);
	
	VkCommandBufferAllocateInfo commandAllocateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandPool = Graphics.CommandPool,
		.commandBufferCount = 1,
	};
	vkAllocateCommandBuffers(Graphics.Device, &commandAllocateInfo, &vertexBuffer->CommandBuffer);
	
	VkFenceCreateInfo fenceInfo =
	{
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT,
	};
	vkCreateFence(Graphics.Device, &fenceInfo, NULL, &vertexBuffer->Fence);
	VkSemaphoreCreateInfo semaphoreInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
	};
	vkCreateSemaphore(Graphics.Device, &semaphoreInfo, NULL, &vertexBuffer->Semaphore);
	
	Graphics.VertexBufferCount++;
	Graphics.VertexCount += vertexCount;
	
	return vertexBuffer;
}

static Vertex * MapVertices(VertexBuffer_T vertexBuffer)
{
	void * data;
	vmaMapMemory(Graphics.Allocator, vertexBuffer->StagingAllocation, &data);
	return data;
}

static void UnmapVertices(VertexBuffer_T vertexBuffer)
{
	vmaUnmapMemory(Graphics.Allocator, vertexBuffer->StagingAllocation);
}

static void UploadStagingBuffer(VertexBuffer_T vertexBuffer)
{
	vkWaitForFences(Graphics.Device, 1, &vertexBuffer->Fence, VK_TRUE, UINT64_MAX);
	vkResetFences(Graphics.Device, 1, &vertexBuffer->Fence);
	
	VkCommandBufferBeginInfo beginInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
	};
	
	vkBeginCommandBuffer(vertexBuffer->CommandBuffer, &beginInfo);
	VkBufferCopy copyInfo =
	{
		.srcOffset = 0,
		.dstOffset = 0,
		.size = vertexBuffer->VertexCount * sizeof(Vertex),
	};
	vkCmdCopyBuffer(vertexBuffer->CommandBuffer, vertexBuffer->StagingBuffer, vertexBuffer->VertexBuffer, 1, &copyInfo);
	vkEndCommandBuffer(vertexBuffer->CommandBuffer);
	
	VkSubmitInfo submitInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers = &vertexBuffer->CommandBuffer,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &vertexBuffer->Semaphore,
	};
	vkQueueSubmit(Graphics.GraphicsQueue, 1, &submitInfo, vertexBuffer->Fence);
	ListPush(Graphics.PreRenderSemaphores, &vertexBuffer->Semaphore);
}

static void QueueDestroy(VertexBuffer_T vertexBuffer)
{
	ListPush(Graphics.FrameResources[Graphics.FrameIndex].DestroyVertexBufferQueue, vertexBuffer);
}

static void Destroy(VertexBuffer_T vertexBuffer)
{
	Graphics.VertexBufferCount--;
	Graphics.VertexCount -= vertexBuffer->VertexCount;
	vkWaitForFences(Graphics.Device, 1, &vertexBuffer->Fence, VK_TRUE, UINT64_MAX);
	vkDestroyFence(Graphics.Device, vertexBuffer->Fence, NULL);
	vkDestroySemaphore(Graphics.Device, vertexBuffer->Semaphore, NULL);
	vkFreeCommandBuffers(Graphics.Device, Graphics.CommandPool, 1, &vertexBuffer->CommandBuffer);
	vmaDestroyBuffer(Graphics.Allocator, vertexBuffer->StagingBuffer, vertexBuffer->StagingAllocation);
	vmaDestroyBuffer(Graphics.Allocator, vertexBuffer->VertexBuffer, vertexBuffer->VertexAllocation);
	free(vertexBuffer);
}

struct VertexBufferInterface VertexBuffer =
{
	.Create = Create,
	.MapVertices = MapVertices,
	.UnmapVertices = UnmapVertices,
	.UploadStagingBuffer = UploadStagingBuffer,
	.QueueDestroy = QueueDestroy,
	.Destroy = Destroy,
};
