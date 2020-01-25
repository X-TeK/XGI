#include "XGI.h"
#include "EventHandler.h"
#include "Swapchain.h"

void XGIInitialize(struct WindowConfig windowFlags, struct GraphicsConfig graphicsFlags)
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
