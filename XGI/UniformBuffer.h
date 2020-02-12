#ifndef Uniform_h
#define Uniform_h

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include "FrameBuffer.h"
#include "LinearMath.h"
#include "Pipeline.h"

struct Pipeline;

typedef struct UniformBuffer
{
	VkBuffer Buffer;
	VmaAllocation Allocation;
	SpvReflectBlockVariable Info;
	unsigned int Size;
} * UniformBuffer;

/// Creates a memory buffer on the vram for use in shaders.
/// The buffer can work for multiple pipelines and bindings, it just needs to view one for a template.
/// \param pipeline The template pipeline used to determine variable locations
/// \param binding The binding in the pipeline to make the template for.
/// \return A buffer that can be used for multiple pipelines or shaders.
UniformBuffer UniformBufferCreate(struct Pipeline * pipeline, int binding);

/// Sets a member within the uniform binding struct
/// \param uniformBuffer The buffer to set
/// \param variable The name of the member to set
/// \param value A pointer to the memory to copy, it's assumed that it's allocated for the right size
void UniformBufferSetVariable(UniformBuffer uniformBuffer, const char * variable, void * value);

/// Places the uniform buffer into a queue to be destroyed.
/// This should only be called if the uniform buffer needs to be destroyed at render-time
/// \param uniformBuffer The uniform buffer to destroy
void UniformBufferQueueDestroy(UniformBuffer uniformBuffer);

/// Destroys and frees a uniform buffer object
/// Don't call this unless it's at the initialize or the deinitialize of the application, otherwise use UniformBufferQueueDestroy
/// \param uniformBuffer The uniform buffer to destroy
void UniformBufferDestroy(UniformBuffer uniformBuffer);

#endif
