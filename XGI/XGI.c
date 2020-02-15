#include "XGI.h"
#include "EventHandler.h"
#include "log.h"

void XGIInitialize(WindowConfigure windowFlags, GraphicsConfigure graphicsFlags)
{
	log_info("Initializing XGI...\n");
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK) < 0)
	{
		log_fatal("Failed to initialize SDL\n");
		exit(1);
	}
	WindowInitialize(windowFlags);
	GraphicsInitialize(graphicsFlags);
	EventHandlerInitialize();
	log_info("Successfully initialized XGI.\n");
}

void XGIDeinitialize()
{
	EventHandlerDeinitialize();
	GraphicsDeinitialize();
	WindowDeinitialize();
}
