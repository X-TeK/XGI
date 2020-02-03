#include <stdio.h>
#include <stdlib.h>
#include "UniformBuffer.h"
#include "Graphics.h"

UniformBuffer UniformBufferCreate(Pipeline pipeline, int binding)
{
	UniformBuffer uniformBuffer = malloc(sizeof(struct UniformBuffer));
	*uniformBuffer = (struct UniformBuffer){ 0 };
	
	bool foundBinding = false;
	for (int i = 0; i < pipeline->StageCount; i++)
	{
		for (int j = 0; j < pipeline->Stages[i].DescriptorInfo.binding_count; j++)
		{
			SpvReflectDescriptorBinding * bindingInfo = pipeline->Stages[i].DescriptorInfo.bindings[j];
			if (bindingInfo->binding == binding && bindingInfo->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
			{
				//memcpy(&uniformBuffer->Info, &binding->block, sizeof(binding->block));
				uniformBuffer->Info = bindingInfo->block;
				foundBinding = true;
			}
		}
	}
	if (!foundBinding) { abort(); }
	
	uniformBuffer->Size = uniformBuffer->Info.size;
	VkBufferCreateInfo bufferInfo =
	{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = uniformBuffer->Info.size,
		.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
	};
	VmaAllocationCreateInfo allocationInfo =
	{
		.usage = VMA_MEMORY_USAGE_CPU_ONLY,
	};
	VmaAllocationInfo info;
	vmaCreateBuffer(Graphics.Allocator, &bufferInfo, &allocationInfo, &uniformBuffer->Buffer, &uniformBuffer->Allocation, &info);
	
	return uniformBuffer;
}

void UniformBufferSetVariable(UniformBuffer uniformBuffer, const char * variable, void * value)
{
	void * data;
	vmaMapMemory(Graphics.Allocator, uniformBuffer->Allocation, &data);
	for (int i = 0; i < uniformBuffer->Info.member_count; i++)
	{
		if (strcmp(uniformBuffer->Info.members[i].name, variable) == 0)
		{
			unsigned int offset = uniformBuffer->Info.members[i].offset;
			unsigned int size = uniformBuffer->Info.members[i].size;
			memcpy(data + offset, value, size);
		}
	}
	vmaUnmapMemory(Graphics.Allocator, uniformBuffer->Allocation);
}

void UniformBufferDestroy(UniformBuffer uniformBuffer)
{
	vmaDestroyBuffer(Graphics.Allocator, uniformBuffer->Buffer, uniformBuffer->Allocation);
	free(uniformBuffer);
}
