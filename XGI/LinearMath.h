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
Vector2 Vector2Add(Vector2 v1, Vector2 v2);
//! Subtracts two vectors (x - x, y - y)
Vector2 Vector2Subtract(Vector2 v1, Vector2 v2);
//! Multiplies two vectors (x * x, y * y)
Vector2 Vector2Multiply(Vector2 v1, Vector2 v2);
//! Divides two vectors (x / x, y / y)
Vector2 Vector2Divide(Vector2 v1, Vector2 v2);
//! Adds a scalar to a vector (x + s, y + s)
Vector2 Vector2AddScalar(Vector2 v, Scalar s);
//! Subtracts a scalar from a vector (x - s, y - s)
Vector2 Vector2SubtractScalar(Vector2 v, Scalar s);
//! Multiplies a vector by a scalar (x * s, y * s)
Vector2 Vector2MultiplyScalar(Vector2 v, Scalar s);
//! Divides a vector by a scalar (x / s, y / s)
Vector2 Vector2DivideScalar(Vector2 v, Scalar s);
//! Negates a vector (-x, -y)
Vector2 Vector2Negate(Vector2 v);
//! Dot product of two vectors
Scalar Vector2Dot(Vector2 v1, Vector2 v2);
//! Length of a vector (distance to zero)
Scalar Vector2Length(Vector2 v);
//! Distance between two vectors
Scalar Vector2Distance(Vector2 v1, Vector2 v2);
//! Angle between two vectors
Scalar Vector2Angle(Vector2 v1, Vector2 v2);
//! Normalizes a vector
Vector2 Vector2Normalize(Vector2 v);

extern Vector3 Vector3Zero; //! (0, 0, 0)
extern Vector3 Vector3One; //! (1, 1, 1)
extern Vector3 Vector3Up; //! (0, 1, 0)
extern Vector3 Vector3Right; //! (1, 0, 0)
extern Vector3 Vector3Forward; //! (0, 0, 1)
//! Adds two vectors (x + x, y + y, z + z)
Vector3 Vector3Add(Vector3 v1, Vector3 v2);
//! Subtracts two vectors (x - x, y - y, z - z)
Vector3 Vector3Subtract(Vector3 v1, Vector3 v2);
//! Multiplies two vectors (x * x, y * y, z * z)
Vector3 Vector3Multiply(Vector3 v1, Vector3 v2);
//! Divides two vectors (x / x, y / y, z / z)
Vector3 Vector3Divide(Vector3 v1, Vector3 v2);
//! Adds a scalar to a vector (x + s, y + s, z + s)
Vector3 Vector3AddScalar(Vector3 v, Scalar s);
//! Subtracts a scalar from a vector (x - s, y - s, z - s)
Vector3 Vector3SubtractScalar(Vector3 v, Scalar s);
//! Multiplies a vector by a scalar (x * s, y * s, z * s)
Vector3 Vector3MultiplyScalar(Vector3 v, Scalar s);
//! Divides a vector by a scalar (x / s, y / s, z / s)
Vector3 Vector3DivideScalar(Vector3 v, Scalar s);
//! Negates a vector (-x, -y, -z)
Vector3 Vector3Negate(Vector3 v);
//! Cross product between two vectors
Vector3 Vector3Cross(Vector3 v1, Vector3 v2);
//! Dot product between two vectors
Scalar Vector3Dot(Vector3 v1, Vector3 v2);
//! Length of a vector (distance to zero)
Scalar Vector3Length(Vector3 v);
//! Distance between two vectors
Scalar Vector3Distance(Vector3 v1, Vector3 v2);
//! Angle between two vectors
Scalar Vector3Angle(Vector3 v1, Vector3 v2);
//! Normalizes a vector
Vector3 Vector3Normalize(Vector3 v);
//! Rotates v1 around v2 along the specified axis and angle
Vector3 Vector3Rotate(Vector3 v1, Vector3 v2, Vector3 axis, Scalar angle);
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
//! Creates a matrix that translates a point
Matrix4x4 Matrix4x4FromTranslate(Vector3 v);
//! Creates a matrix that scales a point
Matrix4x4 Matrix4x4FromScale(Vector3 v);
/*!
 Creates a matrix that rotates a point around an axis
 @param v The axis to rotate around
 @param s The angle to rotate
 */
Matrix4x4 Matrix4x4FromAxisAngle(Vector3 v, Scalar s);
//! Creates a rotation matrix from euler angles in the order z, y, x
Matrix4x4 Matrix4x4FromEuler(Vector3 v);
/*!
 Creates a projection matrix
 @param fov The field of view
 @param w Width of the camera
 @param h Height of the camera
 @param n Near clipping plane distance
 @param f Far clipping plane distance
 */
Matrix4x4 Matrix4x4FromPerspective(Scalar fov, Scalar w, Scalar h, Scalar n, Scalar f);
/*!
 Creates a rotation matrix for a camera looking at a target
 @param position The camera position
 @param target The position the camera is looking at
 @param up The up vector
 */
Matrix4x4 Matrix4x4FromLookAt(Vector3 position, Vector3 target, Vector3 up);
/*!
 Creates a rotation matrix from a direction vector
 @param direction The direction vector
 @param up The up vector
 */
Matrix4x4 Matrix4x4FromDirection(Vector3 direction, Vector3 up);
/*!
 Multiplies two matrices, the order matters in which you multiply
 @param l The left operand
 @param r The right operand
 */
Matrix4x4 Matrix4x4Multiply(Matrix4x4 l, Matrix4x4 r);
/*!
 Multiply a matrix by a 4-valued vector
 @param l The left operand
 @param r The right operand
*/
Vector4 Matrix4x4MultiplyVector4(Matrix4x4 l, Vector4 r);
/*!
 Transform a 3-valued vector with a 4x4 matrix
 @param l The transforming matrix
 @param r The vector to transform
 */
Vector3 Matrix4x4MultiplyVector3(Matrix4x4 l, Vector3 r);

typedef struct Color { unsigned char R, G, B, A; } Color; //! RGBA color using 1 byte per a component, use vector4 for float values
/*!
 Convert a 32-bit hexadecimal number into a color
 @param hex A hexadecimal number e.g. 0xff4800ff --> (255, 72, 0, 255)
 */
Color ColorFromHex(unsigned int hex);
//! Converts from a vector4 floating point color
Color ColorFromVector4(Vector4 v);
//! Converts to a vector4 floating point color
Vector4 ColorToVector4(Color color);
extern Color ColorBlack; //! (0, 0, 0, 255)
extern Color ColorWhite; //! (255, 255, 255, 255)
extern Color ColorRed; //! (255, 0, 0 255)
extern Color ColorGreen; //! (0, 255, 0, 255)
extern Color ColorBlue; //! (0, 0, 255, 255)

#endif
