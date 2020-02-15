#ifndef Event_h
#define Event_h

#include <stdbool.h>
#include <SDL2/SDL.h>
#include "List.h"
#include "Input.h"

typedef enum EventType
{
	/// Unknown event
	EventTypeNone,
	/// Occurs when the application is closed
	EventTypeQuit,
	/// Occurs when the window becomes shown
	EventTypeWindowShown,
	/// Occurs when the window becomes hidden
	EventTypeWindowHidden,
	/// Occurs when the window is moved
	EventTypeWindowMoved,
	/// Occurs when the window is resized
	EventTypeWindowResized,
	/// Occurs when the window is minimized
	EventTypeWindowMinimized,
	/// Occurs when the window is maximized
	EventTypeWindowMaximized,
	/// Occurs when the window is restored
	EventTypeWindowRestored,
	/// Occurs when the window gains mouse focus
	EventTypeWindowMouseFocusGained,
	/// Occurs when the window looses mouse focus
	EventTypeWindowMouseFocusLost,
	/// Occurs when the window gains keyboard focus
	EventTypeWindowKeyboardFocusGained,
	/// Occurs when the window looses keyboard focus
	EventTypeWindowKeyboardFocusLost,
	/// Occurs when the window is requested to be closed
	EventTypeWindowClose,
	/// Occurs when a key is pressed
	EventTypeKeyPressed,
	/// Occurs when a key is released
	EventTypeKeyReleased,
	/// Occurs when text is typed
	EventTypeTextInput,
	/// Occurs when the keyboard layout changes
	EventTypeKeyMapChanged,
	/// Occurs when a mouse is moved
	EventTypeMouseMotion,
	/// Occurs when a mouse button is pressed
	EventTypeMouseButtonPressed,
	/// Occurs when a mouse button is released
	EventTypeMouseButtonReleased,
	/// Occurs when a mouse wheel is moved
	EventTypeMouseWheelMotion,
	/// Occurs when a controller axis is moved
	EventTypeControllerAxisMotion,
	/// Occurs when a controller ball is moved
	EventTypeControllerBallMotion,
	/// Occurs when a controller button is pressed
	EventTypeControllerButtonPressed,
	/// Occurs when a controller button is released
	EventTypeControllerButtonReleased,
	/// Occurs when a controller hat is moved
	EventTypeControllerHatMotion,
	/// Occurs when a controller is connected
	EventTypeControllerConnected,
	/// Occurs when a controller is disconnected
	EventTypeControllerDisconnected,
	/// The number of different events
	EventTypeCount,
} EventType;

/// Don't call this, it's automatically called in XGIInitialize
void EventHandlerInitialize(void);

/// Adds a callback for the specified event
/// \param event The event type to add the callback to
/// \param callback A function pointer that's casted to void (*)(void)
void EventHandlerAddCallback(EventType event, void (*callback)(void));

/// Removes an existing callback from the specified event
/// \param event The event type to remove the callback from
/// \param callback A function pointer that's casted to void (*)(void) and was previously added as a callback
void EventHandlerRemoveCallback(EventType event, void (*callback)(void));

/// Don't call this, it's automatically called in XGIDeinitialize
void EventHandlerDeinitialize(void);

/// Iterates through all of the events and calls each callback, this should be called once per a frame
void EventHandlerPoll(void);

/// Template for EventTypeQuit callback
/// Called when the application is closed
void EventHandlerCallbackQuit(void);

/// Template for EventTypeWindowShown callback
/// Called when the window becomes shown
void EventHandlerCallbackWindowShown(void);

/// Template for EventTypeWindowHidden callback
/// Called when the window becomes hidden
void EventHandlerCallbackWindowHidden(void);

/// Template for EventTypeWindowMoved callback
/// Called when the window is moved
/// \param x The x position of where the window was moved on the display
/// \param y The y position of where the window was moved on the display
void EventHandlerCallbackWindowMoved(int x, int y);

/// Template for EventTypeWindowResized callback
/// Called when the window is resized
/// \param width The new width of the window
/// \param height The new height of the window
void EventHandlerCallbackWindowResized(int width, int height);

/// Template for EventTypeWindowMinimized callback
/// Called when the window is minimized
void EventHandlerCallbackWindowMinimized(void);

/// Template for EventTypeWindowMaximized callback
/// Called when the window is maximized
void EventHandlerCallbackWindowMaximized(void);

/// Template for EventTypeWindowRestored callback
/// Called when the window is restored
void EventHandlerCallbackWindowRestored(void);

/// Template for EventTypeWindowMouseFocusGained callback
/// Called when the window gains mouse focus
void EventHandlerCallbackWindowMouseFocusGained(void);

/// Template for EventTypeWindowMouseFocusLost callback
/// Called when the window looses mouse focus
void EventHandlerCallbackWindowMouseFocusLost(void);

/// Template for EventTypeWindowKeyboardFocusGained callback
/// Called when the window gains keyboard focus
void EventHandlerCallbackWindowKeyboardFocusGained(void);

/// Template for EventTypeWindowKeyboardFocusLost callback
/// Called when the window looses keyboard focus
void EventHandlerCallbackWindowKeyboardFocusLost(void);

/// Template for EventTypeWindowClose callback
/// Called when the window is requested to be closed
void EventHandlerCallbackWindowClose(void);

/// Template for EventTypeKeyPressed callback
/// Called when a key is pressed
/// \param key The key pressed
void EventHandlerCallbackKeyPressed(Key key);

/// Template for EventTypeKeyReleased callback
/// Called when a key is released
/// \param key The key released
void EventHandlerCallbackKeyReleased(Key key);

/// Template for EventTypeTextInput callback
/// Called when text is typed
/// \param text The text typed in
void EventHandlerCallbackTextInput(char * text);

/// Template for EventTypeKeyMapChanged callback
/// Called when the keyboard layout changes
void EventHandlerCallbackKeyMapChanged(void);

/// Template for EventTypeMouseMotion callback
/// Called when a mouse is moved
/// \param mouse The mouse device index
/// \param x The x position of the mouse
/// \param y The y position of the mouse
/// \param dx The change in the x position
/// \param dy The change in the y posiiton
void EventHandlerCallbackMouseMotion(int mouse, int x, int y, int dx, int dy);

/// Template for EventTypeMouseButtonPressed callback
/// Called when a mouse button is pressed
/// \param mouse The mouse device index
/// \param button The button pressed
/// \param x The x position of the mouse
/// \param y The y position of the mouse
void EventHandlerCallbackMouseButtonPressed(int mouse, MouseButton button, int x, int y);

/// Template for EventTypeMouseButtonReleased callback
/// Called when a mouse button is released
/// \param mouse The mouse device index
/// \param button The button pressed
/// \param x The x position of the mouse
/// \param y The y position of the mouse
void EventHandlerCallbackMouseButtonReleased(int mouse, MouseButton button, int x, int y);

/// Template for EventTypeMouseWheelMotion callback
/// Called when a mouse wheel is moved
/// \param mouse The mouse device index
/// \param dx The change in horizontal scroll
/// \param dy The change in vertical scroll
void EventHandlerCallbackMouseWheelMotion(int mouse, int dx, int dy);

/// Template for EventTypeControllerAxisMotion callback
/// Called when a controller axis is moved
/// \param controller The controller device index
/// \param axis The axis index
/// \param value The axis value
void EventHandlerCallbackControllerAxisMotion(int controller, unsigned char axis, int value);

/// Template for EventTypeControllerBallMotion callback
/// Called when a controller ball is moved
/// \param controller The controller device index
/// \param ball The ball index
/// \param dx The chagne in x value
/// \param dy The change in y value
void EventHandlerCallbackControllerBallMotion(int controller, unsigned char ball, int dx, int dy);

/// Template for EventTypeControlleHatMotion callback
/// Called when a controller hat is moved
/// \param controller The controller device index
/// \param hat The hat index
/// \param position The hat position
void EventHandlerCallbackControllerHatMotion(int controller, unsigned char hat, ControllerHatPosition position);

/// Template for EventTypeControllerButtonPressed callback
/// Called when a controller button is pressed
/// \param controller The controller device index
/// \param button The button index pressed
void EventHandlerCallbackControllerButtonPressed(int controller, unsigned char button);

/// Template for EventTypeControllerButtonReleased callback
/// Called when a controller button is released
/// \param controller The controller device index
/// \param button The button index released
void EventHandlerCallbackControllerButtonReleased(int controller, unsigned char button);

/// Template for EventTypeControllerConnected callback
/// Called when a controller is connected
/// \param controller The controller device index connected
void EventHandlerCallbackControllerConnected(int controller);

/// Template for EventTypeControllerDisconnected callback
/// Called when a controller is disconnected
/// \param controller The controller device index disconnected
void EventHandlerCallbackControllerDisconnected(int controller);

#endif
