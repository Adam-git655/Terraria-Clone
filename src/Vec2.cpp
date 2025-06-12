#include "Vec2.h"
#include <math.h>

Vec2::Vec2()
{

}

Vec2::Vec2(float xin, float yin)
	:x(xin), y(yin)
{

}

bool Vec2::operator == (const Vec2& rhs) const
{
	return (x == rhs.x && y == rhs.y);
}

bool Vec2::operator != (const Vec2& rhs) const
{
	return (x != rhs.x || y != rhs.y);
}

Vec2 Vec2::operator + (const Vec2& rhs) const
{
	return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2& rhs) const
{
	return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator * (const float& val) const
{
	return Vec2(x * val, y * val);
}

Vec2 Vec2::operator / (const float& val) const
{
	return Vec2(x / val, y / val);
}

void Vec2::operator += (const Vec2& rhs)
{
	x += rhs.x;
	y += rhs.y;
}

void Vec2::operator -= (const Vec2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
}

void Vec2::operator *= (const float& val)
{
	x *= val;
	y *= val;
}

void Vec2::operator /= (const float& val)
{
	x /= val;
	y /= val;
}

float Vec2::distSquared(const Vec2& rhs) const
{
	Vec2 distvec = Vec2(rhs.x - x, rhs.y - y);
	return ((distvec.x * distvec.x) + (distvec.y * distvec.y));
}

float Vec2::dist(const Vec2& rhs) const
{
	return (sqrtf(distSquared(rhs)));
}

float Vec2::length() const
{
	return (sqrtf((x * x) + (y * y)));
}

Vec2 Vec2::normalize() const
{

	float len = length();
	return Vec2(x / len, y / len);
}

