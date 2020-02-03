#include <stdio.h>
#include <assert.h>
#include <XGI/Window.h>
#include <XGI/EventHandler.h>
#include <XGI/LinearMath.h>
#include <XGI/Graphics.h>
#include <XGI/Swapchain.h>
#include <XGI/XGI.h>

struct Vertex
{
	Vector2 Position;
	Color Color;
};

int main(int argc, const char * argv[])
{
	WindowConfigure windowConfig =
	{
		.Width = 800,
		.Height = 600,
		.Title = "XGI Example",
		.HighDPI = true,
		.Resizable = false,
		.FullScreen = false,
	};
	GraphicsConfigure graphicsConfig =
	{
		.VulkanValidation = true,
		.FrameResourceCount = 3,
	};
	XGIInitialize(windowConfig, graphicsConfig);
	
	FrameBufferConfigure frameConfig =
	{
		.Width = Window.Width,
		.Height = Window.Height,
		.Filter = TextureFilterNearest,
		.AddressMode = TextureAddressModeRepeat,
		.UseStencil = false,
	};
	FrameBuffer framebuffer = FrameBufferCreate(frameConfig);
	FrameBuffer framebuffer2 = FrameBufferCreate(frameConfig);
	
	VertexAttribute attributes[] = { VertexAttributeVector2, VertexAttributeByte4 };
	VertexLayout vertexLayout = VertexLayoutCreate(2, attributes);
	
	VertexBuffer vertexBuffer = VertexBufferCreate(3, sizeof(struct Vertex));
	struct Vertex * vertices = VertexBufferMapVertices(vertexBuffer);
	vertices[0] = (struct Vertex){ { 0.0f, 0.0f }, ColorRed };
	vertices[1] = (struct Vertex){ { 1.0f, 0.0f }, ColorGreen };
	vertices[2] = (struct Vertex){ { 0.0f, 1.0f }, ColorBlue };
	VertexBufferUnmapVertices(vertexBuffer);
	VertexBufferUpload(vertexBuffer);
	
#include "Shaders/Default.vert.c"
#include "Shaders/Default.frag.c"
	Shader shader =
	{
		.VertexSPVSize = sizeof(SPV_DefaultVertexShader),
		.VertexSPV = SPV_DefaultVertexShader,
		.FragmentSPVSize = sizeof(SPV_DefaultFragmentShader),
		.FragmentSPV = SPV_DefaultFragmentShader,
	};
	Pipeline pipeline = PipelineCreate(shader, vertexLayout);
	Vector4 color = ColorToVector4(ColorWhite);
	PipelineSetPushConstant(pipeline, "Transform", &Matrix4x4Identity);
	PipelineSetPushConstant(pipeline, "Color", &color);
	
	UniformBuffer uniform = UniformBufferCreate(pipeline, 0);
	Vector2 dimensions = { Window.Width, Window.Height };
	UniformBufferSetVariable(uniform, "Dimensions", &dimensions);
	PipelineSetUniform(pipeline, 0, uniform);
	PipelineSetSampler(pipeline, 1, framebuffer2->ColorTexture);
	
	while (Window.Running)
	{
		EventHandlerPoll();
		if (EventHandlerOnKeyPressed(KeyEscape)) { WindowExitLoop(); }
		if (EventHandlerOnWindowResized())
		{
			framebuffer = FrameBufferResize(framebuffer, Window.Width, Window.Height);
		}
		SwapchainAquireNextImage();
		
		GraphicsBegin(framebuffer2);
		GraphicsClearColor(ColorRed);
		GraphicsEnd();
		
		GraphicsBegin(framebuffer);
		GraphicsClearColor(ColorFromHex(0x204080ff));
		GraphicsBindPipeline(pipeline);
		GraphicsRenderVertexBuffer(vertexBuffer);
		GraphicsEnd();
		
		GraphicsCopyToSwapchain(framebuffer);
		SwapchainPresent();
	}
	GraphicsStopOperations();
	
	UniformBufferDestroy(uniform);
	VertexBufferDestroy(vertexBuffer);
	PipelineDestroy(pipeline);
	FrameBufferDestroy(framebuffer);
	FrameBufferDestroy(framebuffer2);
	VertexLayoutDestroy(vertexLayout);
	
	XGIDeinitialize();
	return 0;
}
