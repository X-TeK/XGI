#ifndef Physics_h
#define Physics_h

#include "LinearMath.h"
#include "VertexBuffer.h"

typedef struct PhysicsShape
{
	int TriangleCount;
	triangle_t * Triangles;
} * PhysicsShape_T;

typedef struct PhysicsBody
{
	PhysicsShape_T Shape;
	transform * Transform;
	vector3 Velocity;
	vector3 Acceleration;
	scalar Mass;
} * PhysicsBody_T;

typedef struct PhysicsWorld
{
	List Bodies;
} * PhysicsWorld_T;

struct PhysicsInterface
{
	PhysicsBody_T (*CreateBody)(transform * transform, PhysicsShape_T shape, scalar mass);
	PhysicsShape_T (*CreateShape)(vertex_t * vertices, int vertexCount);
	PhysicsWorld_T (*CreateWorld)(void);
	void (*Update)(PhysicsWorld_T world);
	void (*ApplyForce)(PhysicsBody_T body, vector3 force);
	void (*DestroyBody)(PhysicsBody_T body);
	void (*DestroyShape)(PhysicsShape_T shape);
	void (*DestroyWorld)(PhysicsWorld_T world);
} extern Physics;

#endif
