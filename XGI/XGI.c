#include "XGI.h"
#include "EventHandler.h"
#include "Swapchain.h"

void XGIInitialize(WindowConfigure windowFlags, GraphicsConfigure graphicsFlags)
{
	WindowInitialize(windowFlags);
	EventHandlerInitialize();
	GraphicsInitialize(graphicsFlags);
	SwapchainCreate(Window.Width, Window.Height);
}

void XGIDeinitialize()
{
	SwapchainDestroy();
	GraphicsDeinitialize();
	EventHandlerDeinitialize();
	WindowDeinitialize();
}
