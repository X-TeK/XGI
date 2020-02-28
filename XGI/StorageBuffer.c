#include "StorageBuffer.h"
#include "Graphics.h"
#include "log.h"

StorageBuffer StorageBufferCreate(struct Pipeline * pipeline, int binding, int instanceCount)
{
	StorageBuffer storageBuffer = malloc(sizeof(struct StorageBuffer));
	*storageBuffer = (struct StorageBuffer){ 0 };
	
	unsigned long size = 0;
	
	bool foundBinding = false;
	for (int i = 0; i < pipeline->StageCount; i++)
	{
		for (int j = 0; j < pipeline->Stages[i].BindingCount; j++)
		{
			SpvReflectDescriptorBinding * bindingInfo = pipeline->Stages[i].DescriptorInfo.bindings[j];
			if (bindingInfo->binding == binding && bindingInfo->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER)
			{
				storageBuffer->Info = bindingInfo->block;
				for (int k = 0; k < bindingInfo->block.member_count; k++)
				{
					if (bindingInfo->block.members[k].type_description->op == SpvOpTypeRuntimeArray)
					{
						SpvReflectNumericTraits numeric = bindingInfo->block.members[k].numeric;
						unsigned long varSize = numeric.scalar.width / 8;
						varSize *= numeric.vector.component_count == 0 ? 1 : numeric.vector.component_count;
						varSize *= numeric.matrix.row_count == 0 ? 1 : numeric.matrix.row_count;
						size += varSize * instanceCount;
					}
					else
					{
						size += bindingInfo->block.members[k].size;
					}
				}
				foundBinding = true;
				break;
			}
		}
	}
	if (!foundBinding) { abort(); }
	
	storageBuffer->Size = size;
	
	VkBufferCreateInfo stagingInfo =
	{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = size,
		.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	};
	VmaAllocationCreateInfo stagingAllocationInfo =
	{
		.usage = VMA_MEMORY_USAGE_CPU_ONLY,
	};
	vmaCreateBuffer(Graphics.Allocator, &stagingInfo, &stagingAllocationInfo, &storageBuffer->StagingBuffer, &storageBuffer->StagingAllocation, NULL);
	
	VkBufferCreateInfo bufferInfo =
	{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = size,
		.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	};
	VmaAllocationCreateInfo allocationInfo =
	{
		.usage = VMA_MEMORY_USAGE_GPU_ONLY,
	};
	vmaCreateBuffer(Graphics.Allocator, &bufferInfo, &allocationInfo, &storageBuffer->Buffer, &storageBuffer->Allocation, NULL);
	
	VkCommandBufferAllocateInfo commandAllocateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandPool = Graphics.CommandPool,
		.commandBufferCount = 1,
	};
	vkAllocateCommandBuffers(Graphics.Device, &commandAllocateInfo, &storageBuffer->UploadCommandBuffer);
	vkAllocateCommandBuffers(Graphics.Device, &commandAllocateInfo, &storageBuffer->DownloadCommandBuffer);
	
	VkFenceCreateInfo fenceInfo =
	{
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT,
	};
	vkCreateFence(Graphics.Device, &fenceInfo, NULL, &storageBuffer->UploadFence);
	fenceInfo.flags = 0;
	vkCreateFence(Graphics.Device, &fenceInfo, NULL, &storageBuffer->DownloadFence);
	
	return storageBuffer;
}

void * StorageBufferMapVariable(StorageBuffer storageBuffer, const char * variable)
{
	void * data;
	vmaMapMemory(Graphics.Allocator, storageBuffer->StagingAllocation, &data);
	unsigned long offset = 0;
	for (int i = 0; i < storageBuffer->Info.member_count; i++)
	{
		if (strcmp(storageBuffer->Info.members[i].name, variable) == 0)
		{
			offset = storageBuffer->Info.members[i].offset;
		}
	}
	return (unsigned char *)data + offset;
}

void StorageBufferUnmapVariable(StorageBuffer storageBuffer)
{
	vmaUnmapMemory(Graphics.Allocator, storageBuffer->StagingAllocation);
}

void StorageBufferUpload(StorageBuffer storageBuffer)
{
	vkWaitForFences(Graphics.Device, 1, &storageBuffer->UploadFence, VK_TRUE, UINT64_MAX);
	vkResetFences(Graphics.Device, 1, &storageBuffer->UploadFence);
	
	VkCommandBufferBeginInfo beginInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
	};
	
	vkBeginCommandBuffer(storageBuffer->UploadCommandBuffer, &beginInfo);
	VkBufferCopy copyInfo =
	{
		.srcOffset = 0,
		.dstOffset = 0,
		.size = storageBuffer->Size,
	};
	vkCmdCopyBuffer(storageBuffer->UploadCommandBuffer, storageBuffer->StagingBuffer, storageBuffer->Buffer, 1, &copyInfo);
	vkEndCommandBuffer(storageBuffer->UploadCommandBuffer);
	
	VkSubmitInfo submitInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers = &storageBuffer->UploadCommandBuffer,
		.signalSemaphoreCount = 0,
		.pSignalSemaphores = NULL,
	};
	vkQueueSubmit(Graphics.GraphicsQueue, 1, &submitInfo, storageBuffer->UploadFence);
}

void StorageBufferDownload(StorageBuffer storageBuffer)
{
	VkCommandBufferBeginInfo beginInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
	};
	
	vkBeginCommandBuffer(storageBuffer->UploadCommandBuffer, &beginInfo);
	VkBufferCopy copyInfo =
	{
		.srcOffset = 0,
		.dstOffset = 0,
		.size = storageBuffer->Size,
	};
	vkCmdCopyBuffer(storageBuffer->UploadCommandBuffer, storageBuffer->Buffer, storageBuffer->StagingBuffer, 1, &copyInfo);
	vkEndCommandBuffer(storageBuffer->UploadCommandBuffer);
	
	VkSubmitInfo submitInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers = &storageBuffer->UploadCommandBuffer,
		.signalSemaphoreCount = 0,
		.pSignalSemaphores = NULL,
	};
	vkQueueSubmit(Graphics.GraphicsQueue, 1, &submitInfo, storageBuffer->DownloadFence);
	vkWaitForFences(Graphics.Device, 1, &storageBuffer->DownloadFence, VK_TRUE, UINT64_MAX);
}

void StorageBufferQueueDestroy(StorageBuffer storageBuffer)
{
	ListPush(Graphics.FrameResources[Graphics.FrameIndex].Queues[GraphicsQueueDestroyStorageBuffer], storageBuffer);
}

void StorageBufferDestroy(StorageBuffer storageBuffer)
{
	vkWaitForFences(Graphics.Device, 1, &storageBuffer->UploadFence, VK_TRUE, UINT64_MAX);
	vkDestroyFence(Graphics.Device, storageBuffer->UploadFence, NULL);
	vkDestroyFence(Graphics.Device, storageBuffer->DownloadFence, NULL);
	vkFreeCommandBuffers(Graphics.Device, Graphics.CommandPool, 1, &storageBuffer->UploadCommandBuffer);
	vkFreeCommandBuffers(Graphics.Device, Graphics.CommandPool, 1, &storageBuffer->DownloadCommandBuffer);
	vmaDestroyBuffer(Graphics.Allocator, storageBuffer->StagingBuffer, storageBuffer->StagingAllocation);
	vmaDestroyBuffer(Graphics.Allocator, storageBuffer->Buffer, storageBuffer->Allocation);
	free(storageBuffer);
}
