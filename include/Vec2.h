#pragma once
#include <functional>
#include <cstdint>

class Vec2
{
public:

    float x = 0;
    float y = 0;

    Vec2();
    Vec2(float xin, float yin);

    bool operator == (const Vec2& rhs) const;
    bool operator != (const Vec2& rhs) const;

    Vec2 operator + (const Vec2& rhs) const;
    Vec2 operator - (const Vec2& rhs) const;
    Vec2 operator / (const float& val) const;
    Vec2 operator * (const float& val) const;

    void operator += (const Vec2& rhs);
    void operator -= (const Vec2& rhs);
    void operator *= (const float& val);
    void operator /= (const float& val);

    float distSquared(const Vec2& rhs) const;
    float dist(const Vec2& rhs) const;
    float length() const;
    Vec2 normalize() const;
};


template<>
struct std::hash<Vec2> {
    size_t operator()(const Vec2& v) const noexcept {
        // Safe method to hash floats
        auto hash_combine = [](size_t seed, float value) {
            std::hash<float> hasher;
            return seed ^ (hasher(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
            };

        size_t seed = 0;
        seed = hash_combine(seed, v.x);
        seed = hash_combine(seed, v.y);
        return seed;
    }
};

