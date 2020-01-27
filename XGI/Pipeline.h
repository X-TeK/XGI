#ifndef Pipeline_h
#define Pipeline_h

#include <vulkan/vulkan.h>
#include <stdbool.h>
#include "spirv/spirv_reflect.h"

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
	
	SpvReflectShaderModule VSModule;
	SpvReflectShaderModule FSModule;
	
	bool UsesPushConstants;
	SpvReflectBlockVariable PushConstants;
	void * PushConstantData;
	unsigned int PushConstantSize;
	
	unsigned int VSDescriptorCount;
	SpvReflectDescriptorSet * VSDescriptorSets;
	unsigned int FSDescriptorCount;
	SpvReflectDescriptorSet * FSDescriptorSets;
} * Pipeline;

Pipeline PipelineCreate(Shader shader);
void PipelineSetPushConstant(Pipeline pipeline, const char * variableName, void * value);
void PipelineDestroy(Pipeline pipeline);

#endif
