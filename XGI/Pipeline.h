#ifndef Pipeline_h
#define Pipeline_h

#include <vulkan/vulkan.h>
#include <stdbool.h>
#include <spirv/spirv_reflect.h>
#include "VertexBuffer.h"
#include "UniformBuffer.h"
#include "StorageBuffer.h"
#include "Texture.h"

struct UniformBuffer;
struct StorageBuffer;

typedef enum ShaderType
{
	/// Vertex Shader
	ShaderTypeVertex = VK_SHADER_STAGE_VERTEX_BIT,
	/// Fragment Shader
	ShaderTypeFragment = VK_SHADER_STAGE_FRAGMENT_BIT,
	/// Compute Shader (only usable for compute pipeline creation)
	ShaderTypeCompute = VK_SHADER_STAGE_COMPUTE_BIT,
} ShaderType;

typedef enum PolygonMode
{
	/// Fill in the polygons when rendering
	PolygonModeFill = VK_POLYGON_MODE_FILL,
	/// Only draw lines when rendering
	PolygonModeLine = VK_POLYGON_MODE_LINE,
	/// Only draw points when rendering
	PolygonModePoint = VK_POLYGON_MODE_POINT,
} PolygonMode;

typedef enum VertexPrimitive
{
	/// Every three vertices is drawn as a triangle
	VertexPrimitiveTriangleList = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	/// Every vertex is used with the previous vertex and the first vertex to draw a triangle
	VertexPrimitiveTriangleFan = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,
	/// Every vertex is used with the previous two vertices to draw a triangle
	VertexPrimitiveTriangleStrip = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
	/// Every two vertices is drawn as a line segment
	VertexPrimitiveLineList = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
	/// Every vertex is used with the previous vertex to draw a line segment
	VertexPrimitiveLineStrip = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
	/// Every vertex is a point
	VertexPrimitivePointList = VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
} VertexPrimitive;

typedef enum CullMode
{
	/// No culling
	CullModeNone = VK_CULL_MODE_NONE,
	/// Front faces are culled
	CullModeFront = VK_CULL_MODE_FRONT_BIT,
	/// Back faces are culled
	CullModeBack = VK_CULL_MODE_BACK_BIT,
	/// Front and back faces are culled (why would you want this?)
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
	/// The compare operation to use
	CompareOperation Compare;
	/// The operation to use if the stencil test passes
	StencilOperation Pass;
	/// The operation to use if the stencil test fails
	StencilOperation Fail;
	/// The operation to use if the depth test fails but the stencil test passes
	StencilOperation DepthFail;
	/// The value to compare against
	unsigned int Reference;
} StencilConfigure;

typedef struct ShaderData
{
	ShaderType Type;
	unsigned long DataSize;
	void * Data;
} ShaderData;

/// Loads a shader from memory.
/// GLSL strings are accepted just set precompiled to false
/// \param type The type of shader
/// \param dataSize The size of the data to load
/// \param data The data to load
/// \param precompiled Whether or not the data is compiled to spv
/// \return The shader data required for the pipeline configuration
ShaderData ShaderDataFromMemory(ShaderType type, unsigned long dataSize, void * data, bool precompiled);

/// Loads a shader from a file
/// \param type The type of shader
/// \param file The file to load
/// \param precompiled Whether or not it's precompiled
/// \return The shader data required for the pipeline configuration
ShaderData ShaderDataFromFile(ShaderType type, const char * file, bool precompiled);

typedef struct PipelineConfigure
{
	/// The vertex layout that the pipeline uses.
	/// It must match the attribute locations and types in the vertex shader
	VertexLayout VertexLayout;
	/// The number of shaders to use in the pipeline.
	/// Vertex and fragment shaders are required and also currently the only two types of shaders
	int ShaderCount;
	/// The shader datas to use, the ShaderCount specifies the number to use
	ShaderData Shaders[5];
	/// The vertex primitive that determines how the pipeline draws the vertices
	VertexPrimitive Primitive;
	/// The polygon mode used for rasterization
	PolygonMode PolygonMode;
	/// What the pipeline should cull
	CullMode CullMode;
	/// Whether or not the pipeline culls clockwise or counter clockwise
	bool CullClockwise;
	/// Whether or not the pipeline blends alpha values
	bool AlphaBlend;
	/// Whether or not the pipeline tests against the depth buffer
	bool DepthTest;
	/// Whether or not the pipeline writes the depth values to the depth buffer
	bool DepthWrite;
	/// What comparison operator to use for the depth test
	CompareOperation DepthCompare;
	/// Whether or not the pipeline tests against the stencil buffer
	bool StencilTest;
	/// The stencil test for front facing triangles
	StencilConfigure FrontStencil;
	/// The stencil test for back facing triangles
	StencilConfigure BackStencil;
} PipelineConfigure;

typedef struct Pipeline
{
	bool IsCompute;
	VkPipeline Instance;
	VkPipelineLayout Layout;
	VertexLayout VertexLayout;
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

/// Creates a pipeline from a pipeline configuration
/// \param config The pipeline configuration to use
/// \return The pipeline object
Pipeline PipelineCreate(PipelineConfigure config);

/// Sets a push constant value in the pipeline shaders
/// \param pipeline The pipeline to set push constants
/// \param variableName The name of the member in the push_constant struct
/// \param value A pointer to the memory to set, it's assumed that the pointer points to data that is the correct size
void PipelineSetPushConstant(Pipeline pipeline, const char * variableName, void * value);

/// Sets a uniform buffer to a binding in the shader.
/// This is not like push constanst where the buffer can be changed in between draw calls,
/// If the buffer needs to be changed then make the binding an array of values and use push constants to change which index to use.
/// \param pipeline The pipeline to set
/// \param binding The binding specified in the shader to set
/// \param arrayIndex The index in the array to set (0 if it's not an array)
/// \param uniform The uniform buffer object containing the data to set
void PipelineSetUniform(Pipeline pipeline, int binding, int arrayIndex, struct UniformBuffer * uniform);

/// Sets a sampler2D to a binding in the shader.
/// This is not like push constants where the sampler can be chagned in between draw calls.
/// If the sampler needs to be changed then make the binding an array of samplers and use push constants to change which array index.
/// \param pipeline The pipeline to modify
/// \param binding The binding specified in the shader to set
/// \param arrayIndex The index in the array to set (0 if it's not an array)
/// \param texture The texture to sample
void PipelineSetSampler(Pipeline pipeline, int binding, int arrayIndex, Texture texture);

/// Sets a storage buffer to a binding in the shader.
/// This is not like push constants where the buffer can be changed in between draw calls.
/// \param pipeline The pipeline to set
/// \param binding The binding number specified in the shader to set
/// \param arrayIndex The index in the array to set (0 if it's not an array)
/// \param storage The storage buffer to set
void PipelineSetStorageBuffer(Pipeline pipeline, int binding, int arrayIndex, struct StorageBuffer * storage);

/// Sets the front stencil reference value (the value that's compared against for front facing triangles)
/// \param pipeline The pipeline to modify
/// \param reference The new reference value
void PipelineSetFrontStencilReference(Pipeline pipeline, unsigned int reference);

/// Sets the back stencil reference value (the value that's compared against for back facing triangles)
/// \param pipeline The pipeline to modify
/// \param reference The new reference value
void PipelineSetBackStencilReference(Pipeline pipeline, unsigned int reference);

/// Places the pipeline into a queue to be destroyed.
/// Only call this if the pipeline needs to be destroyed while the application is rendering.
/// \param pipeline The pipeline to destroy
void PipelineQueueDestroy(Pipeline pipeline);

/// Destroys a pipeline object.
/// Don't call this unless it's at the initialize or the deinitialize of the application, otherwise use PipelineQueueDestroy
/// \param pipeline The pipeline to destroy
void PipelineDestroy(Pipeline pipeline);

typedef Pipeline ComputePipeline;

ComputePipeline ComputePipelineCreate(ShaderData shader);

void ComputePipelineDestroy(ComputePipeline pipeline);

#endif
