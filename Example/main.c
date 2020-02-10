#include "../XGI/XGI.h"

typedef struct Vertex
{
	Vector3 Position;
	Color Color;
	Vector2 UV;
} Vertex;

FrameBuffer frameBuffer;
VertexLayout vertexLayout;
Pipeline pipeline;
VertexBuffer vertexBuffer;
Texture texture;

static void OnKeyPressed(Key key)
{
	if (key == KeyEscape) { WindowExitLoop(); }
}

static void OnResize(int width, int height)
{
	frameBuffer = FrameBufferResize(frameBuffer, width, height);
}

int main(int argc, char * argv[])
{
	WindowConfigure windowConfig =
	{
		.Width = 800,
		.Height = 600,
		.Title = "XGI Example",
		.Resizable = true,
		.HighDPI = true,
	};
	GraphicsConfigure graphicsConfig =
	{
		.VulkanValidation = true,
		.FrameResourceCount = 3,
	};
	XGIInitialize(windowConfig, graphicsConfig);

	//Initialization code starts here
	FrameBufferConfigure frameConfig =
	{
		.Width = Window.Width,
		.Height = Window.Height,
		.Filter = TextureFilterNearest,
		.AddressMode = TextureAddressModeMirroredRepeat,
	};
	frameBuffer = FrameBufferCreate(frameConfig);

	VertexAttribute attributes[] = { VertexAttributeVector3, VertexAttributeByte4, VertexAttributeVector2 };
	vertexLayout = VertexLayoutCreate(3, attributes);
	
	PipelineConfigure pipelineConfig =
	{
		.VertexLayout = vertexLayout,
		.ShaderCount = 2,
		.Shaders =
		{
			PipelineShaderFromFile(ShaderTypeVertex, "Example/Shaders/Default.vert", false),
			PipelineShaderFromFile(ShaderTypeFragment, "Example/Shaders/Default.frag", false),
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

	vertexBuffer = VertexBufferCreate(6, sizeof(Vertex));
	Vertex * vertices = VertexBufferMapVertices(vertexBuffer);
	vertices[0] = (Vertex){ { -1.0, -1.0, 0.0 }, ColorWhite, { 0.0, 0.0 } };
	vertices[1] = (Vertex){ { 1.0, -1.0, 0.0 }, ColorWhite, { 1.0, 0.0 } };
	vertices[2] = (Vertex){ { 1.0, 1.0, 0.0 }, ColorWhite, { 1.0, 1.0 } };
	vertices[3] = vertices[0];
	vertices[4] = vertices[2];
	vertices[5] = (Vertex){ { -1.0, 1.0, 0.0 }, ColorWhite, { 0.0, 1.0 } };
	VertexBufferUnmapVertices(vertexBuffer);
	VertexBufferUpload(vertexBuffer);

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
	PipelineSetSampler(pipeline, 0, 0, texture);

	EventHandlerSetCallback(EventTypeKeyDown, (void (*)(void))OnKeyPressed);
	EventHandlerSetCallback(EventTypeWindowResized, (void (*)(void))OnResize);

	while (Window.Running)
	{
		EventHandlerPoll();
		// Update code starts here

		SwapchainAquireNextImage();

		// Render code starts here
		GraphicsBegin(frameBuffer);
		GraphicsClearColor(ColorFromHex(0x204080ff));
		GraphicsBindPipeline(pipeline);
		PipelineSetPushConstant(pipeline, "Transform", &Matrix4x4Identity);
		GraphicsRenderVertexBuffer(vertexBuffer);
		GraphicsEnd();
		GraphicsCopyToSwapchain(frameBuffer);

		SwapchainPresent();
	}
	GraphicsStopOperations();

	// Deinitialization code starts here
	TextureDestroy(texture);
	VertexBufferDestroy(vertexBuffer);
	PipelineDestroy(pipeline);
	FrameBufferDestroy(frameBuffer);
	VertexLayoutDestroy(vertexLayout);

	XGIDeinitialize();
	return 0;
}
