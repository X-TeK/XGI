#ifndef Random_h
#define Random_h

#include "LinearMath.h"

struct RandomInterface
{
	void (*Seed)(int);
	int (*Int)(void);
	Scalar (*Scalar)(void);
	Scalar (*ScalarRange)(Scalar a, Scalar b);
	int (*IntRange)(int a, int b);
	Scalar (*Simplex3)(Vector3 xyz);
} extern Random;

#endif
