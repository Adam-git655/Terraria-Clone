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

	//Sample perlin noise at x and y
	float perlin2D(float x, float y);

private:
	unsigned seed;

	//calculating dot product btw. x grid coords and x coord(1D, hence requires only 2 points)
	float dotGridGradient(int ix, float x);

	//calculating dot product btw. x,y grid coords and x,y coords
	float dotGridGradient2D(int ix, int iy, float x, float y);

	//Making a random scalar gradient (left or right)
	//Hashing function, pseudo random, nicely distributed, deterministic
	float randomGradient(int ix) const;
	Vec2 randomGradient2D(int ix, int iy) const;


	float interpolate(float a0, float a1, float w);
};