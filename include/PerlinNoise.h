#pragma once
#include <iostream>
#include <cmath>
#include <math.h>
#include "Vec2.h"

class Perlin
{
public:

	Perlin(unsigned seedVal = 0);

	//Sample perlin noise at x
	float perlin(float x);

private:
	unsigned seed;

	//calculating dot product btw. x grid coords and x coord(1D, hence requires only 2 points)
	float dotGridGradient(int ix, float x);

	//Making a random scalar gradient (left or right)
	//Hashing function, pseudo random, nicely distributed, deterministic
	float randomGradient(int ix) const;

	float interpolate(float a0, float a1, float w);
};