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

class IVec2
{
public:
    int x = 0;
    int y = 0;

    IVec2() = default;
    IVec2(int xin, int yin);

    bool operator == (const IVec2& rhs) const;
    bool operator != (const IVec2& rhs) const;

    IVec2 operator + (const IVec2& rhs) const;
    IVec2 operator - (const IVec2& rhs) const;
    IVec2 operator / (const int& val) const;
    IVec2 operator * (const int& val) const;

    void operator += (const IVec2& rhs);
    void operator -= (const IVec2& rhs);
    void operator *= (const int& val);
    void operator /= (const int& val);
};


namespace std {
    template <>
    struct hash<IVec2> {
        size_t operator()(const IVec2& v) const noexcept {
            size_t seed = 0;
            auto hash_combine = [](size_t seed, int value) {
                return seed ^ (std::hash<int>()(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
                };
            seed = hash_combine(seed, v.x);
            seed = hash_combine(seed, v.y);
            return seed;
        }
    };
}

