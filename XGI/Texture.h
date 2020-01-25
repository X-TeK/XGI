#ifndef Texture_h
#define Texture_h

#include <vulkan/vulkan.h>

typedef enum TextureFormat
{
	TextureFormatColor,
	TextureFormatDepth,
	TextureFormatStencil,
} TextureFormat;

typedef struct Texture
{
	unsigned int Width, Height;
	TextureFormat Format;
	VkImageAspectFlags ImageAspect;
	VkImage Image;
	VmaAllocation Allocation;
	VkImageView ImageView;
	VkSampler Sampler;
	VkDescriptorSet * DescriptorSets;
} * Texture;

Texture TextureCreate(unsigned int width, unsigned int height, TextureFormat format);
void TextureDestroy(Texture texture);

#endif
