# LLGDK : Low Level Game Development Kit

**An easy to use graphics library built on top of Vulkan.**

## WARNING: This library is outdated, it's in the process of being refractored and renamed (old name was XGI)

## Features:
- A low level, yet easy to use, graphics library
- A drastic abstraction over vulkan, but still provides much control to the programmer
- Makes use of Vulkan's asynchronous draw calls and uses multiple frames in flight for optimal frame rate
- Provides an abstraction for handling window creation, input devices, events, etc.
- The API is programmed fully in C

## Modules:
Module            | Description
------------------|---------------------
`EventHandler`    | Processes events and manages callbacks
`File`            | Provides an easy way to read/write files
`FrameBuffer`     | Abstracts a color texture and depth-stencil texture for use in rendering
`Graphics`        | Provides all of the commands necessary for rendering
`Input`           | Provides the functionality to query information about input devices
`LinearMath`      | Provides all of the linear algebra functions needed for transformations
`List`            | Provides a dynamic and generic list object (uses void \*)
`Pipeline`        | Abstracts shaders, state configuration, and uniform variables into an object
`Texture`         | Allows for creating/loading images for use in rendering
`UniformBuffer`   | Provides the ability to upload memory to the gpu for use as uniforms in shaders
`VertexBuffer`    | Provides the ability to upload vertices to the gpu for use as input in shaders
`Window`          | Provides the ability to configure and control the window

## How to setup:

### Windows (using Visual Studio):
1. Install the Vulkan SDK (this part is technically optional but you won't get validation layers without it)
2. Create a new C/C++ project using Visual Studio
3. Clone or download this repository
4. Copy the folders XGI, Libraries, and Include into the project directory
5. Add all the files in the XGI folder to your project
6. Under the project properties and under the VC++ Directories tab, add the include directory XGI/Include and the library directory XGI/Libraries/Windows32 (or XGI/Libraries/Windows64 if you're targeting x64)
7. Under C/C++ - Code Generation set the Runtime Library to Multi-threaded (/MT)
8. Under Linker - Input add SDL2main.lib, SDL2.lib, shaderc_combined.lib and vulkan-1.lib
9. Create your main function in a new file and run

### MacOS (using XCode):
1. Download and install the Vulkan SDK for macOS
- Use version 1.1.114.0 (last time I tried to use a newer version I came across some issues with MoltenVK).
- To install it run the install_vulkan.py file in the download
2. Create a new C or C++ project using xcode
3. Clone or download this repository
4. Copy the folders XGI, Libraries, and Include into the project directory
5. Add all the files in the XGI folder to your project
6. Add $(PROJECT_DIR)/Include to your search headers in the build settings tab
7. Link the libraries SDL2.framework, libshaderc_combined.a and libvulkan.1.1.114.dylib, from the Library folder in the build phases tab
8. Create your main function in a new file and run

### Linux:
1. Ensure you have cmake, ninja and vulkan installed (`sudo apt install cmake ninja-build libvulkan-dev` on Ubuntu).
2. Clone this repository and https://github.com/google/shaderc (e.g. code/XGI and code/shaderc).
3. Build libshaderc per the instructions in https://github.com/google/shaderc#getting-and-building-shaderc in `libshaderc/build`.
4. `cd LLGDK/Example`
5. `mkdir build`
6. `cd build`
7. `cmake ..`
8. `make`
9. `cd ../../`
10. `./Example/build/xgi_example`

### iOS:
Theoretically possible since LLGDK uses MoltenVK and SDL2, but I haven't gotten it to work. Besides there's no touch support yet.

### Android:
Probably possible as well

## Example:
```C
// main.c

#include "../XGI/XGI.h"

// A textured triangle example: in 100 lines of code instead of 1000

// The custom vertex that is used in the shader
typedef struct Vertex
{
	Vector3 Position;
	Vector2 UV;
} Vertex;

// All the objects needed to start rendering
FrameBuffer frameBuffer;   // You need at least one framebuffer to render to
VertexLayout vertexLayout; // Needed for describing the custom vertex to the pipeline
Pipeline pipeline;         // Enscapulates shaders and other configuration into a state object
VertexBuffer vertexBuffer; // The quad used for rendering
Texture texture;           // The texture used to render on the quad

// This is a callback function that is registered with the eventhandler below
static void OnKeyPressed(Key key)
{
	// Detects if key 'escape' is pressed then quits
	if (key == KeyEscape) { WindowExitLoop(); }
}

// Another callback function that is called when the window resizes
static void OnResize(int width, int height)
{
	// You must manually resize framebuffers after a resize
	frameBuffer = FrameBufferResize(frameBuffer, width, height);
}

int main(int argc, char * argv[])
{
	// Window configuration
	WindowConfigure windowConfig =
	{
		.Width = 800,
		.Height = 600,
		.Title = "XGI Example",
		.Resizable = true,
		.HighDPI = true,
	};
	// Graphics Configuration
	GraphicsConfigure graphicsConfig =
	{
		.VulkanValidation = true,
		.FrameResourceCount = 3,
		.TargetPresentMode = PresentModeImmediateVsync,
	};
	XGIInitialize(windowConfig, graphicsConfig);
	
	// Initialization code starts here
	
	// Create the framebuffer
	FrameBufferConfigure frameConfig =
	{
		.Width = Window.Width,
		.Height = Window.Height,
		.Filter = TextureFilterNearest,
		.AddressMode = TextureAddressModeMirroredRepeat,
	};
	frameBuffer = FrameBufferCreate(frameConfig);

	// Create the vertex layout
	VertexAttribute attributes[] = { VertexAttributeVector3, VertexAttributeVector2 };
	vertexLayout = VertexLayoutCreate(2, attributes);

	// Create the pipeline
	PipelineConfigure pipelineConfig =
	{
		.VertexLayout = vertexLayout,
		.ShaderCount = 2,
		.Shaders =
		{
			ShaderDataFromFile(ShaderTypeVertex, "Example/Shaders/Default.vert", false),
			ShaderDataFromFile(ShaderTypeFragment, "Example/Shaders/Default.frag", false),
		},
		.Primitive = VertexPrimitiveTriangleList,
		.LineWidth = 1.0,
		.PolygonMode = PolygonModeFill,
		.CullMode = CullModeNone,
		.CullClockwise = true,
		.AlphaBlend = true,
		.DepthTest = false,
		.StencilTest = false,
	};
	pipeline = PipelineCreate(pipelineConfig);

	// Create the vertex buffer
	vertexBuffer = VertexBufferCreate(4, sizeof(Vertex), 6);
	unsigned int * bufferIndices;
	Vertex * vertices = VertexBufferMapVertices(vertexBuffer, &bufferIndices);
	// Set the vertex data
	vertices[0] = (Vertex){ { -1.0, -1.0, 0.0 }, { 0.0, 0.0 } };
	vertices[1] = (Vertex){ { 1.0, -1.0, 0.0 }, { 1.0, 0.0 } };
	vertices[2] = (Vertex){ { 1.0, 1.0, 0.0 }, { 1.0, 1.0 } };
	vertices[3] = (Vertex){ { -1.0, 1.0, 0.0 }, { 0.0, 1.0 } };
	// Set the index buffer data
	unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };
	memcpy(bufferIndices, indices, sizeof(indices));
	// Make the buffer visible on the gpu
	VertexBufferUnmapVertices(vertexBuffer);
	VertexBufferUpload(vertexBuffer);

	// Create the texture
	TextureData data = TextureDataFromFile("Example/texture.jpg");
	TextureConfigure textureConfig =
	{
		.Format = TextureFormatColor,
		.Filter = TextureFilterNearest,
		.AddressMode = TextureAddressModeRepeat,
		.AnisotropicFiltering = false,
		.LoadFromData = true,
		.Data = data,
	};
	texture = TextureCreate(textureConfig);
	TextureDataDestroy(data);
	// Set the pipeline binding 0 to the texture
	PipelineSetSampler(pipeline, 0, 0, texture);

	// Set the event handler callbacks
	EventHandlerAddCallback(EventTypeKeyPressed, (void (*)(void))OnKeyPressed);
	EventHandlerAddCallback(EventTypeWindowResized, (void (*)(void))OnResize);
	
	// Start the main loop
	while (WindowRunning())
	{
		EventHandlerPoll();
		
		// Update code starts here
		PipelineSetPushConstant(pipeline, "Transform", &Matrix4x4Identity);
		
		GraphicsUpdate();
		GraphicsAquireNextImage();
		// Render code starts here
		GraphicsBegin(frameBuffer);
		GraphicsClearColor(ColorFromHex(0x204080ff));
		GraphicsBindPipeline(pipeline);
		GraphicsRenderVertexBuffer(vertexBuffer);
		GraphicsEnd();
		// Copy the framebuffer to the swapchain for rendering
		GraphicsCopyToSwapchain(frameBuffer);

		GraphicsPresent();
	}
	GraphicsStopOperations(); // Important to call this right after exiting the main loop
	
	// Deinitialization code starts here
	TextureDestroy(texture);
	VertexBufferDestroy(vertexBuffer);
	PipelineDestroy(pipeline);
	FrameBufferDestroy(frameBuffer);
	VertexLayoutDestroy(vertexLayout);

	XGIDeinitialize();
	return 0;
}
```
```glsl
// Default.vert
#version 450

layout (location = 0) in vec3 PositionAttribute;
layout (location = 1) in vec2 UVAttribute;

layout (push_constant) uniform PushConstant
{
	mat4 Transform;
} Input;

layout (location = 1) out vec2 VertexUV;

void main()
{
	VertexUV = UVAttribute;
	gl_Position = Input.Transform * vec4(PositionAttribute, 1.0);
}
```
```glsl
// Default.frag
#version 450

layout (location = 1) in vec2 VertexUV;

layout (binding = 0) uniform sampler2D Texture;

layout (location = 0) out vec4 FragColor;

void main()
{
	FragColor = texture(Texture, VertexUV);
}
```
