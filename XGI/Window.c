#include <stdlib.h>
#include <stdio.h>
#include "Window.h"
#include "log.h"

struct Window Window = { 0 };

void WindowInitialize(WindowConfigure flags)
{
	log_info("Initializing the window...\n");
	Window.Title = flags.Title;
	Window.Width = flags.Width;
	Window.Height = flags.Height;
	Window.Running = true;
	
	unsigned int orFlags = SDL_WINDOW_VULKAN;
	orFlags |= flags.AlwaysOnTop ? SDL_WINDOW_ALWAYS_ON_TOP : orFlags;
	orFlags |= flags.Borderless ? SDL_WINDOW_BORDERLESS : orFlags;
	orFlags |= flags.FullScreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : orFlags;
	orFlags |= flags.Hidden ? SDL_WINDOW_HIDDEN : orFlags;
	orFlags |= flags.HighDPI ? SDL_WINDOW_ALLOW_HIGHDPI : orFlags;
	orFlags |= flags.InputFocus ? SDL_WINDOW_INPUT_FOCUS : orFlags;
	orFlags |= flags.InputGrabbed ? SDL_WINDOW_INPUT_GRABBED : orFlags;
	orFlags |= flags.Maximized ? SDL_WINDOW_MAXIMIZED : orFlags;
	orFlags |= flags.Minimized ? SDL_WINDOW_MINIMIZED : orFlags;
	orFlags |= flags.MouseCapture ? SDL_WINDOW_MOUSE_CAPTURE : orFlags;
	orFlags |= flags.MouseFocus ? SDL_WINDOW_MOUSE_FOCUS : orFlags;
	orFlags |= flags.Resizable ? SDL_WINDOW_RESIZABLE : orFlags;
	Window.Handle = SDL_CreateWindow(Window.Title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, flags.Width, flags.Height, orFlags);
	if (Window.Handle == NULL)
	{
		log_fatal("Failed to create the window\n");
		exit(1);
	}
	log_info("Successfully initialized the window.\n");
}

bool WindowRunning() { return Window.Running; }

void WindowExitLoop()
{
	Window.Running = false;
}

void WindowDisableScreenSaver() { SDL_DisableScreenSaver(); }

void WindowEnableScreenSaver() { SDL_EnableScreenSaver(); }

Vector2 WindowDisplayDimensions(int index)
{
	SDL_Rect dimensions;
	SDL_GetDisplayBounds(index, &dimensions);
	return (Vector2){ dimensions.w, dimensions.h };
}

Vector2 WindowUsableDisplayDimensions(int index)
{
	SDL_Rect dimensions;
	SDL_GetDisplayUsableBounds(index, &dimensions);
	return (Vector2){ dimensions.w, dimensions.h };
}

Vector3 WindowDisplayDPI(int index)
{
	float x, y, z;
	SDL_GetDisplayDPI(index, &x, &y, &z);
	return (Vector3){ x, y, z };
}

const char * WindowDisplayName(int index) { return SDL_GetDisplayName(index); }

int WindowDisplayCount() { return SDL_GetNumVideoDisplays(); }

Vector4 WindowBorderSize()
{
	int t, l, b, r;
	SDL_GetWindowBordersSize(Window.Handle, &t, &l, &b, &r);
	return (Vector4){ l, r, t, b };
}

float WindowBrightness() { return SDL_GetWindowBrightness(Window.Handle); }

int WindowDisplayIndex() { return SDL_GetWindowDisplayIndex(Window.Handle); }

void WindowDeinitialize() { SDL_DestroyWindow(Window.Handle); }

Vector2 WindowMaximumSize()
{
	int w, h;
	SDL_GetWindowMaximumSize(Window.Handle, &w, &h);
	return (Vector2){ w, h };
}

Vector2 WindowMinimumSize()
{
	int w, h;
	SDL_GetWindowMinimumSize(Window.Handle, &w, &h);
	return (Vector2){ w, h };
}

float WindowOpacity()
{
	float opacity;
	SDL_GetWindowOpacity(Window.Handle, &opacity);
	return opacity;
}

Vector2 WindowPosition()
{
	int x, y;
	SDL_GetWindowPosition(Window.Handle, &x, &y);
	return (Vector2){ x, y };
}

Vector2 WindowSize()
{
	return (Vector2){ Window.Width, Window.Height };
}

const char * WindowTitle() { return SDL_GetWindowTitle(Window.Handle); }

void WindowHide() { SDL_HideWindow(Window.Handle); }

void WindowShow() { SDL_ShowWindow(Window.Handle); }

void WindowMaximize() { SDL_MaximizeWindow(Window.Handle); }

void WindowMinimize() { SDL_MinimizeWindow(Window.Handle); }

void WindowRaise() { SDL_RaiseWindow(Window.Handle); }

void WindowRestore() { SDL_RaiseWindow(Window.Handle); }

void WindowSetBordered(bool bordered) { SDL_SetWindowBordered(Window.Handle, bordered); }

void WindowSetBrightness(float brightness) { SDL_SetWindowBrightness(Window.Handle, brightness); }

void WindowSetFullScreen() { SDL_SetWindowFullscreen(Window.Handle, SDL_WINDOW_FULLSCREEN_DESKTOP); }

void WindowSetGrabbed(bool grabbed) { SDL_SetWindowGrab(Window.Handle, grabbed); }

void WindowSetInputFocus() { SDL_SetWindowInputFocus(Window.Handle); }

void WindowSetMaximumSize(Vector2 size) { SDL_SetWindowMaximumSize(Window.Handle, size.X, size.Y); }

void WindowSetMinimumSize(Vector2 size) { SDL_SetWindowMinimumSize(Window.Handle, size.X, size.Y); }

void WindowSetOpacity(float opacity) { SDL_SetWindowOpacity(Window.Handle, opacity); }

void WindowSetPosition(Vector2 position) { SDL_SetWindowPosition(Window.Handle, position.X, position.Y); }

void WindowSetResizable(bool resizable) { SDL_SetWindowResizable(Window.Handle, resizable); }

void WindowSetSize(Vector2 size) { SDL_SetWindowSize(Window.Handle, size.X, size.Y); }

void WindowSetTitle(const char * title) { SDL_SetWindowTitle(Window.Handle, title); }
