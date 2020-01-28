#ifndef Pipeline_h
#define Pipeline_h

#include <vulkan/vulkan.h>
#include <stdbool.h>
#include "spirv/spirv_reflect.h"
#include "VertexBuffer.h"

typedef struct Shader
{
	unsigned int VertexSPVSize;
	unsigned int * VertexSPV;
	unsigned int FragmentSPVSize;
	unsigned int * FragmentSPV;
	bool WireFrame;
	bool FaceCull;
	bool AlphaBlend;
	bool DepthTest;
} Shader;

typedef struct Pipeline
{
	VkPipeline Instance;
	VkPipelineLayout Layout;
	VertexLayout VertexLayout;
	SpvReflectShaderModule VSModule;
	SpvReflectShaderModule FSModule;
	bool UsesPushConstant;
	SpvReflectBlockVariable PushConstantInfo;
	void * PushConstantData;
	unsigned int PushConstantSize;
	unsigned int VSDescriptorSetLayoutCount;
	SpvReflectDescriptorSet * VSDescriptorSetLayoutInfos;
	VkDescriptorSetLayout * VSDescriptorSetLayouts;
	unsigned int FSDescriptorSetLayoutCount;
	SpvReflectDescriptorSet * FSDescriptorSetLayoutInfos;
	VkDescriptorSetLayout * FSDescriptorSetLayouts;
} * Pipeline;

Pipeline PipelineCreate(Shader shader, VertexLayout vertexLayout);
void PipelineSetPushConstant(Pipeline pipeline, const char * variableName, void * value);
void PipelineDestroy(Pipeline pipeline);

#endif
