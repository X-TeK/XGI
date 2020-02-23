#ifndef StorageBuffer_h
#define StorageBuffer_h

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include "Pipeline.h"

struct Pipeline;

typedef struct StorageBuffer
{
	SpvReflectBlockVariable Info;
	unsigned long Size;
	VkBuffer StagingBuffer;
	VmaAllocation StagingAllocation;
	VkBuffer Buffer;
	VmaAllocation Allocation;
	VkCommandBuffer UploadCommandBuffer;
	VkCommandBuffer DownloadCommandBuffer;
	VkFence UploadFence;
	VkFence DownloadFence;
	VkSemaphore UploadSemaphore;
} * StorageBuffer;

/// Creates a storage buffer for use in shaders.
/// The buffer can work for multiple pipelines and bindings, it just needs to view one as a template for the variable structure.
/// \param pipeline The template pipeline used to determine variable locations
/// \param binding The binding in the pipeline to make the template for.
/// \param instances The number of instances to use for array variables.
/// \return A storage buffer that can be used for multiple pipelines or shaders.
StorageBuffer StorageBufferCreate(struct Pipeline * pipeline, int binding, int instances);

/// Returns a pointer to the memory for the storage buffer, at the offset of the given variable.
/// Only one variable should be mapped at a time, do not call this again unless StorageBufferUnmapVariable has been called.
/// \param storageBuffer The storage buffer to map memory to
/// \param variable The name of the variable to map from the template pipeline binding supplied
/// \return A pointer to the memory of the storage buffer
void * StorageBufferMapVariable(StorageBuffer storageBuffer, const char * variable);

/// Must be called after StorageBufferMapVertices.
/// \param storageBuffer The storage buffer to unmap.
void StorageBufferUnmapVariable(StorageBuffer storageBuffer);

/// Pushes the memory staged in StorageBufferMapVariable to the GPU for use in shaders.
/// \param storageBuffer The storage buffer to upload
void StorageBufferUpload(StorageBuffer storageBuffer);

/// Retrieves the memory from the gpu into the staging buffer so it may be read from StorageBufferMapVariable.
/// \param storageBuffer The storage buffer to download
void StorageBufferDownload(StorageBuffer storageBuffer);

/// Places the storage buffer into a queue to be destroyed.
/// This should only be called if the storage buffer needs to be destroyed at render-time
/// \param storageBuffer The storage buffer to destroy
void StorageBufferQueueDestroy(StorageBuffer storageBuffer);

/// Destroys and frees the storage buffer object
/// Don't call this unless it's at the initialize or the deinitialize of the application, otherwise use StorageBufferQueueDestroy
/// \param storageBuffer The vertex buffer to destroy
void StorageBufferDestroy(StorageBuffer storageBuffer);

#endif
