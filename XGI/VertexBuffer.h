#ifndef Vertexbuffer_h
#define Vertexbuffer_h

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include "LinearMath.h"

typedef enum VertexAttribute
{
	/// Corresponds to vec4 in shaders
	VertexAttributeVector4 = VK_FORMAT_R32G32B32A32_SFLOAT,
	/// Nomralized to correspond to a vec4 in shaders
	VertexAttributeByte4 = VK_FORMAT_R8G8B8A8_UNORM,
	/// Corresponds to vec3 in shaders
	VertexAttributeVector3 = VK_FORMAT_R32G32B32_SFLOAT,
	/// Corresponds to vec2 in shaders
	VertexAttributeVector2 = VK_FORMAT_R32G32_SFLOAT,
	/// Corresponds to float in shaders
	VertexAttributeFloat = VK_FORMAT_R32_SFLOAT,
} VertexAttribute;

typedef struct VertexLayout
{
	VkVertexInputBindingDescription Binding;
	unsigned int AttributeCount;
	VkVertexInputAttributeDescription * Attributes;
	unsigned int Size;
} * VertexLayout;

/// Creates a vertex layout object used for pipelines
/// \param attributeCount The number of attributes in the layout
/// \param attributes An array of attributes
/// \return The created vertex layout object
VertexLayout VertexLayoutCreate(int attributeCount, VertexAttribute * attributes);

/// Destroys a vertex layout object
/// \param layout The vertex layout to destroy
void VertexLayoutDestroy(VertexLayout layout);

typedef struct VertexBuffer
{
	int VertexCount;
	int VertexSize;
	int IndexCount;
	VkBuffer StagingBuffer;
	VmaAllocation StagingAllocation;
	VkBuffer VertexBuffer;
	VmaAllocation VertexAllocation;
	VkCommandBuffer CommandBuffer;
	VkFence Fence;
	VkSemaphore Semaphore;
} * VertexBuffer;

/// Creates a vertex buffer combined with an index buffer used for rendering.
/// The index buffer is optional, specifying 0 for the index count will disable it.
/// \param vertexCount The number of vertices to allocate
/// \param vertexSize The size of each vertex
/// \param indexCount The number of indices, this is optional and specifying 0 will disable the index buffer
/// \return The newly created vertex buffer
VertexBuffer VertexBufferCreate(int vertexCount, int vertexSize, int indexCount);

/// Allows for copying data into a vertex buffer.
/// This function only stages the memory onto the cpu, call VertexBufferUpload for it to be visible on the gpu.
/// \param vertexBuffer The vertexbuffer to copy data to
/// \return A pointer to memory that is pre-allocated to vertexCount * vertexSize
void * VertexBufferMapVertices(VertexBuffer vertexBuffer);

/// Must be called after copying memory with VertexBufferMapVertices.
/// It let's the gpu know that the memory isn't in use.
/// \param vertexBuffer The vertexbuffer that had its vertices mapped.
void VertexBufferUnmapVertices(VertexBuffer vertexBuffer);

/// Allows for copying data into the index buffer.
/// If the index buffer is disabled this returns NULL.
/// Like VertexBufferMapVertices, this only stages the memory onto the CPU and needs VertexBufferUpload to be used on the GPU.
/// The indices are actually stored in the same VkBuffer object as the vertices, just at an offset.
/// \param vertexBuffer The vertexbuffer to copy data to
/// \return A uint32 pointer to memory that is pre-allocated to sizeof(uint32) * indexCount
unsigned int * VertexBufferMapIndices(VertexBuffer vertexBuffer);

/// Must be called after copying memory with VertexBufferMapIndices.
/// It let's the gpu know that the memory isn't in use.
/// This does nothing if the index buffer is disabled.
/// \param vertexBuffer The vertexbuffer that had its vertices mapped.
void VertexBufferUnmapIndices(VertexBuffer vertexBuffer);

/// Pushes the memory staged in VertexBufferMapVertices to the GPU for use in shaders.
/// If this isn't called then the gpu will render garbage data.
/// \param vertexBuffer The vertexbuffer to upload
void VertexBufferUpload(VertexBuffer vertexBuffer);

/// Places the vertex buffer into a queue to be destroyed.
/// This should only be called if the vertex buffer needs to be destroyed at render-time
/// \param vertexBuffer The vertex buffer to destroy
void VertexBufferQueueDestroy(VertexBuffer vertexBuffer);

/// Destroys and frees the vertex buffer object
/// Don't call this unless it's at the initialize or the deinitialize of the application, otherwise use VertexBufferQueueDestroy
/// \param vertexBuffer The vertex buffer to destroy
void VertexBufferDestroy(VertexBuffer vertexBuffer);

#endif
