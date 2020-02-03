#ifndef Pipeline_h
#define Pipeline_h

#include <vulkan/vulkan.h>
#include <stdbool.h>
#include "spirv/spirv_reflect.h"
#include "VertexBuffer.h"
#include "UniformBuffer.h"
#include "Texture.h"

struct UniformBuffer;

typedef enum PipelineUniformType
{
	PipelineVariableTypeSampler = SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
	PipelineVariableTypeUniform = SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
} PipelineUniformType;

typedef enum ShaderType
{
	ShaderTypeVertex = VK_SHADER_STAGE_VERTEX_BIT,
	ShaderTypeFragment = VK_SHADER_STAGE_FRAGMENT_BIT,
} ShaderType;

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
		ShaderType ShaderType;
		SpvReflectShaderModule Module;
		SpvReflectDescriptorSet DescriptorInfo;
	} * Stages;
	
	bool UsesDescriptors;
	VkDescriptorSetLayout DescriptorLayout;
	VkDescriptorSet * DescriptorSet;
	
	bool UsesPushConstant;
	SpvReflectBlockVariable PushConstantInfo;
	void * PushConstantData;
	unsigned int PushConstantSize;
} * Pipeline;

Pipeline PipelineCreate(Shader shader, VertexLayout vertexLayout);
void PipelineSetPushConstant(Pipeline pipeline, const char * variableName, void * value);
void PipelineSetUniform(Pipeline pipeline, int binding, struct UniformBuffer * uniform);
void PipelineSetSampler(Pipeline pipeline, int binding, Texture texture);
void PipelineDestroy(Pipeline pipeline);

#endif
