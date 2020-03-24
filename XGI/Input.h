#ifndef Input_h
#define Input_h

#include <SDL2/SDL.h>
#include "LinearMath.h"

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
	
	KeyLeftControl = SDL_SCANCODE_LCTRL,
	KeyLeftShift = SDL_SCANCODE_LSHIFT,
	KeyLeftAlt = SDL_SCANCODE_LALT,
	KeyLeftCommand = SDL_SCANCODE_LGUI,
	KeyRightControl = SDL_SCANCODE_RCTRL,
	KeyRightShift = SDL_SCANCODE_RSHIFT,
	KeyRightAlt = SDL_SCANCODE_RALT,
	KeyRightCommand = SDL_SCANCODE_RGUI,
	
	KeyScancodeCount = SDL_NUM_SCANCODES
} Key;

typedef enum MouseButton
{
	MouseButtonLeft = SDL_BUTTON_LEFT,
	MouseButtonMiddle = SDL_BUTTON_MIDDLE,
	MouseButtonRight = SDL_BUTTON_RIGHT,
	MouseButtonX1 = SDL_BUTTON_X1,
	MouseButtonX2 = SDL_BUTTON_X2,
	MouesButtonCount,
} MouseButton;

typedef enum ControllerHatPosition
{
	ControllerHatPositionUp = SDL_HAT_UP,
	ControllerHatPositionDown = SDL_HAT_DOWN,
	ControllerHatPositionLeft = SDL_HAT_LEFT,
	ControllerHatPositionRight = SDL_HAT_RIGHT,
	ControllerHatPositionLeftUp = SDL_HAT_LEFTUP,
	ControllerHatPositionRightUp = SDL_HAT_RIGHTUP,
	ControllerHatPositionLeftDown = SDL_HAT_LEFTDOWN,
	ControllerHatPositionRightDown = SDL_HAT_RIGHTDOWN,
	ControllerHatPositionCentered = SDL_HAT_CENTERED,
	ControllerHatPositionCount,
} ControllerHatPosition;

typedef enum ControllerPowerLevel
{
	ControllerPowerLevelUnknown = SDL_JOYSTICK_POWER_UNKNOWN,
	ControllerPowerLevelEmpty = SDL_JOYSTICK_POWER_EMPTY,
	ControllerPowerLevelLow = SDL_JOYSTICK_POWER_LOW,
	ControllerPowerLevelMedium = SDL_JOYSTICK_POWER_MEDIUM,
	ControllerPowerLevelFull = SDL_JOYSTICK_POWER_FULL,
	ControllerPowerLevelWired = SDL_JOYSTICK_POWER_WIRED,
	ControllerPowerLevelMax = SDL_JOYSTICK_POWER_MAX,
	ControllerPowerLevelCount,
} ControllerPowerLevel;

/// Detects if a key is pressed
/// \param key The key to detect
/// \return whether or not the key is pressed
bool KeyboardIsKeyPressed(Key key);

/// Detects if a mouse button is pressed
/// \param button The mouse button to detect
/// \return Whether or not the button is pressed
bool MouseIsButtonPressed(MouseButton button);

/// Sets the mouse position relative to the window
/// \param position The position to set
void MouseSetLocalPosition(Vector2 position);

/// Gets the mouse position relative to the window
/// \return A vector2 representing the position
Vector2 MouseLocalPosition(void);

/// Sets the mouse position relative to the top left of the display
/// \param position The position to set
void MouseSetGlobalPosition(Vector2 position);

/// Gets the mouse position relative to the top left of the display
/// \return A vector2 representing the position
Vector2 MouseGlobalPosition(void);

/// Gets the difference in position since the last time this function was called
/// \return A vector2 representing the delta position
Vector2 MouseDeltaPosition(void);

/// Sets whether or not the mouse is shown
/// \param shown True or false
void MouseSetShown(bool shown);

/// Sets whether or not the mouse is captured within the window
/// \param captured True or false
void MouseSetCaptured(bool captured);

/// Gets the power level of the specified controller
/// \param controller The controller device id
/// \return Enumeration for power level
ControllerPowerLevel ControllerCurrentPowerLevel(int controller);

/// Gets the position of an axis on a controller
/// \param controller The controller device id
/// \param axis The axis id
/// \return The axis position
short ControllerAxisPosition(int controller, int axis);

/// Gets the delta position of a ball on a controller
/// \param controller The controller device id
/// \param ball The ball id
/// \return A vector2 representing position
Vector2 ControllerBallDeltaPosition(int controller, int ball);

/// Checks whether or not a button on a controller is pressed
/// \param controller The controller device id
/// \param button The button id
/// \return Whether or not it's pressed
bool ControllerIsButtonPressed(int controller, int button);

/// Gets the position of a hat on a controller
/// \param controller The controller device id
/// \param hat The hat id
/// \return The hat position
ControllerHatPosition ControllerCurrentHatPosition(int controller, int hat);

/// Gets the name of a controller
/// \param controller The controller device id
/// \return The name as a string
const char * ControllerName(int controller);

/// Gets the number of axes on a controller
/// \param controller The controller device id
/// \return The number of axes
int ControllerAxisCount(int controller);

/// Gets the number of balls on a controller
/// \param controller The controller device id
/// \return The number of balls
int ControllerBallCount(int controller);

/// Gets the number of buttons on a controller
/// \param controller The controller device id
/// \return The number of button
int ControllerButtonCount(int controller);

/// Gets the number of hats on a controller
/// \param controller The controller device id
/// \return The number of hats
int ControllerHatCount(int controller);

/// Gets the number of controllers connected
/// \return The number of controllers
int ControllerCount(void);


#endif
