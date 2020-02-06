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

typedef struct PipelineConfigure
{
	VertexLayout VertexLayout;
	int ShaderCount;
	struct
	{
		ShaderType Type;
		bool LoadFromFile;
		const char * File;
		bool Precompiled;
		unsigned long DataSize;
		void * Data;
	} Shaders[5];
	bool WireFrame;
	bool FaceCull;
	bool AlphaBlend;
	bool DepthTest;
} PipelineConfigure;

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
		int BindingCount;
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

Pipeline PipelineCreate(PipelineConfigure config);
void PipelineSetPushConstant(Pipeline pipeline, const char * variableName, void * value);
void PipelineSetUniform(Pipeline pipeline, int binding, int arrayIndex, struct UniformBuffer * uniform);
void PipelineSetSampler(Pipeline pipeline, int binding, int arrayIndex, Texture texture);
void PipelineDestroy(Pipeline pipeline);

#endif
