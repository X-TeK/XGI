#include "Input.h"
#include "Window.h"
#include "log.h"

bool KeyboardIsKeyPressed(Key key)
{
	if (key < 0 || key >= KeyScancodeCount)
	{
		log_fatal("Trying to detect if key is pressed, but Key %i is outside the valid range of enumerations.\n", key);
		exit(1);
	}
	return SDL_GetKeyboardState(NULL)[key];
}

bool MouseIsButtonPressed(MouseButton button)
{
	if (button < 0 || button >= KeyScancodeCount)
	{
		log_fatal("Trying to detect if a mouse button is pressed, but MouseButton %i is outside the valid range of enumerations.\n", button);
		exit(1);
	}
	return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(button);
}

void MouseSetLocalPosition(Vector2 position)
{
	SDL_WarpMouseInWindow(Window.Handle, position.X, position.Y);
}

Vector2 MouseLocalPosition()
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	return (Vector2){ x, y };
}

void MouseSetGlobalPosition(Vector2 position)
{
	SDL_WarpMouseGlobal(position.X, position.Y);
}

Vector2 MouseGlobalPosition()
{
	int x, y;
	SDL_GetGlobalMouseState(&x, &y);
	return (Vector2){ x, y };
}

Vector2 MouseDeltaPosition()
{
	int x, y;
	SDL_GetRelativeMouseState(&x, &y);
	return (Vector2){ x, y };
}

void MouseSetShown(bool shown)
{
	SDL_ShowCursor(shown ? SDL_ENABLE : SDL_DISABLE);
}

void MouseSetCaptured(bool captured)
{
	SDL_SetRelativeMouseMode(captured ? SDL_TRUE : SDL_FALSE);
}

ControllerPowerLevel ControllerCurrentPowerLevel(int controller)
{
	SDL_Joystick * joystick = SDL_JoystickOpen(controller);
	SDL_JoystickPowerLevel power = SDL_JoystickCurrentPowerLevel(joystick);
	SDL_JoystickClose(joystick);
	return (ControllerPowerLevel)power;
}

short ControllerAxisPosition(int controller, int axis)
{
	SDL_Joystick * joystick = SDL_JoystickOpen(controller);
	short value = SDL_JoystickGetAxis(joystick, axis);
	SDL_JoystickClose(joystick);
	return value;
}

Vector2 ControllerBallDeltaPosition(int controller, int ball)
{
	SDL_Joystick * joystick = SDL_JoystickOpen(controller);
	int dx, dy;
	SDL_JoystickGetBall(joystick, ball, &dx, &dy);
	SDL_JoystickClose(joystick);
	return (Vector2){ dx, dy };
}

bool ControllerIsButtonPressed(int controller, int button)
{
	SDL_Joystick * joystick = SDL_JoystickOpen(controller);
	bool value = SDL_JoystickGetButton(joystick, button);
	SDL_JoystickClose(joystick);
	return value;
}

ControllerHatPosition ControllerCurrentHatPosition(int controller, int hat)
{
	SDL_Joystick * joystick = SDL_JoystickOpen(controller);
	unsigned char value = SDL_JoystickGetHat(joystick, hat);
	SDL_JoystickClose(joystick);
	return (ControllerHatPosition)value;
}

const char * ControllerName(int controller)
{
	return SDL_JoystickNameForIndex(controller);
}

int ControllerAxisCount(int controller)
{
	SDL_Joystick * joystick = SDL_JoystickOpen(controller);
	int value = SDL_JoystickNumAxes(joystick);
	SDL_JoystickClose(joystick);
	return value;
}

int ControllerBallCount(int controller)
{
	SDL_Joystick * joystick = SDL_JoystickOpen(controller);
	int value = SDL_JoystickNumBalls(joystick);
	SDL_JoystickClose(joystick);
	return value;
}

int ControllerButtonCount(int controller)
{
	SDL_Joystick * joystick = SDL_JoystickOpen(controller);
	int value = SDL_JoystickNumButtons(joystick);
	SDL_JoystickClose(joystick);
	return value;
}

int ControllerHatCount(int controller)
{
	SDL_Joystick * joystick = SDL_JoystickOpen(controller);
	int value = SDL_JoystickNumHats(joystick);
	SDL_JoystickClose(joystick);
	return value;
}

int ControllerCount()
{
	return SDL_NumJoysticks();
}
