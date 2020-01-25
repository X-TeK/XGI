#ifndef Event_h
#define Event_h

#include <stdbool.h>
#include <SDL2/SDL.h>
#include "List.h"

typedef enum Key
{
	KeyUnknown = SDL_SCANCODE_UNKNOWN,
	
	KeyA = SDL_SCANCODE_A, KeyB = SDL_SCANCODE_B, KeyC = SDL_SCANCODE_C, KeyD = SDL_SCANCODE_D, KeyE = SDL_SCANCODE_E,
	KeyF = SDL_SCANCODE_F, KeyG = SDL_SCANCODE_G, KeyH = SDL_SCANCODE_H, KeyI = SDL_SCANCODE_I, KeyJ = SDL_SCANCODE_J,
	KeyK = SDL_SCANCODE_K, KeyL = SDL_SCANCODE_L, KeyM = SDL_SCANCODE_M, KeyN = SDL_SCANCODE_N, KeyO = SDL_SCANCODE_O,
	KeyP = SDL_SCANCODE_P, KeyQ = SDL_SCANCODE_Q, KeyR = SDL_SCANCODE_R, KeyS = SDL_SCANCODE_S, KeyT = SDL_SCANCODE_T,
	KeyU = SDL_SCANCODE_U, KeyV = SDL_SCANCODE_V, KeyW = SDL_SCANCODE_W, KeyX = SDL_SCANCODE_X, KeyY = SDL_SCANCODE_Y,
	KeyZ = SDL_SCANCODE_Z,
	
	Key0 = SDL_SCANCODE_0, Key1 = SDL_SCANCODE_1, Key2 = SDL_SCANCODE_2, Key3 = SDL_SCANCODE_3, Key4 = SDL_SCANCODE_4,
	Key5 = SDL_SCANCODE_5, Key6 = SDL_SCANCODE_6, key7 = SDL_SCANCODE_7, Key8 = SDL_SCANCODE_8, Key9 = SDL_SCANCODE_9,
	
	KeyReturn = SDL_SCANCODE_RETURN, KeyEscape = SDL_SCANCODE_ESCAPE, KeyBackspace = SDL_SCANCODE_BACKSPACE,
	KeyTab = SDL_SCANCODE_TAB, KeySpace = SDL_SCANCODE_SPACE,
	
	KeyMinus = SDL_SCANCODE_MINUS, KeyEquals = SDL_SCANCODE_EQUALS, KeyLeftBracket = SDL_SCANCODE_LEFTBRACKET,
	KeyRightBracket = SDL_SCANCODE_RIGHTBRACKET, KeyBackslash = SDL_SCANCODE_BACKSLASH,
	KeySemicolon = SDL_SCANCODE_SEMICOLON, KeyApostrophe = SDL_SCANCODE_APOSTROPHE, KeyGrave = SDL_SCANCODE_GRAVE,
	KeyComma = SDL_SCANCODE_COMMA, KeyPeriod = SDL_SCANCODE_PERIOD, KeySlash = SDL_SCANCODE_SLASH,
	KeyCapsLock = SDL_SCANCODE_CAPSLOCK,

	KeyF1 = SDL_SCANCODE_F1, KeyF2 = SDL_SCANCODE_F2, KeyF3 = SDL_SCANCODE_F3, KeyF4 = SDL_SCANCODE_F4,
	KeyF5 = SDL_SCANCODE_F5, KeyF6 = SDL_SCANCODE_F6, KeyF7 = SDL_SCANCODE_F7, KeyF8 = SDL_SCANCODE_F8,
	KeyF9 = SDL_SCANCODE_F9, KeyF10 = SDL_SCANCODE_F10, KeyF11 = SDL_SCANCODE_F11, KeyF12 = SDL_SCANCODE_F12,

	KeyPrintScreen = SDL_SCANCODE_PRINTSCREEN, KeyScrollLock = SDL_SCANCODE_SCROLLLOCK, KeyPause = SDL_SCANCODE_PAUSE,
	KeyInsert = SDL_SCANCODE_INSERT, KeyHome = SDL_SCANCODE_HOME, KeyPageUp = SDL_SCANCODE_PAGEUP,
	KeyDelete = SDL_SCANCODE_DELETE, KeyEnd = SDL_SCANCODE_END, KeyPageDown = SDL_SCANCODE_PAGEDOWN,
	KeyRight = SDL_SCANCODE_RIGHT, KeyLeft = SDL_SCANCODE_LEFT, KeyDown = SDL_SCANCODE_DOWN, KeyUp = SDL_SCANCODE_UP,
	KeyNumLockClear = SDL_SCANCODE_NUMLOCKCLEAR,
	
	KeyScancodeCount = SDL_NUM_SCANCODES
} Key; //! Enum for the scancodes of each key

//! Initialzes the event handler
void EventHandlerInitialize(void);

/*! Processes the incoming events, this must only be called once per a frame */
void EventHandlerPoll(void);

/*!
 Returns true on the exact frame that the specified key is pressed on
 @param key The key to detect
 @return True if the specified key was pressed on the frame that EventHandlerPoll was called on, false on any other frame
 */
bool EventHandlerOnKeyPressed(Key key);

//! Returns true when any key is pressed
bool EventHandlerOnAnyKeyPressed(void);

/*!
 Returns true on the exact frame that the specified key is released on
 @param key The key to detect
 @return True if the specified key was released on the frame that EventHandlerPoll was called on, false on any other frame
 */
bool EventHandlerOnKeyReleased(Key key);

//! Returns true when any key is released
bool EventHandlerOnAnyKeyReleased(void);

/*!
 Returns true if the specified key is currently pressed
 @param key The key to detect
 @return True if the specified key was pressed but not yet released, false otherwise
 */
bool EventHandlerIsKeyDown(Key key);

bool EventHandlerOnWindowShown(void);

bool EventHandlerOnWindowHidden(void);

bool EventHandlerOnWindowExposed(void);

bool EventHandlerOnWindowMoved(void);

bool EventHandlerOnWindowResized(void);

bool EventHandlerOnWindowMinimized(void);

bool EventHandlerOnWindowMaximized(void);

bool EventHandlerOnWindowRestored(void);

bool EventHandlerOnWindowEnter(void);

bool EventHandlerOnWindowLeave(void);

bool EventHandlerOnWindowFocusGained(void);

bool EventHandlerOnWindowFocusLost(void);

bool EventHandlerOnWindowClose(void);

/*! Cleans up EventHandler, should be called at the end of the program */
void EventHandlerDeinitialize(void);

#endif
