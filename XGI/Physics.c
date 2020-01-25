#include <stdlib.h>
#include "Physics.h"

static PhysicsBody_T CreateBody(transform * transform, PhysicsShape_T shape, scalar mass)
{
	PhysicsBody_T body = malloc(sizeof(struct PhysicsBody));
	*body = (struct PhysicsBody)
	{
		.Shape = shape,
		.Transform = transform,
		.Velocity = Vector3Zero,
		.Acceleration = Vector3Zero,
		.Mass = mass,
	};
	return body;
}

static PhysicsShape_T CreateShape(vertex_t * vertices, int vertexCount)
{
	PhysicsShape_T shape = malloc(sizeof(struct PhysicsShape));
	*shape = (struct PhysicsShape)
	{
		.TriangleCount = vertexCount / 3,
		.Triangles = malloc(vertexCount / 3 * sizeof(triangle_t)),
	};
	for (int i = 0; i < vertexCount; i += 3)
	{
		shape->Triangles[i / 3] = (triangle_t){ vertices[i + 0].Position, vertices[i + 1].Position, vertices[i + 2].Position };
	}
	return shape;
}

static PhysicsWorld_T CreateWorld()
{
	PhysicsWorld_T world = malloc(sizeof(struct PhysicsWorld));
	*world = (struct PhysicsWorld)
	{
		.Bodies = ListCreate(),
	};
	return world;
}

static void DestroyBody(PhysicsBody_T body)
{
	free(body);
}

static void DestroyShape(PhysicsShape_T shape)
{
	free(shape->Triangles);
	free(shape);
}

static void DestroyWorld(PhysicsWorld_T world)
{
	ListDestroy(world->Bodies);
	free(world);
}

struct PhysicsInterface Physics =
{
	.CreateBody = CreateBody,
	.CreateShape = CreateShape,
	.CreateWorld = CreateWorld,
	.DestroyBody = DestroyBody,
	.DestroyShape = DestroyShape,
	.DestroyWorld = DestroyWorld,
};
