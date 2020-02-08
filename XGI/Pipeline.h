#ifndef Pipeline_h
#define Pipeline_h

#include <vulkan/vulkan.h>
#include <stdbool.h>
#include "spirv/spirv_reflect.h"
#include "VertexBuffer.h"
#include "UniformBuffer.h"
#include "Texture.h"

struct UniformBuffer;

typedef enum ShaderType
{
	ShaderTypeVertex = VK_SHADER_STAGE_VERTEX_BIT,
	ShaderTypeFragment = VK_SHADER_STAGE_FRAGMENT_BIT,
} ShaderType;

typedef enum PolygonMode
{
	PolygonModeFill = VK_POLYGON_MODE_FILL,
	PolygonModeLine = VK_POLYGON_MODE_LINE,
	PolygonModePoint = VK_POLYGON_MODE_POINT,
} PolygonMode;

typedef enum VertexPrimitive
{
	VertexPrimitiveTriangleList = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	VertexPrimitiveTriangleFan = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,
	VertexPrimitiveTriangleStrip = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
	VertexPrimitiveLineList = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
	VertexPrimitiveLineStrip = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
	VertexPrimitivePointList = VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
} VertexPrimitive;

typedef enum CullMode
{
	CullModeNone = VK_CULL_MODE_NONE,
	CullModeFront = VK_CULL_MODE_FRONT_BIT,
	CullModeBack = VK_CULL_MODE_BACK_BIT,
	CullModeFrontAndBack = VK_CULL_MODE_FRONT_AND_BACK,
} CullMode;

typedef enum CompareOperation
{
	CompareOperationAlways = VK_COMPARE_OP_ALWAYS,
	CompareOperationNever = VK_COMPARE_OP_NEVER,
	CompareOperationEqual = VK_COMPARE_OP_EQUAL,
	CompareOperationLess = VK_COMPARE_OP_LESS,
	CompareOperationGreater = VK_COMPARE_OP_GREATER,
	CompareOperationLessOrEqual = VK_COMPARE_OP_LESS_OR_EQUAL,
	CompareOperationGreaterOrEqual = VK_COMPARE_OP_GREATER_OR_EQUAL,
	CompareOperationNotEqual = VK_COMPARE_OP_NOT_EQUAL,
} CompareOperation;

typedef enum StencilOperation
{
	StencilOperationZero = VK_STENCIL_OP_ZERO,
	StencilOperationKeep = VK_STENCIL_OP_KEEP,
	StencilOperationInvert = VK_STENCIL_OP_INVERT,
	StencilOperationReplace = VK_STENCIL_OP_REPLACE,
	StencilOperationWrapIncrement = VK_STENCIL_OP_INCREMENT_AND_WRAP,
	StencilOperationClampIncrement =  VK_STENCIL_OP_INCREMENT_AND_CLAMP,
	StencilOperationWrapDecrement = VK_STENCIL_OP_DECREMENT_AND_WRAP,
	StencilOperationClampDecrement = VK_STENCIL_OP_DECREMENT_AND_CLAMP,
} StencilOperation;

typedef struct StencilConfigure
{
	CompareOperation Compare;
	StencilOperation Pass;
	StencilOperation Fail;
	StencilOperation DepthFail;
	unsigned int Reference;
} StencilConfigure;

typedef struct PipelineShader
{
	ShaderType Type;
	unsigned long DataSize;
	void * Data;
} PipelineShader;

PipelineShader PipelineShaderFromData(ShaderType type, unsigned long dataSize, void * data, bool Precompiled);
PipelineShader PipelineShaderFromFile(ShaderType type, const char * file, bool Precompiled);

typedef struct PipelineConfigure
{
	VertexLayout VertexLayout;
	int ShaderCount;
	PipelineShader Shaders[5];
	VertexPrimitive Primitive;
	Scalar LineWidth;
	PolygonMode PolygonMode;
	CullMode CullMode;
	bool CullClockwise;
	bool AlphaBlend;
	bool DepthTest;
	bool DepthWrite;
	CompareOperation DepthCompare;
	bool StencilTest;
	StencilConfigure FrontStencil;
	StencilConfigure BackStencil;
} PipelineConfigure;

typedef struct Pipeline
{
	VkPipeline Instance;
	VkPipelineLayout Layout;
	VertexLayout VertexLayout;
	Scalar LineWidth;
	unsigned int FrontStencilReference;
	unsigned int BackStencilReference;
	
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
	VkDescriptorPool DescriptorPool;
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
void PipelineSetLineWidth(Pipeline pipeline, Scalar lineWidth);
void PipelineSetFrontStencilReference(Pipeline pipeline, unsigned int reference);
void PipelineSetBackStencilReference(Pipeline pipeline, unsigned int reference);
void PipelineQueueDestroy(Pipeline pipeline);
void PipelineDestroy(Pipeline pipeline);

#endif
