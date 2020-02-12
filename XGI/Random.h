#ifndef Random_h
#define Random_h

#include "LinearMath.h"

/// Sets the seed used for determining random numbers
/// \param seed The seed to set
void RandomSetSeed(int seed);

/// Generates a random signed 32 bit integer
/// \return A random integer
int RandomInteger(void);

/// Generates a random integer within a range
/// \param min The smallest value
/// \param max The largest value
/// \return The random value
int RandomIntegerRange(int min, int max);

/// Generates a random floating point value between [0,1]
/// \return The random value
Scalar RandomScalar(void);

/// Generates a random floating point value between [min, max]
/// \param min The minimum value
/// \param max The maximum value
/// \return The random value
Scalar RandomScalarRange(Scalar min, Scalar max);

/// Generates a 1D simplex value from a given x coordinate
/// (This is currently not tested to see if it actually works).
/// \param x The x coordinate
/// \return The simplex value at that point
Scalar RandomSimplex(Scalar x);

/// Generates a 2D simplex value from a given (x,y) coordinate
/// (This is currently not tested to see if it actually works).
/// \param xy The (x,y) coordinate
/// \return The simplex value at that point
Scalar RandomSimplex2(Vector2 xy);

/// Generates a 3D simplex value from a given (x,y,z) coordinate
/// \param xyz The (x,y,z) coordinate
/// \return The simplex value at that point
Scalar RandomSimplex3(Vector3 xyz);

#endif
