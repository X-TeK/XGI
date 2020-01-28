#include <stdio.h>
#include <assert.h>
#include <XGI/Window.h>
#include <XGI/EventHandler.h>
#include <XGI/LinearMath.h>
#include <XGI/Graphics.h>
#include <XGI/Swapchain.h>
#include <XGI/XGI.h>

int main(int argc, const char * argv[])
{
	WindowConfigure windowConfig =
	{
		.Width = 800,
		.Height = 600,
		.Title = "XGI Test",
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
	
	VertexAttribute attributes[] = { VertexAttributeVector2 };
	VertexLayout vertexLayout = VertexLayoutCreate(1, attributes);
	
	VertexBuffer vertexBuffer = VertexBufferCreate(3, sizeof(Vector2));
	Vector2 * vertices = VertexBufferMapVertices(vertexBuffer);
	vertices[0] = (Vector2){ 0.0f, 0.0f };
	vertices[1] = (Vector2){ 1.0f, 0.0f };
	vertices[2] = (Vector2){ 0.0f, 1.0f };
	VertexBufferUnmapVertices(vertexBuffer);
	VertexBufferUploadStagingBuffer(vertexBuffer);
	
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
	Vector2 dimensions = { Window.Width, Window.Height };
	PipelineSetPushConstant(pipeline, "Dimensions", &dimensions);
	PipelineSetPushConstant(pipeline, "Transform", &Matrix4x4Identity);
	PipelineSetPushConstant(pipeline, "Color", &color);
	
	while (Window.Running)
	{
		EventHandlerPoll();
		if (EventHandlerOnKeyPressed(KeyEscape)) { WindowExitLoop(); }
		if (EventHandlerOnWindowResized())
		{
			framebuffer = FrameBufferResize(framebuffer, Window.Width, Window.Height);
		}
		SwapchainAquireNextImage();
		GraphicsBegin(framebuffer);
		GraphicsClear(ColorFromHex(0x204080ff), 1.0f, 0);
		GraphicsBindPipeline(pipeline);
		GraphicsRenderVertexBuffer(vertexBuffer);
		GraphicsEnd();
		GraphicsCopyToSwapchain(framebuffer);
		SwapchainPresent();
	}
	GraphicsStopOperations();
	
	VertexBufferDestroy(vertexBuffer);
	PipelineDestroy(pipeline);
	FrameBufferDestroy(framebuffer);
	VertexLayoutDestroy(vertexLayout);
	
	XGIDeinitialize();
	return 0;
}
