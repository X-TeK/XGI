#include <stdio.h>
#include <stdlib.h>
#include "UniformBuffer.h"
#include "Graphics.h"

static UniformBuffer_T Create(uniform_t uniform)
{
	UniformBuffer_T uniformBuffer = malloc(sizeof(struct UniformBuffer));
	*uniformBuffer = (struct UniformBuffer){ 0 };
	
	VkBufferCreateInfo bufferInfo =
	{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = sizeof(struct uniform),
		.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
	};
	VmaAllocationCreateInfo allocationInfo =
	{
		.usage = VMA_MEMORY_USAGE_CPU_ONLY,
	};
	VmaAllocationInfo info;
	vmaCreateBuffer(Graphics.Allocator, &bufferInfo, &allocationInfo, &uniformBuffer->Buffer, &uniformBuffer->Allocation, &info);
	UniformBuffer.Update(uniformBuffer, uniform);
	
	uniformBuffer->DescriptorSets = malloc(Graphics.FrameResourceCount * sizeof(VkDescriptorSet));
	for (int i = 0; i < Graphics.FrameResourceCount; i++)
	{
		VkDescriptorSetAllocateInfo setInfo =
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = Graphics.DescriptorPool,
			.descriptorSetCount = 1,
			.pSetLayouts = &Graphics.DescriptorSetLayout0,
		};
		vkAllocateDescriptorSets(Graphics.Device, &setInfo, uniformBuffer->DescriptorSets + i);
		
		VkDescriptorBufferInfo bufferInfo =
		{
			.buffer = uniformBuffer->Buffer,
			.offset = 0,
			.range = VK_WHOLE_SIZE,
		};
		VkWriteDescriptorSet writeInfo =
		{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.dstArrayElement = 0,
			.dstBinding = 0,
			.dstSet = uniformBuffer->DescriptorSets[i],
			.pBufferInfo = &bufferInfo,
		};
		vkUpdateDescriptorSets(Graphics.Device, 1, &writeInfo, 0, NULL);
	}
	
	return uniformBuffer;
}

static void Update(UniformBuffer_T uniformBuffer, uniform_t uniform)
{
	void * _Data;
	vmaMapMemory(Graphics.Allocator, uniformBuffer->Allocation, &_Data);
	*((uniform_t *)_Data) = uniform;
	vmaUnmapMemory(Graphics.Allocator, uniformBuffer->Allocation);
}

static void Destroy(UniformBuffer_T uniformBuffer)
{
	vkFreeDescriptorSets(Graphics.Device, Graphics.DescriptorPool, Graphics.FrameResourceCount, uniformBuffer->DescriptorSets);
	free(uniformBuffer->DescriptorSets);
	vmaDestroyBuffer(Graphics.Allocator, uniformBuffer->Buffer, uniformBuffer->Allocation);
}

struct UniformBufferInterface UniformBuffer =
{
	.Create = Create,
	.Update = Update,
	.Destroy = Destroy,
};
