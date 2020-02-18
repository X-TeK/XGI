#include "EventHandler.h"
#include "Window.h"
#include "Graphics.h"

struct f_pointer { void (*function)(void); };

static List CallbackLists[EventTypeCount];

void EventHandlerInitialize()
{
	for (int i = 0; i < EventTypeCount; i++)
	{
		CallbackLists[i] = ListCreate();
	}
}

void EventHandlerAddCallback(EventType event, void (*callback)(void))
{
	struct f_pointer * pointer = malloc(sizeof(struct f_pointer));
	pointer->function = callback;
	ListPush(CallbackLists[event], pointer);
}

void EventHandlerRemoveCallback(EventType event, void (*callback)(void))
{
	for (int i = 0; i < ListCount(CallbackLists[event]); i++)
	{
		struct f_pointer * value = ListIndex(CallbackLists[event], i);
		if (value->function == callback)
		{
			ListRemove(CallbackLists[event], i);
			free(value);
			return;
		}
	}
}

void EventHandlerDeinitialize()
{
	for (int i = 0; i < EventTypeCount; i++)
	{
		for (int j = 0; j < ListCount(CallbackLists[i]); j++)
		{
			free(ListIndex(CallbackLists[i], j));
		}
		ListDestroy(CallbackLists[i]);
	}
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

static void CallAllFunctions(List list)
{
	for (int i = 0; i < list->Count; i++)
	{
		((struct f_pointer *)ListIndex(list, i))->function();
	}
}

void EventHandlerCallbackQuit()
{
	WindowExitLoop();
	CallAllFunctions(CallbackLists[EventTypeQuit]);
}

void EventHandlerCallbackWindowShown()
{
	CallAllFunctions(CallbackLists[EventTypeWindowShown]);
}

void EventHandlerCallbackWindowHidden()
{
	CallAllFunctions(CallbackLists[EventTypeWindowHidden]);
}

void EventHandlerCallbackWindowMoved(int x, int y)
{
	List list = CallbackLists[EventTypeWindowMoved];
	for (int i = 0; i < list->Count; i++)
	{
		((void (*)(int, int))(( (struct f_pointer *)ListIndex(list, i) )->function))(x, y);
	}
}

void EventHandlerCallbackWindowResized(int width, int height)
{
	GraphicsDestroySwapchain();
	GraphicsCreateSwapchain(width, height);
	List list = CallbackLists[EventTypeWindowResized];
	for (int i = 0; i < list->Count; i++)
	{
		((void (*)(int, int))(( (struct f_pointer *)ListIndex(list, i) )->function))(Window.Width, Window.Height);
	}
}

void EventHandlerCallbackWindowMinimized()
{
	CallAllFunctions(CallbackLists[EventTypeWindowMinimized]);
}

void EventHandlerCallbackWindowMaximized()
{
	CallAllFunctions(CallbackLists[EventTypeWindowMaximized]);
}

void EventHandlerCallbackWindowRestored()
{
	CallAllFunctions(CallbackLists[EventTypeWindowRestored]);
}

void EventHandlerCallbackWindowMouseFocusGained()
{
	CallAllFunctions(CallbackLists[EventTypeWindowMouseFocusGained]);
}

void EventHandlerCallbackWindowMouseFocusLost()
{
	CallAllFunctions(CallbackLists[EventTypeWindowMouseFocusLost]);
}

void EventHandlerCallbackWindowKeyboardFocusGained()
{
	CallAllFunctions(CallbackLists[EventTypeWindowKeyboardFocusGained]);
}

void EventHandlerCallbackWindowKeyboardFocusLost()
{
	CallAllFunctions(CallbackLists[EventTypeWindowKeyboardFocusLost]);
}

void EventHandlerCallbackWindowClose()
{
	CallAllFunctions(CallbackLists[EventTypeWindowClose]);
}

void EventHandlerCallbackKeyPressed(Key key)
{
	List list = CallbackLists[EventTypeKeyPressed];
	for (int i = 0; i < list->Count; i++)
	{
		((void (*)(Key))(( (struct f_pointer *)ListIndex(list, i) )->function))(key);
	}
}

void EventHandlerCallbackKeyReleased(Key key)
{
	List list = CallbackLists[EventTypeKeyReleased];
	for (int i = 0; i < list->Count; i++)
	{
		((void (*)(Key))(( (struct f_pointer *)ListIndex(list, i) )->function))(key);
	}
}

void EventHandlerCallbackTextInput(char * text)
{
	List list = CallbackLists[EventTypeTextInput];
	for (int i = 0; i < list->Count; i++)
	{
		((void (*)(char *))(( (struct f_pointer *)ListIndex(list, i) )->function))(text);
	}
}

void EventHandlerCallbackKeyMapChanged()
{
	CallAllFunctions(CallbackLists[EventTypeKeyMapChanged]);
}

void EventHandlerCallbackMouseMotion(int mouse, int x, int y, int dx, int dy)
{
	List list = CallbackLists[EventTypeMouseMotion];
	for (int i = 0; i < list->Count; i++)
	{
		((void (*)(int, int, int, int, int))(( (struct f_pointer *)ListIndex(list, i) )->function))(mouse, x, y, dx, dy);
	}
}

void EventHandlerCallbackMouseButtonPressed(int mouse, MouseButton button, int x, int y)
{
	List list = CallbackLists[EventTypeMouseButtonPressed];
	for (int i = 0; i < list->Count; i++)
	{
		((void (*)(int, MouseButton, int, int))(( (struct f_pointer *)ListIndex(list, i) )->function))(mouse, button, x, y);
	}
}

void EventHandlerCallbackMouseButtonReleased(int mouse, MouseButton button, int x, int y)
{
	List list = CallbackLists[EventTypeMouseButtonReleased];
	for (int i = 0; i < list->Count; i++)
	{
		((void (*)(int, MouseButton, int, int))(( (struct f_pointer *)ListIndex(list, i) )->function))(mouse, button, x, y);
	}
}

void EventHandlerCallbackMouseWheelMotion(int mouse, int dx, int dy)
{
	List list = CallbackLists[EventTypeMouseWheelMotion];
	for (int i = 0; i < list->Count; i++)
	{
		((void (*)(int, int, int))(( (struct f_pointer *)ListIndex(list, i) )->function))(mouse, dx, dy);
	}
}

void EventHandlerCallbackControllerAxisMotion(int controller, unsigned char axis, int value)
{
	List list = CallbackLists[EventTypeControllerAxisMotion];
	for (int i = 0; i < list->Count; i++)
	{
		((void (*)(int, unsigned char, int))(( (struct f_pointer *)ListIndex(list, i) )->function))(controller, axis, value);
	}
}

void EventHandlerCallbackControllerBallMotion(int controller, unsigned char ball, int dx, int dy)
{
	List list = CallbackLists[EventTypeControllerBallMotion];
	for (int i = 0; i < list->Count; i++)
	{
		((void (*)(int, unsigned char, int, int))(( (struct f_pointer *)ListIndex(list, i) )->function))(controller, ball, dx, dy);
	}
}

void EventHandlerCallbackControllerHatMotion(int controller, unsigned char hat, ControllerHatPosition position)
{
	List list = CallbackLists[EventTypeControllerHatMotion];
	for (int i = 0; i < list->Count; i++)
	{
		((void (*)(int, unsigned char, ControllerHatPosition))(( (struct f_pointer *)ListIndex(list, i) )->function))(controller, hat, position);
	}
}

void EventHandlerCallbackControllerButtonPressed(int controller, unsigned char button)
{
	List list = CallbackLists[EventTypeControllerButtonPressed];
	for (int i = 0; i < list->Count; i++)
	{
		((void (*)(int, unsigned char))(( (struct f_pointer *)ListIndex(list, i) )->function))(controller, button);
	}
}

void EventHandlerCallbackControllerButtonReleased(int controller, unsigned char button)
{
	List list = CallbackLists[EventTypeControllerButtonReleased];
	for (int i = 0; i < list->Count; i++)
	{
		((void (*)(int, unsigned char))(( (struct f_pointer *)ListIndex(list, i) )->function))(controller, button);
	}
}

void EventHandlerCallbackControllerConnected(int controller)
{
	List list = CallbackLists[EventTypeControllerConnected];
	for (int i = 0; i < list->Count; i++)
	{
		((void (*)(int))(( (struct f_pointer *)ListIndex(list, i) )->function))(controller);
	}
}

void EventHandlerCallbackControllerDisconnected(int controller)
{
	List list = CallbackLists[EventTypeControllerDisconnected];
	for (int i = 0; i < list->Count; i++)
	{
		((void (*)(int))(( (struct f_pointer *)ListIndex(list, i) )->function))(controller);
	}
}
