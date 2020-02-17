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
		// Set the push constant
		PipelineSetPushConstant(pipeline, "Transform", &Matrix4x4Identity);

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
