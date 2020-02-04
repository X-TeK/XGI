#ifndef Random_h
#define Random_h

#include "LinearMath.h"

void RandomSetSeed(int seed);
int RandomInteger(void);
int RandomIntegerRange(int min, int max);
Scalar RandomScalar(void);
Scalar RandomScalarRange(Scalar min, Scalar max);
Scalar RandomSimplex(Scalar x);
Scalar RandomSimplex2(Vector2 xy);
Scalar RandomSimplex3(Vector3 xyz);

#endif
