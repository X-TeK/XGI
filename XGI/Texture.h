#ifndef Texture_h
#define Texture_h

#include <vulkan/vulkan.h>
#include <stdbool.h>

typedef struct TextureData
{
	unsigned int Width, Height;
	void * Pixels;
} TextureData;

TextureData TextureDataFromFile(const char * file);

typedef enum TextureFormat
{
	TextureFormatColor,
	TextureFormatDepthStencil = VK_FORMAT_D32_SFLOAT_S8_UINT,
} TextureFormat;

typedef enum TextureFilter
{
	TextureFilterLinear = VK_FILTER_LINEAR,
	TextureFilterNearest = VK_FILTER_NEAREST,
	TextureFilterCubic = VK_FILTER_CUBIC_IMG,
} TextureFilter;

typedef enum TextureAddressMode
{
	TextureAddressModeRepeat = VK_SAMPLER_ADDRESS_MODE_REPEAT,
	TextureAddressModeMirroredRepeat = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
	TextureAddressModeClamp = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
} TextureAddressMode;

typedef struct TextureConfigure
{
	unsigned int Width, Height;
	TextureFormat Format;
	TextureFilter Filter;
	TextureAddressMode AddressMode;
	bool LoadFromData;
	TextureData Data;
} TextureConfigure;

typedef struct Texture
{
	unsigned int Width, Height;
	TextureFormat Format;
	VkImage Image;
	VmaAllocation Allocation;
	VkImageView ImageView;
	VkSampler Sampler;
} * Texture;

Texture TextureCreate(TextureConfigure config);
void TextureQueueDestroy(Texture texture);
void TextureDestroy(Texture texture);

#endif
