#ifndef Window_h
#define Window_h

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "List.h"
#include "LinearMath.h"

//! A structure of possible window flags for initialization
typedef struct WindowConfigure
{
	int Width;
	int Height;
	const char * Title;
	bool FullScreen;
	bool Hidden;
	bool Borderless;
	bool Resizable;
	bool Minimized;
	bool Maximized;
	bool InputGrabbed;
	bool InputFocus;
	bool MouseFocus;
	bool HighDPI;
	bool MouseCapture;
	bool AlwaysOnTop;
} WindowConfigure;

struct Window
{
	SDL_Window * Handle; //! SDL Window pointer
	int Width; //! Width of the window
	int Height; //! Height of the window
	const char * Title; //! Title of the window
	bool Running; //! The main loop exit condition
} extern Window;

/*!
 Initializes the window system, must be called before GraphicsInitialize
 @param flags A structure filled with optional flags
 */
void WindowInitialize(WindowConfigure flags);

//! Exits the main loop
void WindowExitLoop(void);

//! Disables the screen saver from running while the application is open
void WindowDisableScreenSaver(void);

//! Enables the screen saver to run while the application is open
void WindowEnableScreenSaver(void);

/*!
 Gets the width and height of a display
 @param displayIndex The index of the desired display
 @return A vector2 containing the width and height
 */
Vector2 WindowDisplayDimensions(int displayIndex);

/*!
 Gets the usable width and height of a display
 @param displayIndex The display index
 @return A vector2 containing the width and height
 */
Vector2 WindowUsableDisplayDimensions(int displayIndex);

/*!
 Gets the DPI of a chosen display
 @param displayIndex The index of the disired display
 @return A vector3 containing .X = horizontal dpi, .Y = vertical dpi, .Z = diagonal dpi
 */
Vector3 WindowDisplayDPI(int displayIndex);

//! Gets the name of a display
const char * WindowDisplayName(int displayIndex);

//! Gets the number of displays
int WindowDisplayCount(void);

/*!
 Gets the size of each border around the window
 @return A vector4: .X = left, .Y = right, .Z = top, .W = bottom
 */
Vector4 WindowBorderSize(void);

/*! Gets the window brightness
 @return 0.0 is the darkest 1.0 is the brightest
 */
float WindowBrightness(void);

//! Gets the display index that the current window is on
int WindowDisplayIndex(void);

Vector2 WindowMaximumSize(void);

Vector2 WindowMinimumSize(void);

float WindowOpacity(void);

Vector2 WindowPosition(void);

Vector2 WindowSize(void);

const char * WindowTitle(void);

void WindowHide(void);

void WindowShow(void);

void WindowMaximize(void);

void WindowMinimize(void);

void WindowRaise(void);

void WindowRestore(void);

void WindowSetBordered(bool bordered);

void WindowSetBrightness(float brightness);

void WindowSetFullScreen(void);

void WindowSetGrabbed(bool grabbed);

void WindowSetInputFocus(void);

void WindowSetMaximumSize(Vector2 size);

void WindowSetMinimumSize(Vector2 size);

void WindowSetOpacity(float opacity);

void WindowSetPosition(Vector2 position);

void WindowSetResizable(bool resizable);

void WindowSetSize(Vector2 size);

void WindowSetTitle(const char * title);

//! Cleans up and destroys the window
void WindowDeinitialize(void);

#endif
