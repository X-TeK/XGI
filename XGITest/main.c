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
	struct WindowConfig windowConfig =
	{
		.Width = 800,
		.Height = 600,
		.Title = "XGI Test",
		.HighDPI = true,
		.Resizable = false,
		.FullScreen = false,
	};
	struct GraphicsConfig graphicsConfig =
	{
		.VulkanValidation = true,
		.FrameResourceCount = 3,
	};
	XGIInitialize(windowConfig, graphicsConfig);
	
	FrameBuffer framebuffer = FrameBufferCreate(Window.Width, Window.Height, false);
	
#include "Shaders/Default.vert.c"
#include "Shaders/Default.frag.c"
	Shader shader =
	{
		.VertexSPVSize = sizeof(SPV_DefaultVertexShader),
		.VertexSPV = SPV_DefaultVertexShader,
		.FragmentSPVSize = sizeof(SPV_DefaultFragmentShader),
		.FragmentSPV = SPV_DefaultFragmentShader,
	};
	Pipeline pipeline = PipelineCreate(shader);
	
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
		GraphicsEnd();
		GraphicsCopyToSwapchain(framebuffer);
		SwapchainPresent();
	}
	GraphicsStopOperations();
	
	PipelineDestroy(pipeline);
	FrameBufferDestroy(framebuffer);
	
	XGIDeinitialize();
	return 0;
}
