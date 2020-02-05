#include "EventHandler.h"
#include "Window.h"
#include "Swapchain.h"

static void (*Callbacks[EventTypeCount])(void);
Key KeysDown[KeyScancodeCount];

void EventHandlerInitialize()
{
	for (int i = 0; i < EventTypeCount; i++) { Callbacks[i] = NULL; }
	for (int i = 0; i < KeyScancodeCount; i++) { KeysDown[i] = false; }
}

void EventHandlerSetCallback(EventType event, void (*callback)(void))
{
	Callbacks[event] = callback;
}

void EventHandlerPoll()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				EventHandlerCallbackQuit();
				break;
			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_SHOWN:
						EventHandlerCallbackWindowShown();
						break;
					case SDL_WINDOWEVENT_HIDDEN:
						EventHandlerCallbackWindowHidden();
						break;
					case SDL_WINDOWEVENT_EXPOSED:
						EventHandlerCallbackWindowExposed();
						break;
					case SDL_WINDOWEVENT_MOVED:
						EventHandlerCallbackWindowMoved(event.window.data1, event.window.data2);
						break;
					case SDL_WINDOWEVENT_RESIZED:
						EventHandlerCallbackWindowResized(event.window.data1, event.window.data2);
						break;
					case SDL_WINDOWEVENT_MINIMIZED:
						EventHandlerCallbackWindowMinimized();
						break;
					case SDL_WINDOWEVENT_MAXIMIZED:
						EventHandlerCallbackWindowMaximized();
						break;
					case SDL_WINDOWEVENT_RESTORED:
						EventHandlerCallbackWindowRestored();
						break;
					case SDL_WINDOWEVENT_ENTER:
						EventHandlerCallbackWindowEnter();
						break;
					case SDL_WINDOWEVENT_LEAVE:
						EventHandlerCallbackWindowLeave();
						break;
					case SDL_WINDOWEVENT_FOCUS_GAINED:
						EventHandlerCallbackWindowFocusGained();
						break;
					case SDL_WINDOWEVENT_FOCUS_LOST:
						EventHandlerCallbackWindowFocusLost();
						break;
					case SDL_WINDOWEVENT_CLOSE:
						EventHandlerCallbackWindowClose();
						break;
				}
				break;
			case SDL_KEYDOWN:
				EventHandlerCallbackKeyDown((Key)event.key.keysym.scancode);
				break;
			case SDL_KEYUP:
				EventHandlerCallbackKeyUp((Key)event.key.keysym.scancode);
				break;
			case SDL_TEXTEDITING:
				EventHandlerCallbackTextEditing(event.edit.text, event.edit.start, event.edit.length);
				break;
			case SDL_TEXTINPUT:
				EventHandlerCallbackTextInput(event.text.text);
				break;
			case SDL_KEYMAPCHANGED:
				EventHandlerCallbackKeyMapChanged();
				break;
			case SDL_MOUSEMOTION:
				EventHandlerCallbackMouseMotion(event.motion.which, event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
				break;
			case SDL_MOUSEBUTTONDOWN:
				EventHandlerCallbackMouseButtonDown(event.button.which, event.button.button, event.button.x, event.button.y);
				break;
			case SDL_MOUSEBUTTONUP:
				EventHandlerCallbackMouseButtonUp(event.button.which, event.button.button, event.button.x, event.button.y);
				break;
			case SDL_MOUSEWHEEL:
				EventHandlerCallbackMouseWheelMotion(event.wheel.which, event.wheel.x, event.wheel.y);
				break;
		}
	}
}

void EventHandlerCallbackQuit()
{
	WindowExitLoop();
	if (Callbacks[EventTypeQuit] != NULL) { Callbacks[EventTypeQuit](); }
}

void EventHandlerCallbackWindowShown()
{
	if (Callbacks[EventTypeWindowShown] != NULL) { Callbacks[EventTypeWindowShown](); }
}

void EventHandlerCallbackWindowHidden()
{
	if (Callbacks[EventTypeWindowHidden] != NULL) { Callbacks[EventTypeWindowHidden](); }
}

void EventHandlerCallbackWindowExposed()
{
	if (Callbacks[EventTypeWindowExposed] != NULL) { Callbacks[EventTypeWindowExposed](); }
}

void EventHandlerCallbackWindowMoved(int x, int y)
{
	if (Callbacks[EventTypeWindowMoved] != NULL) { ((void (*)(int, int))Callbacks[EventTypeWindowMoved])(x, y); }
}

void EventHandlerCallbackWindowResized(int width, int height)
{
	SwapchainDestroy();
	SwapchainCreate(width, height);
	if (Callbacks[EventTypeWindowResized] != NULL) { ((void (*)(int, int))Callbacks[EventTypeWindowResized])(width, height); }
}

void EventHandlerCallbackWindowMinimized()
{
	if (Callbacks[EventTypeWindowMinimized] != NULL) { Callbacks[EventTypeWindowMinimized](); }
}

void EventHandlerCallbackWindowMaximized()
{
	if (Callbacks[EventTypeWindowMaximized] != NULL) { Callbacks[EventTypeWindowMaximized](); }
}

void EventHandlerCallbackWindowRestored()
{
	if (Callbacks[EventTypeWindowRestored] != NULL) { Callbacks[EventTypeWindowRestored](); }
}

void EventHandlerCallbackWindowEnter()
{
	if (Callbacks[EventTypeWindowEnter] != NULL) { Callbacks[EventTypeWindowEnter](); }
}

void EventHandlerCallbackWindowLeave()
{
	if (Callbacks[EventTypeWindowLeave] != NULL) { Callbacks[EventTypeWindowLeave](); }
}

void EventHandlerCallbackWindowFocusGained()
{
	if (Callbacks[EventTypeWindowFocusGained] != NULL) { Callbacks[EventTypeWindowFocusGained](); }
}

void EventHandlerCallbackWindowFocusLost()
{
	if (Callbacks[EventTypeWindowFocusLost] != NULL) { Callbacks[EventTypeWindowFocusLost](); }
}

void EventHandlerCallbackWindowClose()
{
	if (Callbacks[EventTypeWindowClose] != NULL) { Callbacks[EventTypeWindowClose](); }
}

void EventHandlerCallbackKeyDown(Key key)
{
	KeysDown[key] = true;
	if (Callbacks[EventTypeKeyDown] != NULL) { ((void (*)(Key))Callbacks[EventTypeKeyDown])(key); }
}

void EventHandlerCallbackKeyUp(Key key)
{
	KeysDown[key] = false;
	if (Callbacks[EventTypeKeyUp] != NULL) { ((void (*)(Key))Callbacks[EventTypeKeyUp])(key); }
}

void EventHandlerCallbackTextEditing(char * text, int start, int length)
{
	if (Callbacks[EventTypeTextEditing] != NULL)
	{
		((void (*)(char *, int, int))Callbacks[EventTypeTextEditing])(text, start, length);
	}
}

void EventHandlerCallbackTextInput(char * text)
{
	if (Callbacks[EventTypeTextInput] != NULL) { ((void (*)(char *))Callbacks[EventTypeTextInput])(text); }
}

void EventHandlerCallbackKeyMapChanged()
{
	if (Callbacks[EventTypeKeyMapChanged] != NULL) { Callbacks[EventTypeKeyMapChanged](); }
}

void EventHandlerCallbackMouseMotion(int mouse, int x, int y, int dx, int dy)
{
	if (Callbacks[EventTypeMouseMotion] != NULL)
	{
		((void (*)(int, int, int, int, int))Callbacks[EventTypeMouseMotion])(mouse, x, y, dx, dy);
	}
}

void EventHandlerCallbackMouseButtonDown(int mouse, MouseButton button, int x, int y)
{
	if (Callbacks[EventTypeMouseButtonDown] != NULL)
	{
		((void (*)(int, MouseButton, int, int))Callbacks[EventTypeMouseButtonDown])(mouse, button, x, y);
	}
}

void EventHandlerCallbackMouseButtonUp(int mouse, MouseButton button, int x, int y)
{
	if (Callbacks[EventTypeMouseButtonUp] != NULL)
	{
		((void (*)(int, MouseButton, int, int))Callbacks[EventTypeMouseButtonUp])(mouse, button, x, y);
	}
}

void EventHandlerCallbackMouseWheelMotion(int mouse, int dx, int dy)
{
	if (Callbacks[EventTypeMouseWheelMotion] != NULL)
	{
		((void (*)(int, int, int))Callbacks[EventTypeMouseWheelMotion])(mouse, dx, dy);
	}
}

void EventHandlerDeinitialize()
{
}
