#ifndef Event_h
#define Event_h

#include <stdbool.h>
#include <SDL2/SDL.h>
#include "List.h"
#include "Input.h"

typedef enum EventType
{
	EventTypeNone,
	EventTypeQuit,
	EventTypeWindowShown,
	EventTypeWindowHidden,
	EventTypeWindowExposed,
	EventTypeWindowMoved,
	EventTypeWindowResized,
	EventTypeWindowMinimized,
	EventTypeWindowMaximized,
	EventTypeWindowRestored,
	EventTypeWindowEnter,
	EventTypeWindowLeave,
	EventTypeWindowFocusGained,
	EventTypeWindowFocusLost,
	EventTypeWindowClose,
	EventTypeKeyPressed,
	EventTypeKeyReleased,
	EventTypeTextEditing,
	EventTypeTextInput,
	EventTypeKeyMapChanged,
	EventTypeMouseMotion,
	EventTypeMouseButtonPressed,
	EventTypeMouseButtonReleased,
	EventTypeMouseWheelMotion,
	EventTypeControllerAxisMotion,
	EventTypeControllerBallMotion,
	EventTypeControllerButtonPressed,
	EventTypeControllerButtonReleased,
	EventTypeControllerHatMotion,
	EventTypeControllerConnected,
	EventTypeControllerDisconnected,
	EventTypeCount,
} EventType;

void EventHandlerInitialize(void);

void EventHandlerSetCallback(EventType event, void (*callback)(void));

void EventHandlerPoll(void);

void EventHandlerCallbackQuit(void);

void EventHandlerCallbackWindowShown(void);

void EventHandlerCallbackWindowHidden(void);

void EventHandlerCallbackWindowExposed(void);

void EventHandlerCallbackWindowMoved(int x, int y);

void EventHandlerCallbackWindowResized(int width, int height);

void EventHandlerCallbackWindowMinimized(void);

void EventHandlerCallbackWindowMaximized(void);

void EventHandlerCallbackWindowRestored(void);

void EventHandlerCallbackWindowEnter(void);

void EventHandlerCallbackWindowLeave(void);

void EventHandlerCallbackWindowFocusGained(void);

void EventHandlerCallbackWindowFocusLost(void);

void EventHandlerCallbackWindowClose(void);

void EventHandlerCallbackKeyPressed(Key key);

void EventHandlerCallbackKeyReleased(Key key);

void EventHandlerCallbackTextEditing(char * text, int start, int length);

void EventHandlerCallbackTextInput(char * text);

void EventHandlerCallbackKeyMapChanged(void);

void EventHandlerCallbackMouseMotion(int mouse, int x, int y, int dx, int dy);

void EventHandlerCallbackMouseButtonPressed(int mouse, MouseButton button, int x, int y);

void EventHandlerCallbackMouseButtonReleased(int mouse, MouseButton button, int x, int y);

void EventHandlerCallbackMouseWheelMotion(int mouse, int dx, int dy);

void EventHandlerCallbackControllerAxisMotion(int controller, unsigned char axis, int value);

void EventHandlerCallbackControllerBallMotion(int controller, unsigned char ball, int dx, int dy);

void EventHandlerCallbackControllerHatMotion(int controller, unsigned char hat, ControllerHatPosition position);

void EventHandlerCallbackControllerButtonPressed(int controller, unsigned char button);

void EventHandlerCallbackControllerButtonReleased(int controller, unsigned char button);

void EventHandlerCallbackControllerConnected(int controller);

void EventHandlerCallbackControllerDisconnected(int controller);

void EventHandlerDeinitialize(void);

#endif
