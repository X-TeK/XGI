#include "EventHandler.h"
#include "Window.h"
#include "Graphics.h"

static void (*Callbacks[EventTypeCount])(void) = { NULL };

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
					case SDL_WINDOWEVENT_MOVED:
						EventHandlerCallbackWindowMoved(event.window.data1, event.window.data2);
						break;
					case SDL_WINDOWEVENT_SIZE_CHANGED:
						EventHandlerCallbackWindowResized(event.window.data1, event.window.data2);
						break;
					case SDL_WINDOWEVENT_RESIZED:
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
						EventHandlerCallbackWindowMouseFocusGained();
						break;
					case SDL_WINDOWEVENT_LEAVE:
						EventHandlerCallbackWindowMouseFocusLost();
						break;
					case SDL_WINDOWEVENT_FOCUS_GAINED:
						EventHandlerCallbackWindowKeyboardFocusGained();
						break;
					case SDL_WINDOWEVENT_FOCUS_LOST:
						EventHandlerCallbackWindowKeyboardFocusLost();
						break;
					case SDL_WINDOWEVENT_CLOSE:
						EventHandlerCallbackWindowClose();
						break;
				}
				break;
			case SDL_KEYDOWN:
				EventHandlerCallbackKeyPressed((Key)event.key.keysym.scancode);
				break;
			case SDL_KEYUP:
				EventHandlerCallbackKeyReleased((Key)event.key.keysym.scancode);
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
				EventHandlerCallbackMouseButtonPressed(event.button.which, event.button.button, event.button.x, event.button.y);
				break;
			case SDL_MOUSEBUTTONUP:
				EventHandlerCallbackMouseButtonReleased(event.button.which, event.button.button, event.button.x, event.button.y);
				break;
			case SDL_MOUSEWHEEL:
				EventHandlerCallbackMouseWheelMotion(event.wheel.which, event.wheel.x, event.wheel.y);
				break;
			case SDL_JOYAXISMOTION:
				EventHandlerCallbackControllerAxisMotion(event.jaxis.which, event.jaxis.axis, event.jaxis.value);
				break;
			case SDL_JOYBALLMOTION:
				EventHandlerCallbackControllerBallMotion(event.jball.which, event.jball.ball, event.jball.xrel, event.jball.yrel);
				break;
			case SDL_JOYHATMOTION:
				EventHandlerCallbackControllerHatMotion(event.jhat.which, event.jhat.hat, event.jhat.value);
				break;
			case SDL_JOYBUTTONDOWN:
				EventHandlerCallbackControllerButtonPressed(event.jbutton.which, event.jbutton.button);
				break;
			case SDL_JOYBUTTONUP:
				EventHandlerCallbackControllerButtonReleased(event.jbutton.which, event.jbutton.button);
				break;
			case SDL_JOYDEVICEADDED:
				EventHandlerCallbackControllerConnected(event.jdevice.which);
				break;
			case SDL_JOYDEVICEREMOVED:
				EventHandlerCallbackControllerDisconnected(event.jdevice.which);
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

void EventHandlerCallbackWindowMoved(int x, int y)
{
	if (Callbacks[EventTypeWindowMoved] != NULL) { ((void (*)(int, int))Callbacks[EventTypeWindowMoved])(x, y); }
}

void EventHandlerCallbackWindowResized(int width, int height)
{
	GraphicsDestroySwapchain();
	GraphicsCreateSwapchain(width, height);
	if (Callbacks[EventTypeWindowResized] != NULL) { ((void (*)(int, int))Callbacks[EventTypeWindowResized])(Window.Width, Window.Height); }
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

void EventHandlerCallbackWindowMouseFocusGained()
{
	if (Callbacks[EventTypeWindowMouseFocusGained] != NULL) { Callbacks[EventTypeWindowMouseFocusGained](); }
}

void EventHandlerCallbackWindowMouseFocusLost()
{
	if (Callbacks[EventTypeWindowMouseFocusLost] != NULL) { Callbacks[EventTypeWindowMouseFocusLost](); }
}

void EventHandlerCallbackWindowKeyboardFocusGained()
{
	if (Callbacks[EventTypeWindowKeyboardFocusGained] != NULL) { Callbacks[EventTypeWindowKeyboardFocusGained](); }
}

void EventHandlerCallbackWindowKeyboardFocusLost()
{
	if (Callbacks[EventTypeWindowKeyboardFocusLost] != NULL) { Callbacks[EventTypeWindowKeyboardFocusLost](); }
}

void EventHandlerCallbackWindowClose()
{
	if (Callbacks[EventTypeWindowClose] != NULL) { Callbacks[EventTypeWindowClose](); }
}

void EventHandlerCallbackKeyPressed(Key key)
{
	if (Callbacks[EventTypeKeyPressed] != NULL) { ((void (*)(Key))Callbacks[EventTypeKeyPressed])(key); }
}

void EventHandlerCallbackKeyReleased(Key key)
{
	if (Callbacks[EventTypeKeyReleased] != NULL) { ((void (*)(Key))Callbacks[EventTypeKeyReleased])(key); }
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

void EventHandlerCallbackMouseButtonPressed(int mouse, MouseButton button, int x, int y)
{
	if (Callbacks[EventTypeMouseButtonPressed] != NULL)
	{
		((void (*)(int, MouseButton, int, int))Callbacks[EventTypeMouseButtonPressed])(mouse, button, x, y);
	}
}

void EventHandlerCallbackMouseButtonReleased(int mouse, MouseButton button, int x, int y)
{
	if (Callbacks[EventTypeMouseButtonReleased] != NULL)
	{
		((void (*)(int, MouseButton, int, int))Callbacks[EventTypeMouseButtonReleased])(mouse, button, x, y);
	}
}

void EventHandlerCallbackMouseWheelMotion(int mouse, int dx, int dy)
{
	if (Callbacks[EventTypeMouseWheelMotion] != NULL)
	{
		((void (*)(int, int, int))Callbacks[EventTypeMouseWheelMotion])(mouse, dx, dy);
	}
}

void EventHandlerCallbackControllerAxisMotion(int controller, unsigned char axis, int value)
{
	if (Callbacks[EventTypeControllerAxisMotion] != NULL)
	{
		((void (*)(int, unsigned char, int))Callbacks[EventTypeControllerAxisMotion])(controller, axis, value);
	}
}

void EventHandlerCallbackControllerBallMotion(int controller, unsigned char ball, int dx, int dy)
{
	if (Callbacks[EventTypeControllerBallMotion] != NULL)
	{
		((void (*)(int, unsigned char, int, int))Callbacks[EventTypeControllerBallMotion])(controller, ball, dx, dy);
	}
}

void EventHandlerCallbackControllerHatMotion(int controller, unsigned char hat, ControllerHatPosition position)
{
	if (Callbacks[EventTypeControllerHatMotion] != NULL)
	{
		((void (*)(int, unsigned char, ControllerHatPosition))Callbacks[EventTypeControllerHatMotion])(controller, hat, position);
	}
}

void EventHandlerCallbackControllerButtonPressed(int controller, unsigned char button)
{
	if (Callbacks[EventTypeControllerButtonPressed] != NULL)
	{
		((void (*)(int, unsigned char))Callbacks[EventTypeControllerButtonPressed])(controller, button);
	}
}

void EventHandlerCallbackControllerButtonReleased(int controller, unsigned char button)
{
	if (Callbacks[EventTypeControllerButtonReleased] != NULL)
	{
		((void (*)(int, unsigned char))Callbacks[EventTypeControllerButtonReleased])(controller, button);
	}
}

void EventHandlerCallbackControllerConnected(int controller)
{
	if (Callbacks[EventTypeControllerConnected] != NULL)
	{
		((void (*)(int))Callbacks[EventTypeControllerConnected])(controller);
	}
}

void EventHandlerCallbackControllerDisconnected(int controller)
{
	if (Callbacks[EventTypeControllerDisconnected] != NULL)
	{
		((void (*)(int))Callbacks[EventTypeControllerDisconnected])(controller);
	}
}
