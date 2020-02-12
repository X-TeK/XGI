#ifndef FrameBuffer_h
#define FrameBuffer_h

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include "Texture.h"
#include "Pipeline.h"

typedef struct FrameBufferConfigure
{
	/// Width of the framebuffer
	unsigned int Width;
	/// Height of the framebuffer
	unsigned int Height;
	/// The filter to use when scaling the framebuffer
	TextureFilter Filter;
	/// How the texture should be sampled outside the (0, 1) bounds
	TextureAddressMode AddressMode;
} FrameBufferConfigure;

typedef struct FrameBuffer
{
	unsigned int Width, Height;
	TextureFilter Filter;
	TextureAddressMode AddressMode;
	Texture ColorTexture;
	Texture DepthTexture;
	VkFramebuffer Instance;
} * FrameBuffer;

/// Creates a framebuffer object from a configuration
/// \param config The framebuffer configuration
/// \return The framebuffer object
FrameBuffer FrameBufferCreate(FrameBufferConfigure config);

/// Gets the width from a framebuffer object
/// \param frameBuffer The framebuffer object
/// \return The width
unsigned int FrameBufferGetWidth(FrameBuffer frameBuffer);

/// Gets the height from a framebuffer object
/// \param frameBuffer The framebuffer object
/// \return The height
unsigned int FrameBufferGetHeight(FrameBuffer frameBuffer);

/// Gets the texture filter from a framebuffer object
/// \param frameBuffer The framebuffer object
/// \return The texture filter
TextureFilter FrameBufferGetFilter(FrameBuffer frameBuffer);

/// Gets the address mode from a framebuffer object
/// \param frameBuffer The framebuffer object
/// \return The address mode
TextureAddressMode FrameBufferGetAddressMode(FrameBuffer frameBuffer);

/// Resizes a framebuffer object by destroying it and creating a new one.
/// Anything previously drawn on it must be draw again.
/// \param frameBuffer The framebuffer object to resize
/// \param width The new width
/// \param height The new height
/// \return The new framebuffer object
FrameBuffer FrameBufferResize(FrameBuffer frameBuffer, unsigned int width, unsigned int height);

/// Places the framebuffer into a queue to be destroyed.
/// This should be called if the framebuffer needs to be destoryed while rendering is occuring
/// \param frameBuffer The framebuffer object to destroy
void FrameBufferQueueDestroy(FrameBuffer frameBuffer);

/// Destroys a framebuffer object
/// Don't call this unless it's at the initialize or the deinitialize of the application, otherwise use FrameBufferQueueDestroy
/// \param frameBuffer The framebuffer object to destroy
void FrameBufferDestroy(FrameBuffer frameBuffer);

#endif
