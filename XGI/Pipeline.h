#ifndef Pipeline_h
#define Pipeline_h

#include <vulkan/vulkan.h>
#include <stdbool.h>
#include "spirv/spirv_reflect.h"
#include "VertexBuffer.h"

typedef enum PipelineUniformType
{
	PipelineVariableTypeSampler = SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
	PipelineVariableTypeUniform = SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
} PipelineUniformType;

typedef enum ShaderStage
{
	ShaderStageVertex = VK_SHADER_STAGE_VERTEX_BIT,
	ShaderStageFragment = VK_SHADER_STAGE_FRAGMENT_BIT,
} ShaderStage;

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
	int StageCount;
	struct PipelineStage
	{
		ShaderStage Stage;
		SpvReflectShaderModule Module;
		unsigned int DescriptorSetLayoutCount;
		SpvReflectDescriptorSet * DescriptorSetLayoutInfos;
		VkDescriptorSetLayout * DescriptorSetLayouts;
		unsigned int DescriptorSetCount;
		VkDescriptorSet * DescriptorSets;
	} * Stages;
	bool UsesPushConstant;
	SpvReflectBlockVariable PushConstantInfo;
	void * PushConstantData;
	unsigned int PushConstantSize;
	VkDescriptorPool DescriptorPool;
} * Pipeline;

Pipeline PipelineCreate(Shader shader, VertexLayout vertexLayout);
void PipelineSetPushConstant(Pipeline pipeline, const char * variableName, void * value);
// PipelineSetUniform(Pipeline pipeline, ShaderStage stage, const char * binding, const char * variable, void * value);
// PipelineSetSampler(PIpeline pipeline, ShaderStage stage, const char * binding, Texture texture);
void PipelineDestroy(Pipeline pipeline);

#endif
