#ifndef Pipeline_h
#define Pipeline_h

#include <vulkan/vulkan.h>
#include <stdbool.h>

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
} * Pipeline;

Pipeline PipelineCreate(Shader shader);
void PipelineDestroy(Pipeline pipeline);

#endif
