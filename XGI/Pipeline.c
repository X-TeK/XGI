#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Pipeline.h"
#include "Graphics.h"
#include "Swapchain.h"
#include "UniformBuffer.h"
#include "spirv/spirv_reflect.h"

static void CreatePipelineLayout(Pipeline pipeline, struct Shader shader)
{
	pipeline->StageCount = 2;
	pipeline->Stages = malloc(pipeline->StageCount * sizeof(struct PipelineStage));
	
	pipeline->Stages[0].Stage = ShaderStageVertex;
	pipeline->Stages[1].Stage = ShaderStageFragment;
	
	spvReflectCreateShaderModule(shader.VertexSPVSize, shader.VertexSPV, &pipeline->Stages[0].Module);
	spvReflectCreateShaderModule(shader.FragmentSPVSize, shader.FragmentSPV, &pipeline->Stages[1].Module);
	
	VkPushConstantRange pushConstantRange;
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
			pushConstantRange = (VkPushConstantRange)
			{
				.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				.offset = pushConstants[0].offset,
				.size = pushConstants[0].size,
			};
			break;
		}
	}
	
	int samplerCount = 0;
	int uboCount = 0;
	for (int i = 0; i < pipeline->StageCount; i++)
	{
		struct PipelineStage * stage = pipeline->Stages + i;
		spvReflectEnumerateDescriptorSets(&stage->Module, &stage->DescriptorSetLayoutCount, NULL);
		stage->DescriptorSetLayoutInfos = malloc(stage->DescriptorSetLayoutCount * sizeof(SpvReflectDescriptorSet));
		spvReflectEnumerateDescriptorSets(&stage->Module, &stage->DescriptorSetLayoutCount, &stage->DescriptorSetLayoutInfos);
		stage->DescriptorSetLayouts = malloc(stage->DescriptorSetLayoutCount * sizeof(VkDescriptorSetLayout));
		
		for (int j = 0; j < stage->DescriptorSetLayoutCount; j++)
		{
			SpvReflectDescriptorSet set = stage->DescriptorSetLayoutInfos[j];
			VkDescriptorSetLayoutBinding * layoutBindings = malloc(set.binding_count * sizeof(VkDescriptorSetLayoutBinding));
			for (int k = 0; k < set.binding_count; k++)
			{
				SpvReflectDescriptorBinding * binding = set.bindings[k];
				printf("set:%i binding:%i %s\n", binding->set, binding->binding, binding->name);
				layoutBindings[k] = (VkDescriptorSetLayoutBinding)
				{
					.binding = k,
					.descriptorCount = binding->count,
					.descriptorType = (VkDescriptorType)binding->descriptor_type,
					.stageFlags = stage->Stage,
				};
				if (binding->descriptor_type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) { samplerCount++; }
				if (binding->descriptor_type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) { uboCount++; }
			}
			VkDescriptorSetLayoutCreateInfo layoutInfo =
			{
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
				.bindingCount = set.binding_count,
				.pBindings = layoutBindings,
			};
			vkCreateDescriptorSetLayout(Graphics.Device, &layoutInfo, NULL, stage->DescriptorSetLayouts + j);
			free(layoutBindings);
		}
	}
	
	int totalLayoutsCount = 0;
	for (int i = 0; i < pipeline->StageCount; i++) { totalLayoutsCount += pipeline->Stages[i].DescriptorSetLayoutCount; }
	VkDescriptorSetLayout * totalLayouts = malloc(totalLayoutsCount * sizeof(VkDescriptorSetLayout));
	
	for (int i = 0, c = 0; i < pipeline->StageCount; i++)
	{
		for (int j = 0; j < pipeline->Stages[i].DescriptorSetLayoutCount; j++, c++)
		{
			totalLayouts[c] = pipeline->Stages[i].DescriptorSetLayouts[j];
		}
	}
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = totalLayoutsCount,
		.pSetLayouts = totalLayouts,
		.pushConstantRangeCount = pipeline->UsesPushConstant ? 1 : 0,
		.pPushConstantRanges = &pushConstantRange,
	};
	vkCreatePipelineLayout(Graphics.Device, &pipelineLayoutCreateInfo, NULL, &pipeline->Layout);
	free(totalLayouts);
	
	VkDescriptorPoolSize poolSizes[] =
	{
		{
			.descriptorCount = uboCount * Graphics.FrameResourceCount,
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		},
		{
			.descriptorCount = samplerCount * Graphics.FrameResourceCount,
			.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		}
	};
	VkDescriptorPoolCreateInfo poolInfo =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.maxSets = (uboCount + samplerCount) * Graphics.FrameResourceCount,
		.poolSizeCount = 2,
		.pPoolSizes = poolSizes,
	};
	vkCreateDescriptorPool(Graphics.Device, &poolInfo, NULL, &pipeline->DescriptorPool);
	
	for (int i = 0; i < pipeline->StageCount; i++)
	{
		struct PipelineStage * stage = pipeline->Stages + i;
		stage->DescriptorSetCount = stage->DescriptorSetLayoutCount * Graphics.FrameResourceCount;
		stage->DescriptorSets = malloc(stage->DescriptorSetCount * sizeof(VkDescriptorSet));
		for (int j = 0; j < stage->DescriptorSetLayoutCount; j++)
		{
			for (int k = 0; k < Graphics.FrameResourceCount; k++)
			{
				VkDescriptorSetAllocateInfo setInfo =
				{
					.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
					.descriptorPool = Graphics.DescriptorPool,
					.descriptorSetCount = 1,
					.pSetLayouts = stage->DescriptorSetLayouts + j,
				};
				vkAllocateDescriptorSets(Graphics.Device, &setInfo, stage->DescriptorSets + j * Graphics.FrameResourceCount + k);
			}
		}
	}
}

Pipeline PipelineCreate(struct Shader shader, VertexLayout vertexLayout)
{
	Pipeline pipeline = malloc(sizeof(struct Pipeline));
	*pipeline = (struct Pipeline){ .VertexLayout = vertexLayout };
	
	VkShaderModuleCreateInfo moduleInfo =
	{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = shader.VertexSPVSize,
		.pCode = shader.VertexSPV,
	};
	VkShaderModule vsModule;
	VkResult result = vkCreateShaderModule(Graphics.Device, &moduleInfo, NULL, &vsModule);
	
	VkPipelineShaderStageCreateInfo vsInfo =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_VERTEX_BIT,
		.module = vsModule,
		.pName = "main",
	};
	
	moduleInfo = (VkShaderModuleCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = shader.FragmentSPVSize,
		.pCode = shader.FragmentSPV,
	};
	VkShaderModule fsModule;
	result = vkCreateShaderModule(Graphics.Device, &moduleInfo, NULL, &fsModule);
	
	VkPipelineShaderStageCreateInfo fsInfo =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
		.module = fsModule,
		.pName = "main",
	};
	
	VkPipelineShaderStageCreateInfo shaderStages[2] = { vsInfo, fsInfo };
	
	VkPipelineVertexInputStateCreateInfo vertexInput =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = 1,
		.pVertexBindingDescriptions = &vertexLayout->Binding,
		.vertexAttributeDescriptionCount = vertexLayout->AttributeCount,
		.pVertexAttributeDescriptions = vertexLayout->Attributes,
	};
	
	VkPipelineInputAssemblyStateCreateInfo inputAssembly =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE,
	};
	
	VkViewport viewport =
	{
		.x = 0.0f,
		.y = 0.0f,
		.width = Swapchain.Extent.width,
		.height = Swapchain.Extent.height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};
	
	VkRect2D scissor =
	{
		.offset = { 0, 0 },
		.extent = Swapchain.Extent,
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
		.polygonMode = shader.WireFrame ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL,
		.lineWidth = 1.0f,
		.cullMode = shader.FaceCull ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_NONE,
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
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
		.blendEnable = shader.AlphaBlend ? VK_TRUE : VK_FALSE,
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
		.depthTestEnable = shader.DepthTest ? VK_TRUE : VK_FALSE,
		.depthWriteEnable = VK_TRUE,
		.depthCompareOp = VK_COMPARE_OP_LESS,
	};
	
	CreatePipelineLayout(pipeline, shader);
	VkGraphicsPipelineCreateInfo pipelineCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = 2,
		.pStages = shaderStages,
		.pVertexInputState = &vertexInput,
		.pInputAssemblyState = &inputAssembly,
		.pViewportState = &viewportState,
		.pRasterizationState = &rasterizationState,
		.pMultisampleState = &multisampleState,
		.pDepthStencilState = &depthStencilState,
		.pColorBlendState = &colorBlendState,
		.pDynamicState = NULL,
		.layout = pipeline->Layout,
		.renderPass = Swapchain.RenderPass,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1,
	};
	result = vkCreateGraphicsPipelines(Graphics.Device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, NULL, &pipeline->Instance);
	if (result != VK_SUCCESS)
	{
		printf("[Error] Unable to create graphics pipeline: %i\n", result);
		exit(-1);
	}
	
	vkDestroyShaderModule(Graphics.Device, vsModule, NULL);
	vkDestroyShaderModule(Graphics.Device, fsModule, NULL);
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
				memcpy(pipeline->PushConstantData + member.offset, value, member.size);
			}
		}
	}
}

void PipelineDestroy(Pipeline pipeline)
{
	vkDeviceWaitIdle(Graphics.Device);
	for (int i = 0; i < pipeline->StageCount; i++)
	{
		if (pipeline->Stages[i].DescriptorSetCount > 0) { free(pipeline->Stages[i].DescriptorSets); }
	}
	vkDestroyDescriptorPool(Graphics.Device, pipeline->DescriptorPool, NULL);
	vkDestroyPipelineLayout(Graphics.Device, pipeline->Layout, NULL);
	for (int i = 0; i < pipeline->StageCount; i++)
	{
		for (int j = 0; j < pipeline->Stages[i].DescriptorSetLayoutCount; j++) { vkDestroyDescriptorSetLayout(Graphics.Device, pipeline->Stages[i].DescriptorSetLayouts[j], NULL); }
		if (pipeline->Stages[i].DescriptorSetLayoutCount > 0) { free(pipeline->Stages[i].DescriptorSetLayouts); }
	}
	if (pipeline->UsesPushConstant) { free(pipeline->PushConstantData); }
	spvReflectDestroyShaderModule(&pipeline->Stages[0].Module);
	spvReflectDestroyShaderModule(&pipeline->Stages[1].Module);
	free(pipeline->Stages);
	vkDestroyPipeline(Graphics.Device, pipeline->Instance, NULL);
	free(pipeline);
}
