#include "Input.h"
#include "Window.h"

bool InputIsKeyDown(Key key)
{
	return SDL_GetKeyboardState(NULL)[key];
}

bool InputIsMouseButtonPressed(MouseButton button)
{
	return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(button);
}

void InputSetLocalMousePosition(Vector2 position)
{
	SDL_WarpMouseInWindow(Window.Handle, position.X, position.Y);
}

Vector2 InputLocalMousePosition()
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	return (Vector2){ x, y };
}

void InputSetGlobalMousePosition(Vector2 position)
{
	SDL_WarpMouseGlobal(position.X, position.Y);
}

Vector2 InputGlobalMousePosition()
{
	int x, y;
	SDL_GetGlobalMouseState(&x, &y);
	return (Vector2){ x, y };
}

Vector2 InputDeltaMousePosition()
{
	int x, y;
	SDL_GetRelativeMouseState(&x, &y);
	return (Vector2){ x, y };
}

void InputSetMouseShown(bool shown)
{
	SDL_ShowCursor(shown ? SDL_ENABLE : SDL_DISABLE);
}

void InputSetMouseCaptured(bool captured)
{
	SDL_SetRelativeMouseMode(captured ? SDL_TRUE : SDL_FALSE);
}

void InputSetGlobalMouseTracking(bool track)
{
	SDL_CaptureMouse(track ? SDL_TRUE : SDL_FALSE);
}
