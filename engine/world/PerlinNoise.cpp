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

Vec2 Perlin::randomGradient2D(int ix, int iy) const
{
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2;
    unsigned a = ix, b = iy;
    a *= 3284157443;

    b ^= a << s | a >> w - s;
    b *= 1911520717;

    a ^= b << s | b >> w - s;
    a *= 2048419325;
    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]

    // Create the vector from the angle
    Vec2 v;
    v.x = sin(random);
    v.y = cos(random);

    return v;
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

float Perlin::dotGridGradient2D(int ix, int iy, float x, float y)
{
    //Get random gradient from grid x, y
    Vec2 gradient = randomGradient2D(ix, iy);

    //Calculate dist vector
    float dx = x - (float)ix;
    float dy = y - (float)iy;

    //return dot product
    return (dx * gradient.x + dy * gradient.y);
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

float Perlin::perlin2D(float x, float y)
{
    //Determine grid cell corner coords
    int x0 = static_cast<int>(std::floor(x));
    int y0 = static_cast<int>(std::floor(y));
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    //Compute Interpolation weights
    float sx = x - (float)x0;
    float sy = y - (float)y0;

    //Compute Dot products of and interpolate top two corners
    float n0 = dotGridGradient2D(x0, y0, x, y);
    float n1 = dotGridGradient2D(x1, y0, x, y);
    float ix0 = interpolate(n0, n1, sx);

    //Compute Dot products of and interpolate bottom two corners
    n0 = dotGridGradient2D(x0, y1, x, y);
    n1 = dotGridGradient2D(x1, y1, x, y);
    float ix1 = interpolate(n0, n1, sx);

    //Interpolate between previous two interpolations (now in y)
    float value = interpolate(ix0, ix1, sy);

    return value;
}



