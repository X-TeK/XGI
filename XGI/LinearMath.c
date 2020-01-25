#include "LinearMath.h"

Vector2 Vector2Zero = { 0.0, 0.0 };
Vector2 Vector2One = { 1.0, 1.0 };
Vector2 Vector2Up = { 0.0, 1.0 };
Vector2 Vector2Right = { 1.0, 0.0 };
inline Vector2 Vector2Add(Vector2 v1, Vector2 v2) { return (Vector2){ v1.X + v2.X, v1.Y + v2.Y }; }
inline Vector2 Vector2Subtract(Vector2 v1, Vector2 v2) { return (Vector2){ v1.X - v2.X, v1.Y - v2.Y }; }
inline Vector2 Vector2Multiply(Vector2 v1, Vector2 v2) { return (Vector2){ v1.X * v2.X, v1.Y * v2.Y }; }
inline Vector2 Vector2Divide(Vector2 v1, Vector2 v2) { return (Vector2){ v1.X / v2.X, v1.Y / v2.Y }; }
inline Vector2 Vector2AddScalar(Vector2 v, Scalar s) { return (Vector2){ v.X + s, v.Y + s }; }
inline Vector2 Vector2SubtractScalar(Vector2 v, Scalar s) { return (Vector2){ v.X - s, v.Y - s }; }
inline Vector2 Vector2MultiplyScalar(Vector2 v, Scalar s) { return (Vector2){ v.X * s, v.Y * s }; }
inline Vector2 Vector2DivideScalar(Vector2 v, Scalar s) { return (Vector2){ v.X / s, v.Y / s }; }
inline Vector2 Vector2Negate(Vector2 v) { return (Vector2){ -v.X, -v.Y }; }
inline Scalar Vector2Dot(Vector2 v1, Vector2 v2) { return v1.X * v2.X + v1.Y * v2.Y; }
inline Scalar Vector2Angle(Vector2 v1, Vector2 v2) { return acos(Vector2Dot(v1, v2) / (Vector2Length(v1) * Vector2Length(v2))); }
inline Scalar Vector2Length(Vector2 v) { return sqrt(v.X * v.X + v.Y * v.Y); }
inline Scalar Vector2Distance(Vector2 v1, Vector2 v2) { return Vector2Length(Vector2Subtract(v2, v1)); }
inline Vector2 Vector2Normalize(Vector2 v) { return Vector2DivideScalar(v, Vector2Length(v)); }

Vector3 Vector3Zero = { 0.0, 0.0, 0.0 };
Vector3 Vector3One = { 1.0, 1.0, 1.0 };
Vector3 Vector3Up = { 0.0, 1.0, 0.0 };
Vector3 Vector3Right = { 1.0, 0.0, 0.0 };
Vector3 Vector3Forward = { 0.0, 0.0, 1.0 };
inline Vector3 Vector3Add(Vector3 v1, Vector3 v2) { return (Vector3) { v1.X + v2.X, v1.Y + v2.Y, v1.Z + v2.Z }; }
inline Vector3 Vector3Subtract(Vector3 v1, Vector3 v2) { return (Vector3) { v1.X - v2.X, v1.Y - v2.Y, v1.Z - v2.Z }; }
inline Vector3 Vector3Multiply(Vector3 v1, Vector3 v2) { return (Vector3) { v1.X * v2.X, v1.Y * v2.Y, v1.Z * v2.Z }; }
inline Vector3 Vector3Divide(Vector3 v1, Vector3 v2) { return (Vector3) { v1.X / v2.X, v1.Y / v2.Y, v1.Z / v2.Z }; }
inline Vector3 Vector3AddScalar(Vector3 v, Scalar s) { return (Vector3) { v.X + s, v.Y + s, v.Z + s }; }
inline Vector3 Vector3SubtractScalar(Vector3 v, Scalar s) { return (Vector3) { v.X - s, v.Y - s, v.Z - s }; }
inline Vector3 Vector3MultiplyScalar(Vector3 v, Scalar s) { return (Vector3) { v.X * s, v.Y * s, v.Z * s }; }
inline Vector3 Vector3DivideScalar(Vector3 v, Scalar s) { return (Vector3) { v.X / s, v.Y / s, v.Z / s }; }
inline Vector3 Vector3Cross(Vector3 v1, Vector3 v2) { return (Vector3) { v1.Y * v2.Z - v1.Z * v2.Y, v1.Z * v2.X - v1.X * v2.Z, v1.X * v2.Y - v1.Y * v2.X }; }
inline Vector3 Vector3Negate(Vector3 v) { return (Vector3) { -v.X, -v.Y, -v.Z }; }
inline Scalar Vector3Dot(Vector3 v1, Vector3 v2) { return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z; }
inline Scalar Vector3Length(Vector3 v) { return sqrt(v.X * v.X + v.Y * v.Y + v.Z * v.Z); }
inline Scalar Vector3Angle(Vector3 v1, Vector3 v2) { return acos(Vector3Dot(v1, v2) / (Vector3Length(v1) * Vector3Length(v2))); }
inline Scalar Vector3Distance(Vector3 v1, Vector3 v2) { return Vector3Length(Vector3Subtract(v2, v1)); }
inline Vector3 Vector3Normalize(Vector3 v) { return Vector3DivideScalar(v, Vector3Length(v)); }
inline Vector3 Vector3Lerp(Vector3 v1, Vector3 v2, Scalar t) { return Vector3Add(Vector3MultiplyScalar(v1, t), Vector3MultiplyScalar(v2, 1.0 - t)); }
inline Vector3 Vector3Rotate(Vector3 v1, Vector3 v2, Vector3 axis, Scalar angle)
{
	return Vector3Add(Matrix4x4MultiplyVector3(Matrix4x4FromAxisAngle(axis, angle), Vector3Subtract(v1, v2)), v2);
}

Matrix4x4 Matrix4x4Identity =
{
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0,
};

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
inline Matrix4x4 Matrix4x4FromEuler(Vector3 v)
{
	Matrix4x4 matrix = Matrix4x4Identity;
	matrix = Matrix4x4Multiply(Matrix4x4FromAxisAngle((Vector3){ 0.0, 0.0, 1.0 }, v.Z), matrix);
	matrix = Matrix4x4Multiply(Matrix4x4FromAxisAngle((Vector3){ 0.0, 1.0, 0.0 }, v.X), matrix);
	matrix = Matrix4x4Multiply(Matrix4x4FromAxisAngle((Vector3){ 1.0, 0.0, 0.0 }, v.Y), matrix);
	return matrix;
}
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
inline Matrix4x4 Matrix4x4Multiply(Matrix4x4 l, Matrix4x4 r)
{
	return (Matrix4x4)
	{
		r.M00 * l.M00 + r.M10 * l.M01 + r.M20 * l.M02 + r.M30 * l.M03,
		r.M00 * l.M10 + r.M10 * l.M11 + r.M20 * l.M12 + r.M30 * l.M13,
		r.M00 * l.M20 + r.M10 * l.M21 + r.M20 * l.M22 + r.M30 * l.M23,
		r.M00 * l.M30 + r.M10 * l.M31 + r.M20 * l.M32 + r.M30 * l.M33,
		r.M01 * l.M00 + r.M11 * l.M01 + r.M21 * l.M02 + r.M31 * l.M03,
		r.M01 * l.M10 + r.M11 * l.M11 + r.M21 * l.M12 + r.M31 * l.M13,
		r.M01 * l.M20 + r.M11 * l.M21 + r.M21 * l.M22 + r.M31 * l.M23,
		r.M01 * l.M30 + r.M11 * l.M31 + r.M21 * l.M32 + r.M31 * l.M33,
		r.M02 * l.M00 + r.M12 * l.M01 + r.M22 * l.M02 + r.M32 * l.M03,
		r.M02 * l.M10 + r.M12 * l.M11 + r.M22 * l.M12 + r.M32 * l.M13,
		r.M02 * l.M20 + r.M12 * l.M21 + r.M22 * l.M22 + r.M32 * l.M23,
		r.M02 * l.M30 + r.M12 * l.M31 + r.M22 * l.M32 + r.M32 * l.M33,
		r.M03 * l.M00 + r.M13 * l.M01 + r.M23 * l.M02 + r.M33 * l.M03,
		r.M03 * l.M10 + r.M13 * l.M11 + r.M23 * l.M12 + r.M33 * l.M13,
		r.M03 * l.M20 + r.M13 * l.M21 + r.M23 * l.M22 + r.M33 * l.M23,
		r.M03 * l.M30 + r.M13 * l.M31 + r.M23 * l.M32 + r.M33 * l.M33,
	};
}
inline Vector4 Matrix4x4MultiplyVector4(Matrix4x4 l, Vector4 r)
{
	return (Vector4)
	{
		r.X * l.M00 + r.Y * l.M01 + r.Z * l.M02 + r.W * l.M03,
		r.X * l.M10 + r.Y * l.M11 + r.Z * l.M12 + r.W * l.M13,
		r.X * l.M20 + r.Y * l.M21 + r.Z * l.M22 + r.W * l.M23,
		r.X * l.M30 + r.Y * l.M31 + r.Z * l.M32 + r.W * l.M33,
	};
}
inline Vector3 Matrix4x4MultiplyVector3(Matrix4x4 l, Vector3 r)
{
	return (Vector3)
	{
		r.X * l.M00 + r.Y * l.M01 + r.Z * l.M02 + l.M03,
		r.X * l.M10 + r.Y * l.M11 + r.Z * l.M12 + l.M13,
		r.X * l.M20 + r.Y * l.M21 + r.Z * l.M22 + l.M23,
	};
}

Color ColorBlack = { 0x00, 0x00, 0x00, 0xff };
Color ColorWhite = { 0xff, 0xff, 0xff, 0xff };
Color ColorRed = { 0xff, 0x00, 0x00, 0xff };
Color ColorGreen = { 0x00, 0xff, 0x00, 0xff };
Color ColorBlue = { 0x00, 0x00, 0xff, 0xff };
inline Color ColorFromHex(uint32_t hex)
{
	return (Color)
	{
		(hex & 0xff000000) >> 24,
		(hex & 0x00ff0000) >> 16,
		(hex & 0x0000ff00) >> 8,
		(hex & 0x000000ff) >> 0
	};
}
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

/*
static inline bool TriangleIntersect(triangle_t t0, triangle_t t1)
{
	return false;
}
static inline triangle_t TriangleTransform(triangle_t t, matrix4x4 m)
{
	return (triangle_t)
	{
		.V0 = Matrix4x4.MultiplyVector3(m, t.V0),
		.V1 = Matrix4x4.MultiplyVector3(m, t.V1),
		.V2 = Matrix4x4.MultiplyVector3(m, t.V2),
	};
}
static inline triangle_t TriangleNormalize(triangle_t t)
{
	return (triangle_t)
	{
		.V0 = Vector3Normalize(t.V0),
		.V1 = Vector3Normalize(t.V1),
		.V2 = Vector3Normalize(t.V2),
	};
}
static inline vector3 TriangleNormal(triangle_t t)
{
	return Vector3Normalize(Vector3Cross(Vector3Subtract(t.V1, t.V0), Vector3Subtract(t.V2, t.V0)));
}
static inline scalar TriangleAngle(triangle_t t, vector3 up)
{
	return acos(Vector3Dot(Triangle.Normal(t), Vector3Normalize(up)));
}
static inline scalar TriangleSlope(triangle_t t, vector3 up)
{
	return tan(Triangle.Angle(t, up));
}
static inline vector3 TriangleCentroid(triangle_t t)
{
	return Vector3DivideScalar(Vector3Add(Vector3Add(t.V0, t.V1), t.V2), 3.0);
}

static inline void TriangleSubdivide(triangle_t t, triangle_t * triangles)
{
	triangles[0] = (triangle_t){ t.V0, Vector3MultiplyScalar(Vector3Add(t.V0, t.V1), 0.5), Vector3MultiplyScalar(Vector3Add(t.V0, t.V2), 0.5) };
	triangles[1] = (triangle_t){ Vector3MultiplyScalar(Vector3Add(t.V0, t.V2), 0.5), Vector3MultiplyScalar(Vector3Add(t.V1, t.V2), 0.5), t.V2 };
	triangles[2] = (triangle_t){ Vector3MultiplyScalar(Vector3Add(t.V0, t.V1), 0.5), t.V1, Vector3MultiplyScalar(Vector3Add(t.V1, t.V2), 0.5) };
	triangles[3] = (triangle_t){ Vector3MultiplyScalar(Vector3Add(t.V0, t.V2), 0.5), Vector3MultiplyScalar(Vector3Add(t.V0, t.V1), 0.5), Vector3MultiplyScalar(Vector3Add(t.V1, t.V2), 0.5) };
}
 */
