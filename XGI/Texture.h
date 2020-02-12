#ifndef Texture_h
#define Texture_h

#include <vulkan/vulkan.h>
#include <stdbool.h>

typedef struct TextureData
{
	unsigned int Width, Height;
	void * Pixels;
} TextureData;

/// Creates a texture data object from an image file
/// \param file The path to the image
/// \return The texturedata object
TextureData TextureDataFromFile(const char * file);

/// Destroys and frees a texture data object.
/// (It's important to do this, those texture datas are uncompressed and take a lot of memory)
/// \param data The texture data to free
void TextureDataDestroy(TextureData data);

typedef enum TextureFormat
{
	/// Used for creating a color texture
	TextureFormatColor,
	/// Used for creating a depth-stencil texture
	TextureFormatDepthStencil = VK_FORMAT_D32_SFLOAT_S8_UINT,
} TextureFormat;

typedef enum TextureFilter
{
	/// Linearly interpolates sampling when the texture is scaled up
	TextureFilterLinear = VK_FILTER_LINEAR,
	/// Chooses the nearest texel when the texture is scaled up
	TextureFilterNearest = VK_FILTER_NEAREST,
	/// Not sure, haven't tested yet
	TextureFilterCubic = VK_FILTER_CUBIC_IMG,
} TextureFilter;

typedef enum TextureAddressMode
{
	/// Repeats texture sampling outside the uv bounds
	TextureAddressModeRepeat = VK_SAMPLER_ADDRESS_MODE_REPEAT,
	/// Mirrored repeats sampling outside the uv bounds
	TextureAddressModeMirroredRepeat = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
	/// Clamps to one color outside the uv bounds
	TextureAddressModeClamp = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
} TextureAddressMode;

typedef struct TextureConfigure
{
	/// The width of the texture to create.
	/// Ignored if LoadFromData is true
	unsigned int Width;
	/// The height of the texture to create.
	/// Ignored if LoadFromData is true
	unsigned int Height;
	/// The type of texture (color or depth-stencil).
	/// Assumed to be color texture if LoadFromData is true
	TextureFormat Format;
	/// The sampling filter to use
	TextureFilter Filter;
	/// The address mode to use
	TextureAddressMode AddressMode;
	/// Whether or not the texture is being loaded from data.
	bool LoadFromData;
	/// The texture data object used if LoadFromData is true
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

/// Creates a texture object from a configuration
/// \param config The configuration to create from
/// \return The texture object created
Texture TextureCreate(TextureConfigure config);

/// Places the texture into a queue to be destroyed.
/// This should only be called if the texture needs to be destroyed at render-time
/// \param texture The texture to destroy
void TextureQueueDestroy(Texture texture);

/// Destroys and frees a texture object
/// Don't call this unless it's at the initialize or the deinitialize of the application, otherwise use TextureQueueDestroy
/// \param texture The texture to destroy
void TextureDestroy(Texture texture);

#endif
