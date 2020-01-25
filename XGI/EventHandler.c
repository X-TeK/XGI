#include "EventHandler.h"
#include "Window.h"
#include "Swapchain.h"

List EventList;
Key KeysDown[KeyScancodeCount];

void EventHandlerInitialize()
{
	EventList = ListCreate();
	for (int i = 0; i < KeyScancodeCount; i++) { KeysDown[i] = false; }
}

void EventHandlerPoll()
{
	for (int i = ListCount(EventList) - 1; i >= 0; i--)
	{
		free(ListIndex(EventList, i));
		ListOutsert(EventList, i);
	}
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT) { WindowExitLoop(); }
		if (event.type == SDL_KEYDOWN) { KeysDown[event.key.keysym.scancode] = true; }
		if (event.type == SDL_KEYUP) { KeysDown[event.key.keysym.scancode] = false; }
		SDL_Event * eventPointer = malloc(sizeof(SDL_Event));
		*eventPointer = event;
		ListPush(EventList, eventPointer);
	}
	if (EventHandlerOnWindowResized())
	{
		Vector2 size = WindowGetSize();
		SwapchainDestroy();
		SwapchainCreate(size.X, size.Y);
	}
}


static bool OnKeyEvent(Key key, SDL_EventType eventType, bool anyKey)
{
	for (int i = 0; i < ListCount(EventList); i++)
	{
		SDL_Event event = *(SDL_Event *)ListIndex(EventList, i);
		if (event.type == eventType)
		{
			if (event.key.keysym.scancode == (SDL_Scancode)key || anyKey) { return true; }
		}
	}
	return false;
}

bool EventHandlerOnKeyPressed(Key key) { return OnKeyEvent(key, SDL_KEYDOWN, false); }

bool EventHandlerOnAnyKeyPressed() { return OnKeyEvent(0, SDL_KEYDOWN, true); }

bool EventHandlerOnKeyReleased(Key key) { return OnKeyEvent(key, SDL_KEYUP, false); }

bool EventHandlerOnAnyKeyReleased() { return OnKeyEvent(0, SDL_KEYUP, true); }

bool EventHandlerIsKeyDown(Key key) { return KeysDown[key]; }


static bool OnWindowEvent(SDL_WindowEventID eventID)
{
	for (int i = 0; i < ListCount(EventList); i++)
	{
		SDL_Event event = *(SDL_Event *)ListIndex(EventList, i);
		if (event.type == SDL_WINDOWEVENT && event.window.event == eventID) { return true; }
	}
	return false;
}

bool EventHandlerOnWindowShown() { return OnWindowEvent(SDL_WINDOWEVENT_SHOWN); }

bool EventHandlerOnWindowHidden() { return OnWindowEvent(SDL_WINDOWEVENT_HIDDEN); }

bool EventHandlerOnWindowExposed() { return OnWindowEvent(SDL_WINDOWEVENT_EXPOSED); }

bool EventHandlerOnWindowMoved() { return OnWindowEvent(SDL_WINDOWEVENT_MOVED); }

bool EventHandlerOnWindowResized() { return OnWindowEvent(SDL_WINDOWEVENT_RESIZED); }

bool EventHandlerOnWindowMinimized() { return OnWindowEvent(SDL_WINDOWEVENT_MINIMIZED); }

bool EventHandlerOnWindowMaximized() { return OnWindowEvent(SDL_WINDOWEVENT_MAXIMIZED); }

bool EventHandlerOnWindowRestored() { return OnWindowEvent(SDL_WINDOWEVENT_RESTORED); }

bool EventHandlerOnWindowEnter() { return OnWindowEvent(SDL_WINDOWEVENT_ENTER); }

bool EventHandlerOnWindowLeave() { return OnWindowEvent(SDL_WINDOWEVENT_LEAVE); }

bool EventHandlerOnWindowFocusGained() { return OnWindowEvent(SDL_WINDOWEVENT_FOCUS_GAINED); }

bool EventHandlerOnWindowFocusLost() { return OnWindowEvent(SDL_WINDOWEVENT_FOCUS_LOST); }

bool EventHandlerOnWindowClose() { return OnWindowEvent(SDL_WINDOWEVENT_CLOSE); }

void EventHandlerDeinitialize()
{
	for (int i = 0; i < ListCount(EventList); i++) { free(ListIndex(EventList, i)); }
	ListDestroy(EventList);
}
