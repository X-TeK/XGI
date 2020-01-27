#ifndef Math3D_h
#define Math3D_h

#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include "List.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define pi 3.14159265

typedef float Scalar;

typedef struct Vector4 { Scalar X, Y, Z, W; } Vector4; //! A 4 value scalar which is used for vector 2, 3 and 4 for alignment reasons
typedef Vector4 Vector2;
typedef Vector4 Vector3;

extern Vector2 Vector2Zero; //! (0, 0)
extern Vector2 Vector2One; //! (1, 1)
extern Vector2 Vector2Up; //! (1, 0)
extern Vector2 Vector2Right; //! (0, 1)

//! Adds two vectors (x + x, y + y)
inline Vector2 Vector2Add(Vector2 v1, Vector2 v2) { return (Vector2) { v1.X + v2.X, v1.Y + v2.Y }; }
//! Subtracts two vectors (x - x, y - y)
inline Vector2 Vector2Subtract(Vector2 v1, Vector2 v2) { return (Vector2) { v1.X - v2.X, v1.Y - v2.Y }; }
//! Multiplies two vectors (x * x, y * y)
inline Vector2 Vector2Multiply(Vector2 v1, Vector2 v2) { return (Vector2) { v1.X* v2.X, v1.Y* v2.Y }; }
//! Divides two vectors (x / x, y / y)
inline Vector2 Vector2Divide(Vector2 v1, Vector2 v2) { return (Vector2) { v1.X / v2.X, v1.Y / v2.Y }; }
//! Adds a scalar to a vector (x + s, y + s)
inline Vector2 Vector2AddScalar(Vector2 v, Scalar s) { return (Vector2) { v.X + s, v.Y + s }; }
//! Subtracts a scalar from a vector (x - s, y - s)
inline Vector2 Vector2SubtractScalar(Vector2 v, Scalar s) { return (Vector2) { v.X - s, v.Y - s }; }
//! Multiplies a vector by a scalar (x * s, y * s)
inline Vector2 Vector2MultiplyScalar(Vector2 v, Scalar s) { return (Vector2) { v.X* s, v.Y* s }; }
//! Divides a vector by a scalar (x / s, y / s)
inline Vector2 Vector2DivideScalar(Vector2 v, Scalar s) { return (Vector2) { v.X / s, v.Y / s }; }
//! Negates a vector (-x, -y)
inline Vector2 Vector2Negate(Vector2 v) { return (Vector2) { -v.X, -v.Y }; }
//! Dot product of two vectors
inline Scalar Vector2Dot(Vector2 v1, Vector2 v2) { return v1.X * v2.X + v1.Y * v2.Y; }
//! Length of a vector (distance to zero)
inline Scalar Vector2Length(Vector2 v) { return (Scalar)sqrt(v.X * v.X + v.Y * v.Y); }
//! Angle between two vectors
inline Scalar Vector2Angle(Vector2 v1, Vector2 v2) { return (Scalar)acos(Vector2Dot(v1, v2) / (Vector2Length(v1) * Vector2Length(v2))); }
//! Distance between two vectors
inline Scalar Vector2Distance(Vector2 v1, Vector2 v2) { return Vector2Length(Vector2Subtract(v2, v1)); }
//! Normalizes a vector
inline Vector2 Vector2Normalize(Vector2 v) { return Vector2DivideScalar(v, Vector2Length(v)); }

extern Vector3 Vector3Zero; //! (0, 0, 0)
extern Vector3 Vector3One; //! (1, 1, 1)
extern Vector3 Vector3Up; //! (0, 1, 0)
extern Vector3 Vector3Right; //! (1, 0, 0)
extern Vector3 Vector3Forward; //! (0, 0, 1)

//! Adds two vectors (x + x, y + y, z + z)
inline Vector3 Vector3Add(Vector3 v1, Vector3 v2) { return (Vector3) { v1.X + v2.X, v1.Y + v2.Y, v1.Z + v2.Z }; }
//! Subtracts two vectors (x - x, y - y, z - z)
inline Vector3 Vector3Subtract(Vector3 v1, Vector3 v2) { return (Vector3) { v1.X - v2.X, v1.Y - v2.Y, v1.Z - v2.Z }; }
//! Multiplies two vectors (x * x, y * y, z * z)
inline Vector3 Vector3Multiply(Vector3 v1, Vector3 v2) { return (Vector3) { v1.X * v2.X, v1.Y * v2.Y, v1.Z * v2.Z }; }
//! Divides two vectors (x / x, y / y, z / z)
inline Vector3 Vector3Divide(Vector3 v1, Vector3 v2) { return (Vector3) { v1.X / v2.X, v1.Y / v2.Y, v1.Z / v2.Z }; }
//! Adds a scalar to a vector (x + s, y + s, z + s)
inline Vector3 Vector3AddScalar(Vector3 v, Scalar s) { return (Vector3) { v.X + s, v.Y + s, v.Z + s }; }
//! Subtracts a scalar from a vector (x - s, y - s, z - s)
inline Vector3 Vector3SubtractScalar(Vector3 v, Scalar s) { return (Vector3) { v.X - s, v.Y - s, v.Z - s }; }
//! Multiplies a vector by a scalar (x * s, y * s, z * s)
inline Vector3 Vector3MultiplyScalar(Vector3 v, Scalar s) { return (Vector3) { v.X * s, v.Y * s, v.Z * s }; }
//! Divides a vector by a scalar (x / s, y / s, z / s)
inline Vector3 Vector3DivideScalar(Vector3 v, Scalar s) { return (Vector3) { v.X / s, v.Y / s, v.Z / s }; }
//! Cross product between two vectors
inline Vector3 Vector3Cross(Vector3 v1, Vector3 v2) { return (Vector3) { v1.Y * v2.Z - v1.Z * v2.Y, v1.Z * v2.X - v1.X * v2.Z, v1.X * v2.Y - v1.Y * v2.X }; }
//! Negates a vector (-x, -y, -z)
inline Vector3 Vector3Negate(Vector3 v) { return (Vector3) { -v.X, -v.Y, -v.Z }; }
//! Dot product between two vectors
inline Scalar Vector3Dot(Vector3 v1, Vector3 v2) { return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z; }
//! Length of a vector (distance to zero)
inline Scalar Vector3Length(Vector3 v) { return (Scalar)sqrt(v.X * v.X + v.Y * v.Y + v.Z * v.Z); }
//! Angle between two vectors
inline Scalar Vector3Angle(Vector3 v1, Vector3 v2) { return (Scalar)acos(Vector3Dot(v1, v2) / (Vector3Length(v1) * Vector3Length(v2))); }
//! Distance between two vectors
inline Scalar Vector3Distance(Vector3 v1, Vector3 v2) { return Vector3Length(Vector3Subtract(v2, v1)); }
//! Normalizes a vector
inline Vector3 Vector3Normalize(Vector3 v) { return Vector3DivideScalar(v, Vector3Length(v)); }
inline Vector3 Vector3Lerp(Vector3 v1, Vector3 v2, Scalar t) { return Vector3Add(Vector3MultiplyScalar(v1, t), Vector3MultiplyScalar(v2, 1.0 - t)); }

//! Linear interpolate between two vectors: v1 * t + v2 * (1.0 - t)
Vector3 Vector3Lerp(Vector3 v1, Vector3 v2, Scalar t);

typedef struct Matrix4x4
{
	Scalar M00, M10, M20, M30;
	Scalar M01, M11, M21, M31;
	Scalar M02, M12, M22, M32;
	Scalar M03, M13, M23, M33;
} Matrix4x4; //! 4x4 matrix, each row is 0n, each column is n0

extern Matrix4x4 Matrix4x4Identity; //! Identitiy matrix
/*!
 Multiplies two matrices, the order matters in which you multiply
 @param l The left operand
 @param r The right operand
 */
inline Matrix4x4 Matrix4x4Multiply(Matrix4x4 l, Matrix4x4 r)
{
	return (Matrix4x4)
	{
		r.M00* l.M00 + r.M10 * l.M01 + r.M20 * l.M02 + r.M30 * l.M03,
			r.M00* l.M10 + r.M10 * l.M11 + r.M20 * l.M12 + r.M30 * l.M13,
			r.M00* l.M20 + r.M10 * l.M21 + r.M20 * l.M22 + r.M30 * l.M23,
			r.M00* l.M30 + r.M10 * l.M31 + r.M20 * l.M32 + r.M30 * l.M33,
			r.M01* l.M00 + r.M11 * l.M01 + r.M21 * l.M02 + r.M31 * l.M03,
			r.M01* l.M10 + r.M11 * l.M11 + r.M21 * l.M12 + r.M31 * l.M13,
			r.M01* l.M20 + r.M11 * l.M21 + r.M21 * l.M22 + r.M31 * l.M23,
			r.M01* l.M30 + r.M11 * l.M31 + r.M21 * l.M32 + r.M31 * l.M33,
			r.M02* l.M00 + r.M12 * l.M01 + r.M22 * l.M02 + r.M32 * l.M03,
			r.M02* l.M10 + r.M12 * l.M11 + r.M22 * l.M12 + r.M32 * l.M13,
			r.M02* l.M20 + r.M12 * l.M21 + r.M22 * l.M22 + r.M32 * l.M23,
			r.M02* l.M30 + r.M12 * l.M31 + r.M22 * l.M32 + r.M32 * l.M33,
			r.M03* l.M00 + r.M13 * l.M01 + r.M23 * l.M02 + r.M33 * l.M03,
			r.M03* l.M10 + r.M13 * l.M11 + r.M23 * l.M12 + r.M33 * l.M13,
			r.M03* l.M20 + r.M13 * l.M21 + r.M23 * l.M22 + r.M33 * l.M23,
			r.M03* l.M30 + r.M13 * l.M31 + r.M23 * l.M32 + r.M33 * l.M33,
	};
}
/*!
 Multiply a matrix by a 4-valued vector
 @param l The left operand
 @param r The right operand
*/
inline Vector4 Matrix4x4MultiplyVector4(Matrix4x4 l, Vector4 r)
{
	return (Vector4)
	{
		r.X* l.M00 + r.Y * l.M01 + r.Z * l.M02 + r.W * l.M03,
			r.X* l.M10 + r.Y * l.M11 + r.Z * l.M12 + r.W * l.M13,
			r.X* l.M20 + r.Y * l.M21 + r.Z * l.M22 + r.W * l.M23,
			r.X* l.M30 + r.Y * l.M31 + r.Z * l.M32 + r.W * l.M33,
	};
}
/*!
 Transform a 3-valued vector with a 4x4 matrix
 @param l The transforming matrix
 @param r The vector to transform
 */
inline Vector3 Matrix4x4MultiplyVector3(Matrix4x4 l, Vector3 r)
{
	return (Vector3)
	{
		r.X* l.M00 + r.Y * l.M01 + r.Z * l.M02 + l.M03,
			r.X* l.M10 + r.Y * l.M11 + r.Z * l.M12 + l.M13,
			r.X* l.M20 + r.Y * l.M21 + r.Z * l.M22 + l.M23,
	};
}
//! Creates a matrix that translates a point
inline Matrix4x4 Matrix4x4FromTranslate(Vector3 v)
{
	return (Matrix4x4)
	{
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		v.X, v.Y, v.Z, 1.0
	};
}
//! Creates a matrix that scales a point
inline Matrix4x4 Matrix4x4FromScale(Vector3 v)
{
	return (Matrix4x4)
	{
		v.X, 0.0, 0.0, 0.0,
		0.0, v.Y, 0.0, 0.0,
		0.0, 0.0, v.Z, 0.0,
		0.0, 0.0, 0.0, 1.0
	};
}
/*!
 Creates a matrix that rotates a point around an axis
 @param v The axis to rotate around
 @param s The angle to rotate
 */
inline Matrix4x4 Matrix4x4FromAxisAngle(Vector3 v, Scalar a)
{
	Scalar c, c2, s, xx, xy, xz, yy, yz, zz;
	c = cos(a);
	c2 = 1.0 - c;
	s = sin(a);
	v = Vector3Normalize(v);
	xx = v.X * v.X;
	xy = v.X * v.Y;
	xz = v.X * v.Z;
	yy = v.Y * v.Y;
	yz = v.Y * v.Z;
	zz = v.Z * v.Z;
	return (Matrix4x4)
	{
		(xx + (yy + zz) * c),
		(xy * c2 + v.Z * s),
		(xz * c2 - v.Y * s),
		0.0f,
		(xy * c2 - v.Z * s),
		(yy + (xx + zz) * c),
		(yz * c2 + v.X * s),
		0.0f,
		(xz * c2 + v.Y * s),
		(yz * c2 - v.X * s),
		(zz + (xx + yy) * c),
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};
}
//! Creates a rotation matrix from euler angles in the order z, y, x
inline Matrix4x4 Matrix4x4FromEuler(Vector3 v)
{
	Matrix4x4 matrix = Matrix4x4Identity;
	matrix = Matrix4x4Multiply(Matrix4x4FromAxisAngle((Vector3) { 0.0, 0.0, 1.0 }, v.Z), matrix);
	matrix = Matrix4x4Multiply(Matrix4x4FromAxisAngle((Vector3) { 0.0, 1.0, 0.0 }, v.X), matrix);
	matrix = Matrix4x4Multiply(Matrix4x4FromAxisAngle((Vector3) { 1.0, 0.0, 0.0 }, v.Y), matrix);
	return matrix;
}
/*!
 Creates a projection matrix
 @param fov The field of view
 @param w Width of the camera
 @param h Height of the camera
 @param n Near clipping plane distance
 @param f Far clipping plane distance
 */
inline Matrix4x4 Matrix4x4FromPerspective(Scalar fov, Scalar w, Scalar h, Scalar near, Scalar far)
{
	Scalar f = 1.0f / tan(0.5f * fov);
	return (Matrix4x4)
	{
		f / (w / h), 0.0f, 0.0f, 0.0f,
		0.0f, -f, 0.0f, 0.0f,
		0.0f, 0.0f, -far / (near - far), 1.0f,
		0.0f, 0.0f, (near * far) / (near - far), 0.0f
	};
}
/*!
 Creates a rotation matrix for a camera looking at a target
 @param position The camera position
 @param target The position the camera is looking at
 @param up The up vector
 */
inline Matrix4x4 Matrix4x4FromLookAt(Vector3 position, Vector3 target, Vector3 up)
{
	Vector3 f = Vector3Normalize(Vector3Subtract(target, position));
	Vector3 r = Vector3Normalize(Vector3Cross(up, f));
	Vector3 u = Vector3Normalize(Vector3Cross(f, r));
	return (Matrix4x4)
	{
		r.X, u.X, f.X, 0.0,
		r.Y, u.Y, f.Y, 0.0,
		r.Z, u.Z, f.Z, 0.0,
		-Vector3Dot(r, position),
		-Vector3Dot(u, position),
		-Vector3Dot(f, position),
		1.0
	};
}
/*!
 Creates a rotation matrix from a direction vector
 @param direction The direction vector
 @param up The up vector
 */
inline Matrix4x4 Matrix4x4FromDirection(Vector3 direction, Vector3 up)
{
	Vector3 f = Vector3Normalize(direction);
	Vector3 r = Vector3Normalize(Vector3Cross(up, f));
	Vector3 u = Vector3Cross(f, r);
	return (Matrix4x4)
	{
		r.X, u.X, f.X, 0.0,
		r.Y, u.Y, f.Y, 0.0,
		r.Z, u.Z, f.Z, 0.0,
		0.0, 0.0, 0.0, 1.0
	};
}

typedef struct Color { unsigned char R, G, B, A; } Color; //! RGBA color using 1 byte per a component, use vector4 for float values
/*!
 Convert a 32-bit hexadecimal number into a color
 @param hex A hexadecimal number e.g. 0xff4800ff --> (255, 72, 0, 255)
 */
inline Color ColorFromHex(unsigned int hex)
{
	return (Color)
	{
		(hex & 0xff000000) >> 24,
		(hex & 0x00ff0000) >> 16,
		(hex & 0x0000ff00) >> 8,
		(hex & 0x000000ff) >> 0
	};
}
//! Converts from a vector4 floating point color
inline Color ColorFromVector4(Vector4 rgba)
{
	return (Color)
	{
		(uint8_t)(rgba.X * 256.0),
		(uint8_t)(rgba.Y * 256.0),
		(uint8_t)(rgba.Z * 256.0),
		(uint8_t)(rgba.W * 256.0),
	};
}
//! Converts to a vector4 floating point color
inline Vector4 ColorToVector4(Color color)
{
	return (Vector4)
	{
		color.R / 256.0,
		color.G / 256.0,
		color.B / 256.0,
		color.A / 256.0,
	};
}
extern Color ColorBlack; //! (0, 0, 0, 255)
extern Color ColorWhite; //! (255, 255, 255, 255)
extern Color ColorRed; //! (255, 0, 0 255)
extern Color ColorGreen; //! (0, 255, 0, 255)
extern Color ColorBlue; //! (0, 0, 255, 255)

#endif
