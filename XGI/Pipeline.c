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
	VkPushConstantRange pushConstantRange;
	
	spvReflectCreateShaderModule(shader.VertexSPVSize, shader.VertexSPV, &pipeline->VSModule);
	spvReflectCreateShaderModule(shader.FragmentSPVSize, shader.FragmentSPV, &pipeline->FSModule);
	
	unsigned int pushConstantCount;
	spvReflectEnumeratePushConstantBlocks(&pipeline->VSModule, &pushConstantCount, NULL);
	SpvReflectBlockVariable * pushConstants = malloc(pushConstantCount * sizeof(SpvReflectBlockVariable));
	spvReflectEnumeratePushConstantBlocks(&pipeline->VSModule, &pushConstantCount, &pushConstants);
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
	}
	
	unsigned int vsSetCount;
	spvReflectEnumerateDescriptorSets(&pipeline->VSModule, &vsSetCount, NULL);
	SpvReflectDescriptorSet * vsSets = malloc(vsSetCount * sizeof(SpvReflectDescriptorSet));
	spvReflectEnumerateDescriptorSets(&pipeline->VSModule, &vsSetCount, &vsSets);
	unsigned int fsSetCount;
	spvReflectEnumerateDescriptorSets(&pipeline->FSModule, &fsSetCount, NULL);
	SpvReflectDescriptorSet * fsSets = malloc(fsSetCount * sizeof(SpvReflectDescriptorSet));
	spvReflectEnumerateDescriptorSets(&pipeline->FSModule, &fsSetCount, &fsSets);
	
	pipeline->DescriptorSetLayoutCount = vsSetCount + fsSetCount;
	pipeline->DescriptorSetLayoutInfos = malloc(pipeline->DescriptorSetLayoutCount * sizeof(SpvReflectDescriptorSet));
	memcpy(pipeline->DescriptorSetLayoutInfos, vsSets, vsSetCount * sizeof(SpvReflectDescriptorSet));
	memcpy(pipeline->DescriptorSetLayoutInfos + vsSetCount, fsSets, fsSetCount * sizeof(SpvReflectDescriptorSet));
	pipeline->DescriptorSetLayouts = malloc(pipeline->DescriptorSetLayoutCount * sizeof(VkDescriptorSetLayout));
	
	for (int i = 0; i < vsSetCount; i++)
	{
		SpvReflectDescriptorSet set = vsSets[i];
		VkDescriptorSetLayoutBinding * layoutBindings = malloc(set.binding_count * sizeof(VkDescriptorSetLayoutBinding));
		for (int j = 0; j < set.binding_count; j++)
		{
			SpvReflectDescriptorBinding * binding = set.bindings[j];
			printf("set:%i binding:%i %s\n", binding->set, binding->binding, binding->name);
			layoutBindings[j] = (VkDescriptorSetLayoutBinding)
			{
				.binding = j,
				.descriptorCount = binding->count,
				.descriptorType = (VkDescriptorType)binding->descriptor_type,
				.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
			};
		}
		VkDescriptorSetLayoutCreateInfo layoutInfo =
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.bindingCount = set.binding_count,
			.pBindings = layoutBindings,
		};
		vkCreateDescriptorSetLayout(Graphics.Device, &layoutInfo, NULL, pipeline->DescriptorSetLayouts + i);
		free(layoutBindings);
	}
	
	for (int i = 0; i < fsSetCount; i++)
	{
		SpvReflectDescriptorSet set = fsSets[i];
		VkDescriptorSetLayoutBinding * layoutBindings = malloc(set.binding_count * sizeof(VkDescriptorSetLayoutBinding));
		for (int j = 0; j < set.binding_count; j++)
		{
			SpvReflectDescriptorBinding * binding = set.bindings[j];
			printf("set:%i binding:%i %s\n", binding->set, binding->binding, binding->name);
			layoutBindings[j] = (VkDescriptorSetLayoutBinding)
			{
				.binding = j,
				.descriptorCount = binding->count,
				.descriptorType = (VkDescriptorType)binding->descriptor_type,
				.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
			};
		}
		VkDescriptorSetLayoutCreateInfo layoutInfo =
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.bindingCount = set.binding_count,
			.pBindings = layoutBindings,
		};
		vkCreateDescriptorSetLayout(Graphics.Device, &layoutInfo, NULL, pipeline->DescriptorSetLayouts + vsSetCount + i);
		free(layoutBindings);
	}

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = pipeline->DescriptorSetLayoutCount,
		.pSetLayouts = pipeline->DescriptorSetLayouts,
		.pushConstantRangeCount = pushConstantCount,
		.pPushConstantRanges = &pushConstantRange,
	};
	
	VkResult result = vkCreatePipelineLayout(Graphics.Device, &pipelineLayoutCreateInfo, NULL, &pipeline->Layout);
	if (result != VK_SUCCESS)
	{
		printf("[Error] Unable to create pipeline layout: %i\n", result);
		exit(-1);
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
	if (result != VK_SUCCESS)
	{
		printf("[Error] Unable to create vertex shader: %i\n", result);
		exit(-1);
	}
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
	if (result != VK_SUCCESS)
	{
		printf("[Error] Unable to create fragment shader: %i\n", result);
		exit(-1);
	}
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
	vkDestroyPipelineLayout(Graphics.Device, pipeline->Layout, NULL);
	vkDestroyPipeline(Graphics.Device, pipeline->Instance, NULL);
	if (pipeline->UsesPushConstant) { free(pipeline->PushConstantData); }
	for (int i = 0; i < pipeline->DescriptorSetLayoutCount; i++) { vkDestroyDescriptorSetLayout(Graphics.Device, pipeline->DescriptorSetLayouts[i], NULL); }
	if (pipeline->DescriptorSetLayoutCount > 0) { free(pipeline->DescriptorSetLayouts); free(pipeline->DescriptorSetLayoutInfos); }
	spvReflectDestroyShaderModule(&pipeline->VSModule);
	spvReflectDestroyShaderModule(&pipeline->FSModule);
	free(pipeline);
}
