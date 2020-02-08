#include <stdlib.h>
#include <stdio.h>
#include <vk_mem_alloc.h>
#include "Graphics.h"
#include "VertexBuffer.h"

VertexLayout VertexLayoutCreate(int attributeCount, VertexAttribute * attributes)
{
	VertexLayout layout = malloc(sizeof(struct VertexLayout));
	*layout = (struct VertexLayout)
	{
		.AttributeCount = attributeCount,
		.Attributes = malloc(attributeCount * sizeof(VkVertexInputAttributeDescription)),
	};
	int size = 0;
	for (int i = 0; i < attributeCount; i++)
	{
		layout->Attributes[i] = (VkVertexInputAttributeDescription)
		{
			.binding = 0,
			.format = (VkFormat)attributes[i],
			.location = i,
			.offset = size,
		};
		switch (attributes[i])
		{
			case VertexAttributeVector4: size += 16; break;
			case VertexAttributeVector3: size += 12; break;
			case VertexAttributeVector2: size += 8; break;
			case VertexAttributeFloat: size += 4; break;
			case VertexAttributeByte4: size += 4; break;
		}
	}
	layout->Binding = (VkVertexInputBindingDescription)
	{
		.binding = 0,
		.stride = size,
		.inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
	};
	layout->Size = size;
	return layout;
}

void VertexLayoutDestroy(VertexLayout layout)
{
	free(layout->Attributes);
	free(layout);
}

VertexBuffer VertexBufferCreate(int vertexCount, int vertexSize)
{
	VertexBuffer vertexBuffer = malloc(sizeof(struct VertexBuffer));
	*vertexBuffer = (struct VertexBuffer){ .VertexCount = vertexCount, .VertexSize = vertexSize };
	
	size_t size = vertexCount * vertexSize;
	
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

void * VertexBufferMapVertices(VertexBuffer vertexBuffer)
{
	void * data;
	vmaMapMemory(Graphics.Allocator, vertexBuffer->StagingAllocation, &data);
	return data;
}

void VertexBufferUnmapVertices(VertexBuffer vertexBuffer)
{
	vmaUnmapMemory(Graphics.Allocator, vertexBuffer->StagingAllocation);
}

void VertexBufferUpload(VertexBuffer vertexBuffer)
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
		.size = vertexBuffer->VertexCount * vertexBuffer->VertexSize,
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

void VertexBufferQueueDestroy(VertexBuffer vertexBuffer)
{
	ListPush(Graphics.FrameResources[Graphics.FrameIndex].DestroyVertexBufferQueue, vertexBuffer);
}

void VertexBufferDestroy(VertexBuffer vertexBuffer)
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
