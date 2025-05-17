#include "PerlinNoise.h"


Perlin::Perlin(unsigned seedVal)
    :seed(seedVal)
{

}

float Perlin::randomGradient(int ix) const
{
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2; 
    unsigned a = ix + seed;
    a *= 3284157443;
 
    a ^= a << s | a >> w - s;
    a *= 2048419325;
    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]
    
    return sin(random);
}

float Perlin::dotGridGradient(int ix, float x)
{
    //Get random gradient from grid x
	float gradient = randomGradient(ix);

    //Calc dist btw x and grid x
    float dx = x - (float)ix;

    //return dot product
    return (gradient * dx);
}

float Perlin::interpolate(float a0, float a1, float w)
{
    //Cubic interpolation
    return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}

float Perlin::perlin(float x)
{
	//Determine left and right nearby x grid coords
	int x0 = static_cast<int>(std::floor(x));
	int x1 = x0 + 1;

	//Determine interpolation weight
	float sx = x - (float)x0;

    //Compute dot products  (1D noise hence only 2 points)
    float n0 = dotGridGradient(x0, x); //gradient at x0(left)
    float n1 = dotGridGradient(x1, x); //gradient at x1(right)

    //Interpolate n0,n1
    float value = interpolate(n0, n1, sx);
    
    return value;
}




