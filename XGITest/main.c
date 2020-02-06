#include <stdio.h>
#include <assert.h>
#include <XGI/Window.h>
#include <XGI/EventHandler.h>
#include <XGI/LinearMath.h>
#include <XGI/Graphics.h>
#include <XGI/Swapchain.h>
#include <XGI/File.h>
#include <XGI/XGI.h>

FrameBuffer frameBuffer;
VertexLayout vertexLayout;
VertexBuffer vertexBuffer;
Pipeline pipeline;
UniformBuffer uniform;
int frame;

struct Vertex
{
	Vector2 Position;
	Color Color;
};

static void OnKeyPressed(Key key);
static void OnWindowResized(int width, int height);

static void Initialize()
{
	frame = 0;
	
	FrameBufferConfigure frameConfig =
	{
		.Width = Window.Width,
		.Height = Window.Height,
		.Filter = TextureFilterNearest,
		.AddressMode = TextureAddressModeRepeat,
		.UseStencil = false,
	};
	frameBuffer = FrameBufferCreate(frameConfig);
	
	VertexAttribute attributes[] = { VertexAttributeVector2, VertexAttributeByte4 };
	vertexLayout = VertexLayoutCreate(2, attributes);
	
	vertexBuffer = VertexBufferCreate(3, sizeof(struct Vertex));
	struct Vertex * vertices = VertexBufferMapVertices(vertexBuffer);
	vertices[0] = (struct Vertex){ { 0.0f, 0.0f }, ColorRed };
	vertices[1] = (struct Vertex){ { 1.0f, 0.0f }, ColorGreen };
	vertices[2] = (struct Vertex){ { 0.0f, 1.0f }, ColorBlue };
	VertexBufferUnmapVertices(vertexBuffer);
	VertexBufferUpload(vertexBuffer);

	PipelineConfigure pipelineConfig =
	{
		.VertexLayout = vertexLayout,
		.ShaderCount = 2,
		.Shaders =
		{
			{
				.Type = ShaderTypeVertex,
				.LoadFromFile = true,
				.File = "Shaders/Default.vert.spv",
				.Precompiled = true,
			},
			{
				.Type = ShaderTypeFragment,
				.LoadFromFile = true,
				.File = "Shaders/Default.frag.spv",
				.Precompiled = true,
			}
		},
		.WireFrame = false,
		.FaceCull = false,
		.AlphaBlend = false,
		.DepthTest = false,
	};
	pipeline = PipelineCreate(pipelineConfig);
	Vector4 color = ColorToVector4(ColorWhite);
	PipelineSetPushConstant(pipeline, "Transform", &Matrix4x4Identity);
	PipelineSetPushConstant(pipeline, "Color", &color);
	
	uniform = UniformBufferCreate(pipeline, 0);
	Vector2 dimensions = { Window.Width, Window.Height };
	UniformBufferSetVariable(uniform, "Dimensions", &dimensions);
	PipelineSetUniform(pipeline, 0, 0, uniform);
	
	EventHandlerSetCallback(EventTypeKeyDown, (void (*)(void))OnKeyPressed);
	EventHandlerSetCallback(EventTypeWindowResized, (void (*)(void))OnWindowResized);
}

static void Update()
{
	frame++;
	Matrix4x4 transform = Matrix4x4FromAxisAngle(Vector3Forward, frame / 60.0);
	PipelineSetPushConstant(pipeline, "Transform", &transform);
}

static void Render()
{
	GraphicsBegin(frameBuffer);
	GraphicsClearColor(ColorFromHex(0x204080ff));
	GraphicsBindPipeline(pipeline);
	GraphicsRenderVertexBuffer(vertexBuffer);
	GraphicsEnd();
	GraphicsCopyToSwapchain(frameBuffer);
}

static void OnKeyPressed(Key key)
{
	if (key == KeyEscape) { WindowExitLoop(); }
}

static void OnWindowResized(int width, int height)
{
	frameBuffer = FrameBufferResize(frameBuffer, width, height);
	Vector2 dimensions = { width, height };
	UniformBufferSetVariable(uniform, "Dimensions", &dimensions);
}

static void Deinitialize()
{
	UniformBufferDestroy(uniform);
	VertexBufferDestroy(vertexBuffer);
	PipelineDestroy(pipeline);
	FrameBufferDestroy(frameBuffer);
	VertexLayoutDestroy(vertexLayout);
}

int main(int argc, const char * argv[])
{
	WindowConfigure windowConfig =
	{
		.Width = 800,
		.Height = 600,
		.Title = "XGI Example",
		.HighDPI = true,
		.Resizable = true,
		.FullScreen = false,
	};
	GraphicsConfigure graphicsConfig =
	{
		.VulkanValidation = true,
		.FrameResourceCount = 3,
	};
	XGIInitialize(windowConfig, graphicsConfig);
	Initialize();
	while (Window.Running)
	{
		EventHandlerPoll();
		Update();
		SwapchainAquireNextImage();
		Render();
		SwapchainPresent();
	}
	GraphicsStopOperations();
	Deinitialize();
	XGIDeinitialize();
	return 0;
}
