#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <shaderc/shaderc.h>
#include <spirv/spirv_reflect.h>
#include "Pipeline.h"
#include "Graphics.h"
#include "UniformBuffer.h"
#include "StorageBuffer.h"
#include "File.h"
#include "log.h"

ShaderData ShaderDataFromMemory(ShaderType type, unsigned long dataSize, void * data, bool precompiled)
{
	if (!precompiled)
	{
		shaderc_shader_kind shaderType;
		switch (type)
		{
			case ShaderTypeVertex: shaderType = shaderc_vertex_shader; break;
			case ShaderTypeFragment: shaderType = shaderc_fragment_shader; break;
			case ShaderTypeCompute: shaderType = shaderc_compute_shader; break;
		}
		shaderc_compilation_result_t result = shaderc_compile_into_spv(Graphics.ShaderCompiler, data, dataSize, shaderType, "shader", "main", 0);
		if (shaderc_result_get_num_errors(result) > 0)
		{
			log_fatal("Error while compiling shader:\n%s\n", shaderc_result_get_error_message(result));
			exit(1);
		}
		data = (void *)shaderc_result_get_bytes(result);
		dataSize = shaderc_result_get_length(result);
	}
	return (ShaderData)
	{
		.Type = type,
		.DataSize = dataSize,
		.Data = data,
	};
}

ShaderData ShaderDataFromFile(ShaderType type, const char * file, bool precompiled)
{
	if (!precompiled)
	{
		File shader = FileOpen(file, FileModeRead);
		unsigned long size = FileGetSize(shader);
		char * text = malloc(size);
		FileRead(shader, 0, size, text);
		FileClose(shader);
		shaderc_shader_kind shaderType;
		switch (type)
		{
			case ShaderTypeVertex: shaderType = shaderc_vertex_shader; break;
			case ShaderTypeFragment: shaderType = shaderc_fragment_shader; break;
			case ShaderTypeCompute: shaderType = shaderc_compute_shader; break;
		}
		shaderc_compilation_result_t result = shaderc_compile_into_spv(Graphics.ShaderCompiler, text, size, shaderType, file, "main", 0);
		if (shaderc_result_get_num_errors(result) > 0)
		{
			log_fatal("Error while compiling shader:\n%s\n", shaderc_result_get_error_message(result));
			exit(1);
		}
		void * data = (void *)shaderc_result_get_bytes(result);
		unsigned long dataSize = shaderc_result_get_length(result);
		return (ShaderData)
		{
			.Type = type,
			.DataSize = dataSize,
			.Data = data,
		};
	}
	File spv = FileOpen(file, FileModeReadBinary);
	unsigned long size = FileGetSize(spv);
	void * data = malloc(size);
	FileRead(spv, 0, size, data);
	FileClose(spv);
	return (ShaderData)
	{
		.Type = type,
		.DataSize = size,
		.Data = data,
	};
}

static void CreateReflectModules(Pipeline pipeline, PipelineConfigure config)
{
	pipeline->StageCount = config.ShaderCount;
	pipeline->Stages = malloc(pipeline->StageCount * sizeof(struct PipelineStage));
	
	for (int i = 0; i < pipeline->StageCount; i++)
	{
		pipeline->Stages[i].ShaderType = config.Shaders[i].Type;
		spvReflectCreateShaderModule(config.Shaders[i].DataSize, config.Shaders[i].Data, &pipeline->Stages[i].Module);
	}
}

static VkPushConstantRange GetPushConstantRange(Pipeline pipeline)
{
	VkPushConstantRange pushConstantRange = { 0 };
	for (int i = 0; i < pipeline->StageCount; i++)
	{
		unsigned int pushConstantCount;
		spvReflectEnumeratePushConstantBlocks(&pipeline->Stages[i].Module, &pushConstantCount, NULL);
		SpvReflectBlockVariable * pushConstants = malloc(pushConstantCount * sizeof(SpvReflectBlockVariable));
		spvReflectEnumeratePushConstantBlocks(&pipeline->Stages[i].Module, &pushConstantCount, &pushConstants);
		if (pushConstantCount > 0)
		{
			pipeline->UsesPushConstant = true;
			pipeline->PushConstantInfo = pushConstants[0];
			pipeline->PushConstantData = malloc(pushConstants[0].size);
			pipeline->PushConstantSize = pushConstants[0].size;
			VkShaderStageFlags stages = 0;
			for (int j = 0; j < pipeline->StageCount; j++) { stages |= pipeline->Stages[j].ShaderType; }
			return (VkPushConstantRange)
			{
				.stageFlags = stages,
				.offset = pushConstants[0].offset,
				.size = pushConstants[0].size,
			};
		}
	}
	return pushConstantRange;
}

static void CreateDescriptorLayout(Pipeline pipeline, int * uboCount, int * samplerCount, int * storageCount)
{
	unsigned int bindingCount = 0;
	pipeline->UsesDescriptors = false;
	for (int i = 0; i < pipeline->StageCount; i++)
	{
		struct PipelineStage * stage = pipeline->Stages + i;
		unsigned int setCount;
		spvReflectEnumerateDescriptorSets(&stage->Module, &setCount, NULL);
		SpvReflectDescriptorSet * sets = malloc(setCount * sizeof(SpvReflectDescriptorSet));
		spvReflectEnumerateDescriptorSets(&stage->Module, &setCount, &sets);
		
		stage->BindingCount = 0;
		if (setCount > 0)
		{
			pipeline->UsesDescriptors = true;
			stage->DescriptorInfo = sets[0];
			stage->BindingCount = stage->DescriptorInfo.binding_count;
			bindingCount += stage->DescriptorInfo.binding_count;
		}
	}
	if (pipeline->UsesDescriptors)
	{
		VkDescriptorSetLayoutBinding * layoutBindings = malloc(bindingCount * sizeof(VkDescriptorSetLayoutBinding));
		for (int i = 0, c = 0; i < pipeline->StageCount; i++)
		{
			struct PipelineStage * stage = pipeline->Stages + i;
			unsigned int setCount;
			spvReflectEnumerateDescriptorSets(&stage->Module, &setCount, NULL);
			SpvReflectDescriptorSet * sets = malloc(setCount * sizeof(SpvReflectDescriptorSet));
			spvReflectEnumerateDescriptorSets(&stage->Module, &setCount, &sets);

			if (setCount > 0)
			{
				for (int j = 0; j < stage->DescriptorInfo.binding_count; j++, c++)
				{
					SpvReflectDescriptorBinding * binding = stage->DescriptorInfo.bindings[j];
					layoutBindings[c] = (VkDescriptorSetLayoutBinding)
					{
						.binding = binding->binding,
						.descriptorCount = binding->count,
						.descriptorType = (VkDescriptorType)binding->descriptor_type,
						.stageFlags = stage->ShaderType,
					};
					if (binding->descriptor_type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) { (*samplerCount) += binding->count; }
					if (binding->descriptor_type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) { (*uboCount) += binding->count; }
					if (binding->descriptor_type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER) { (*storageCount) += binding->count; }
				}
			}
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo =
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.bindingCount = bindingCount,
			.pBindings = layoutBindings,
		};
		vkCreateDescriptorSetLayout(Graphics.Device, &layoutInfo, NULL, &pipeline->DescriptorLayout);
		free(layoutBindings);
	}
}

static void CreateDescriptorPool(Pipeline pipeline, int uboCount, int samplerCount, int storageCount)
{
	if (pipeline->UsesDescriptors)
	{
		VkDescriptorPoolSize poolSizes[2];
		int i = 0;
		if (uboCount > 0)
		{
			poolSizes[i] = (VkDescriptorPoolSize)
			{
				.descriptorCount = uboCount * Graphics.FrameResourceCount,
				.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			};
			i++;
		}
		if (samplerCount > 0)
		{
			poolSizes[i] = (VkDescriptorPoolSize)
			{
				.descriptorCount = samplerCount * Graphics.FrameResourceCount,
				.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			};
			i++;
		}
		if (storageCount > 0)
		{
			poolSizes[i] = (VkDescriptorPoolSize)
			{
				.descriptorCount = storageCount * Graphics.FrameResourceCount,
				.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			};
			i++;
		}
		VkDescriptorPoolCreateInfo poolInfo =
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.maxSets = Graphics.FrameResourceCount,
			.poolSizeCount = i,
			.pPoolSizes = poolSizes,
		};
		vkCreateDescriptorPool(Graphics.Device, &poolInfo, NULL, &pipeline->DescriptorPool);
	}
}

static void CreateDescriptorSets(Pipeline pipeline)
{
	if (pipeline->UsesDescriptors)
	{
		pipeline->DescriptorSet = malloc(Graphics.FrameResourceCount * sizeof(VkDescriptorSet));
		for (int i = 0; i < Graphics.FrameResourceCount; i++)
		{
			VkDescriptorSetAllocateInfo allocateInfo =
			{
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
				.descriptorPool = pipeline->DescriptorPool,
				.descriptorSetCount = 1,
				.pSetLayouts = &pipeline->DescriptorLayout,
			};
			vkAllocateDescriptorSets(Graphics.Device, &allocateInfo, pipeline->DescriptorSet + i);
		}
	}
}

static void CreatePipelineLayout(Pipeline pipeline, VkPushConstantRange pushConstantRange)
{
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = pipeline->UsesDescriptors ? 1 : 0,
		.pSetLayouts = &pipeline->DescriptorLayout,
		.pushConstantRangeCount = pipeline->UsesPushConstant ? 1 : 0,
		.pPushConstantRanges = &pushConstantRange,
	};
	vkCreatePipelineLayout(Graphics.Device, &pipelineLayoutCreateInfo, NULL, &pipeline->Layout);
}

static void CreateLayout(Pipeline pipeline, PipelineConfigure config)
{
	CreateReflectModules(pipeline, config);
	VkPushConstantRange pushConstantRange = GetPushConstantRange(pipeline);
	int samplerCount = 0, uboCount = 0, storageCount = 0;
	CreateDescriptorLayout(pipeline, &uboCount, &samplerCount, &storageCount);
	CreateDescriptorPool(pipeline, uboCount, samplerCount, storageCount);
	CreatePipelineLayout(pipeline, pushConstantRange);
	CreateDescriptorSets(pipeline);
}

Pipeline PipelineCreate(PipelineConfigure config)
{
	Pipeline pipeline = malloc(sizeof(struct Pipeline));
	*pipeline = (struct Pipeline)
	{
		.VertexLayout = config.VertexLayout,
		.LineWidth = config.LineWidth,
		.FrontStencilReference = config.FrontStencil.Reference,
		.BackStencilReference = config.BackStencil.Reference,
	};
	
	VkPipelineShaderStageCreateInfo shaderInfos[5];
	VkShaderModule modules[5];
	for (int i = 0; i < config.ShaderCount; i++)
	{
		VkShaderModuleCreateInfo moduleInfo =
		{
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = config.Shaders[i].DataSize,
			.pCode = config.Shaders[i].Data,
		};
		vkCreateShaderModule(Graphics.Device, &moduleInfo, NULL, modules + i);
		
		shaderInfos[i] = (VkPipelineShaderStageCreateInfo)
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = (VkShaderStageFlagBits)config.Shaders[i].Type,
			.module = modules[i],
			.pName = "main",
		};
	}
	
	VkPipelineVertexInputStateCreateInfo vertexInput =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = 1,
		.pVertexBindingDescriptions = &config.VertexLayout->Binding,
		.vertexAttributeDescriptionCount = config.VertexLayout->AttributeCount,
		.pVertexAttributeDescriptions = config.VertexLayout->Attributes,
	};
	
	VkPipelineInputAssemblyStateCreateInfo inputAssembly =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = (VkPrimitiveTopology)config.Primitive,
		.primitiveRestartEnable = VK_FALSE,
	};
	
	VkViewport viewport =
	{
		.x = 0.0f,
		.y = 0.0f,
		.width = Graphics.Swapchain.Extent.width,
		.height = Graphics.Swapchain.Extent.height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};
	
	VkRect2D scissor =
	{
		.offset = { 0, 0 },
		.extent = Graphics.Swapchain.Extent,
	};

	VkPipelineViewportStateCreateInfo viewportState =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.pViewports = &viewport,
		.scissorCount = 1,
		.pScissors = &scissor,
	};
	
	VkPipelineRasterizationStateCreateInfo rasterizationState =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = (VkPolygonMode)config.PolygonMode,
		.lineWidth = config.LineWidth,
		.cullMode = (VkCullModeFlagBits)config.CullMode,
		.frontFace = config.CullClockwise ? VK_FRONT_FACE_CLOCKWISE : VK_FRONT_FACE_COUNTER_CLOCKWISE,
		.depthBiasEnable = VK_FALSE,
	};
	
	VkPipelineMultisampleStateCreateInfo multisampleState =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.sampleShadingEnable = VK_FALSE,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
	};
	
	VkPipelineColorBlendAttachmentState colorBlendAttachmentState =
	{
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
		.blendEnable = config.AlphaBlend ? VK_TRUE : VK_FALSE,
		.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
		.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		.colorBlendOp = VK_BLEND_OP_ADD,
		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		.alphaBlendOp = VK_BLEND_OP_ADD,
	};
	
	VkPipelineColorBlendStateCreateInfo colorBlendState =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = VK_FALSE,
		.attachmentCount = 1,
		.pAttachments = &colorBlendAttachmentState,
	};
	
	VkPipelineDepthStencilStateCreateInfo depthStencilState =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.depthTestEnable = config.DepthTest ? VK_TRUE : VK_FALSE,
		.depthWriteEnable = config.DepthWrite ? VK_TRUE : VK_FALSE,
		.depthCompareOp = (VkCompareOp)config.DepthCompare,
		.depthBoundsTestEnable = VK_FALSE,
		.stencilTestEnable = config.StencilTest ? VK_TRUE : VK_FALSE,
		.front =
		{
			.compareOp = (VkCompareOp)config.FrontStencil.Compare,
			.passOp = (VkStencilOp)config.FrontStencil.Pass,
			.failOp = (VkStencilOp)config.FrontStencil.Fail,
			.depthFailOp = (VkStencilOp)config.FrontStencil.DepthFail,
			.compareMask = 0xffffffff,
			.reference = config.FrontStencil.Reference,
			.writeMask = 0xffffffff,
		},
		.back =
		{
			.compareOp = (VkCompareOp)config.BackStencil.Compare,
			.passOp = (VkStencilOp)config.BackStencil.Pass,
			.failOp = (VkStencilOp)config.BackStencil.Fail,
			.depthFailOp = (VkStencilOp)config.BackStencil.DepthFail,
			.compareMask = 0xffffffff,
			.reference = config.BackStencil.Reference,
			.writeMask = 0xffffffff,
		},
	};
	
	VkDynamicState dynamicStates[] =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
		VK_DYNAMIC_STATE_LINE_WIDTH,
		VK_DYNAMIC_STATE_STENCIL_REFERENCE,
	};
	VkPipelineDynamicStateCreateInfo dynamicState =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = 4,
		.pDynamicStates = dynamicStates,
	};
	
	CreateLayout(pipeline, config);
	VkGraphicsPipelineCreateInfo pipelineCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = config.ShaderCount,
		.pStages = shaderInfos,
		.pVertexInputState = &vertexInput,
		.pInputAssemblyState = &inputAssembly,
		.pViewportState = &viewportState,
		.pRasterizationState = &rasterizationState,
		.pMultisampleState = &multisampleState,
		.pDepthStencilState = &depthStencilState,
		.pColorBlendState = &colorBlendState,
		.pDynamicState = &dynamicState,
		.layout = pipeline->Layout,
		.renderPass = Graphics.RenderPass,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1,
	};
	VkResult result = vkCreateGraphicsPipelines(Graphics.Device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, NULL, &pipeline->Instance);
	if (result != VK_SUCCESS)
	{
		log_fatal("Unable to create graphics pipeline: %i\n", result);
		exit(1);
	}
	
	for (int i = 0; i < config.ShaderCount; i++)
	{
		vkDestroyShaderModule(Graphics.Device, modules[i], NULL);
	}
	return pipeline;
}

void PipelineSetPushConstant(Pipeline pipeline, const char * variable, void * value)
{
	if (pipeline->UsesPushConstant)
	{
		for (int i = 0; i < pipeline->PushConstantInfo.member_count; i++)
		{
			SpvReflectBlockVariable member = pipeline->PushConstantInfo.members[i];
			if (strcmp(member.name, variable) == 0)
			{
				memcpy((unsigned char *)pipeline->PushConstantData + member.offset, value, member.size);
			}
		}
	}
}

void PipelineSetUniform(Pipeline pipeline, int binding, int arrayIndex, struct UniformBuffer * uniform)
{
	if (pipeline->UsesDescriptors)
	{
		for (int i = 0; i < Graphics.FrameResourceCount; i++)
		{
			VkDescriptorBufferInfo * bufferInfo = malloc(sizeof(VkDescriptorBufferInfo));
			*bufferInfo = (VkDescriptorBufferInfo)
			{
				.buffer = uniform->Buffer,
				.offset = 0,
				.range = uniform->Size,
			};
			
			VkWriteDescriptorSet * writeInfo = malloc(sizeof(VkWriteDescriptorSet));
			*writeInfo = (VkWriteDescriptorSet)
			{
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.dstArrayElement = arrayIndex,
				.dstBinding = binding,
				.dstSet = pipeline->DescriptorSet[i],
				.pBufferInfo = bufferInfo,
			};
			ListPush(Graphics.FrameResources[i].UpdateDescriptorQueue, writeInfo);
		}
	}
}

void PipelineSetSampler(Pipeline pipeline, int binding, int arrayIndex, Texture texture)
{
	if (pipeline->UsesDescriptors)
	{
		for (int i = 0; i < Graphics.FrameResourceCount; i++)
		{
			VkDescriptorImageInfo * imageInfo = malloc(sizeof(VkDescriptorImageInfo));
			*imageInfo = (VkDescriptorImageInfo)
			{
				.imageLayout = VK_IMAGE_LAYOUT_GENERAL,
				.sampler = texture->Sampler,
				.imageView = texture->ImageView,
			};
			VkWriteDescriptorSet * writeInfo = malloc(sizeof(VkWriteDescriptorSet));
			*writeInfo = (VkWriteDescriptorSet)
			{
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.dstArrayElement = arrayIndex,
				.dstBinding = binding,
				.dstSet = pipeline->DescriptorSet[i],
				.pImageInfo = imageInfo,
			};
			ListPush(Graphics.FrameResources[i].UpdateDescriptorQueue, writeInfo);
		}
	}
}

void PipelineSetStorageBuffer(Pipeline pipeline, int binding, int arrayIndex, StorageBuffer storage)
{
	if (pipeline->UsesDescriptors)
	{
		for (int i = 0; i < Graphics.FrameResourceCount; i++)
		{
			VkDescriptorBufferInfo * bufferInfo = malloc(sizeof(VkDescriptorBufferInfo));
			*bufferInfo = (VkDescriptorBufferInfo)
			{
				.buffer = storage->Buffer,
				.offset = 0,
				.range = storage->Size,
			};
			
			VkWriteDescriptorSet * writeInfo = malloc(sizeof(VkWriteDescriptorSet));
			*writeInfo = (VkWriteDescriptorSet)
			{
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.dstArrayElement = arrayIndex,
				.dstBinding = binding,
				.dstSet = pipeline->DescriptorSet[i],
				.pBufferInfo = bufferInfo,
			};
			ListPush(Graphics.FrameResources[i].UpdateDescriptorQueue, writeInfo);
		}
	}
}

void PipelineSetLineWidth(Pipeline pipeline, Scalar lineWidth)
{
	pipeline->LineWidth = lineWidth;
}

void PipelineSetFrontStencilReference(Pipeline pipeline, unsigned int reference)
{
	pipeline->FrontStencilReference = reference;
}
void PipelineSetBackStencilReference(Pipeline pipeline, unsigned int reference)
{
	pipeline->BackStencilReference = reference;
}

void PipelineQueueDestroy(Pipeline pipeline)
{
	ListPush(Graphics.FrameResources[Graphics.FrameIndex].DestroyPipelineQueue, pipeline);
}

void PipelineDestroy(Pipeline pipeline)
{
	vkDeviceWaitIdle(Graphics.Device);
	vkDestroyPipelineLayout(Graphics.Device, pipeline->Layout, NULL);
	if (pipeline->UsesDescriptors)
	{
		free(pipeline->DescriptorSet);
		vkDestroyDescriptorSetLayout(Graphics.Device, pipeline->DescriptorLayout, NULL);
		vkDestroyDescriptorPool(Graphics.Device, pipeline->DescriptorPool, NULL);
	}
	if (pipeline->UsesPushConstant) { free(pipeline->PushConstantData); }
	for (int i = 0; i < pipeline->StageCount; i++) { spvReflectDestroyShaderModule(&pipeline->Stages[i].Module); }
	free(pipeline->Stages);
	vkDestroyPipeline(Graphics.Device, pipeline->Instance, NULL);
	free(pipeline);
}

ComputePipeline ComputePipelineCreate(ShaderData shader)
{
	ComputePipeline pipeline = malloc(sizeof(struct Pipeline));
	*pipeline = (struct Pipeline){ 0 };
	
	PipelineConfigure config =
	{
		.ShaderCount = 1,
		.Shaders = { shader },
	};
	CreateLayout(pipeline, config);
	
	VkShaderModule module;
	VkShaderModuleCreateInfo moduleInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = config.Shaders[0].DataSize,
		.pCode = config.Shaders[0].Data,
	};
	vkCreateShaderModule(Graphics.Device, &moduleInfo, NULL, &module);
	
	VkPipelineShaderStageCreateInfo stageInfo =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_COMPUTE_BIT,
		.module = module,
		.pName = "main",
	};
	
	VkComputePipelineCreateInfo pipelineInfo =
	{
		.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = 0,
		.stage = stageInfo,
		.layout = pipeline->Layout,
	};
	vkCreateComputePipelines(Graphics.Device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &pipeline->Instance);
	vkDestroyShaderModule(Graphics.Device, module, NULL);
	
	return pipeline;
}

void ComputePipelineDestroy(ComputePipeline pipeline)
{
	PipelineDestroy(pipeline);
}
