#ifndef Window_h
#define Window_h

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "List.h"
#include "LinearMath.h"

typedef struct WindowConfigure
{
	/// Width of the window
	int Width;
	/// Height of the window
	int Height;
	/// Title of the window
	const char * Title;
	/// Whether or not it's fullscreen
	bool FullScreen;
	/// Whether or not the window is visible
	bool Hidden;
	/// Whether or not the window has borders
	bool Borderless;
	/// Whether or not the window is resizable
	bool Resizable;
	/// Whether or not to initialize the window minimized
	bool Minimized;
	/// Whether or not to initialize the window maximized
	bool Maximized;
	/// Whether or not to initialize with the input grabbed
	bool InputGrabbed;
	/// Whether or not to initialize with input focus
	bool InputFocus;
	/// Whether or not to initialize with mouse focus
	bool MouseFocus;
	/// Whether or not the window needs to be adjusted for highdpi
	bool HighDPI;
	/// Whether or not to initialize with the mouse captured
	bool MouseCapture;
	/// Whether or not the window is always on top
	bool AlwaysOnTop;
} WindowConfigure;

struct Window
{
	SDL_Window * Handle;
	int Width;
	int Height;
	const char * Title;
	bool Running;
} extern Window;

/// This should not be called, it's automatically called in XGIInitialize
void WindowInitialize(WindowConfigure flags);

/// Use as the exit condition for the main loop, it's initialized as true
bool WindowRunning(void);

/// Exits the main loop by setting WindowRunning to false
void WindowExitLoop(void);

/// Disables the screen saver from running while the application is open
void WindowDisableScreenSaver(void);

/// Enables the screen saver to run while the application is open
void WindowEnableScreenSaver(void);

/// Gets the width and height of a display
/// \param displayIndex The index of the desired display
/// \return A vector2 containing the width and height
Vector2 WindowDisplayDimensions(int displayIndex);

/// Gets the usable width and height of a display
/// \param displayIndex The display index
/// \return A vector2 containing the width and height
Vector2 WindowUsableDisplayDimensions(int displayIndex);

/// Gets the DPI of a chosen display
/// \param displayIndex The index of the disired display
/// \return A vector3 containing .X = horizontal dpi, .Y = vertical dpi, .Z = diagonal dpi
Vector3 WindowDisplayDPI(int displayIndex);

/// Gets the name of a display
/// \param displayIndex The display index
/// \return The display name
const char * WindowDisplayName(int displayIndex);

/// Gets the number of displays
/// \return The display count
int WindowDisplayCount(void);

/// Gets the size of each border around the window
/// \return A vector4: .X = left, .Y = right, .Z = top, .W = bottom
Vector4 WindowBorderSize(void);

/// Gets the window brightness
/// \return 0.0 is the darkest 1.0 is the brightest
float WindowBrightness(void);

/// Gets the display index that the current window is on
/// \return The display index
int WindowDisplayIndex(void);

/// Gets the maximum size of the window
/// \return A vector2 of the size
Vector2 WindowMaximumSize(void);

/// Gets the minimum size of the window
/// \return A vector2 of the size
Vector2 WindowMinimumSize(void);

/// Gets the opacity of the window
/// \return The opacity from [0, 1]
float WindowOpacity(void);

/// Gets the window position
/// \return A vector2 of the window position
Vector2 WindowPosition(void);

/// Gets the window dimensions
/// \return A vector2 of the window size
Vector2 WindowSize(void);

/// Gets the window title
/// \return The window title
const char * WindowTitle(void);

/// Hides the window
void WindowHide(void);

/// Shows the window
void WindowShow(void);

/// Maximizes the window
void WindowMaximize(void);

/// Minimizes the window
void WindowMinimize(void);

/// Raises the window
void WindowRaise(void);

/// Restores the window
void WindowRestore(void);

/// Sets whether or not it's bordered
/// \param bordered Whether or not it's bordered
void WindowSetBordered(bool bordered);

/// Sets the window brightness
/// \param brightness Sets the brightness of the window
void WindowSetBrightness(float brightness);

/// Sets the window to fullscreen
void WindowSetFullScreen(void);

/// Sets whether or not the window is grabbed
/// \param grabbed whether or not it should be grabbed
void WindowSetGrabbed(bool grabbed);

/// Sets the input focus to the window
void WindowSetInputFocus(void);

/// Sets the maximum size of the window
/// \param size The size of the window
void WindowSetMaximumSize(Vector2 size);

/// Sets the minimum size of the window
/// \param size The size of the window
void WindowSetMinimumSize(Vector2 size);

/// Sets the opacity of the window
/// \param opacity Opacity to set between [0, 1]
void WindowSetOpacity(float opacity);

/// Sets the position of the window
/// \param position Position on display to set
void WindowSetPosition(Vector2 position);

/// Sets whether or not the window is resizable
/// \param resizable Whether or not it's resizable
void WindowSetResizable(bool resizable);

/// Sets the window dimensions
/// \param size Size to set
void WindowSetSize(Vector2 size);

/// Sets the window title
/// \param title The title to set
void WindowSetTitle(const char * title);

/// This should not be called, it's automatically called in XGIDeinitialize
void WindowDeinitialize(void);

#endif
