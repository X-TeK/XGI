#include "LinearMath.h"

Vector2 Vector2Zero = { 0.0, 0.0 };
Vector2 Vector2One = { 1.0, 1.0 };
Vector2 Vector2Up = { 0.0, 1.0 };
Vector2 Vector2Right = { 1.0, 0.0 };

Vector3 Vector3Zero = { 0.0, 0.0, 0.0 };
Vector3 Vector3One = { 1.0, 1.0, 1.0 };
Vector3 Vector3Up = { 0.0, 1.0, 0.0 };
Vector3 Vector3Right = { 1.0, 0.0, 0.0 };
Vector3 Vector3Forward = { 0.0, 0.0, 1.0 };

Matrix4x4 Matrix4x4Identity =
{
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0,
};

Color ColorBlack = { 0x00, 0x00, 0x00, 0xff };
Color ColorWhite = { 0xff, 0xff, 0xff, 0xff };
Color ColorRed = { 0xff, 0x00, 0x00, 0xff };
Color ColorGreen = { 0x00, 0xff, 0x00, 0xff };
Color ColorBlue = { 0x00, 0x00, 0xff, 0xff };