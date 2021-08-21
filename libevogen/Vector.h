#pragma once

#include <string>

namespace evo
{

template<class T>
class Vector
{
public:
    Vector()
    : Vector(0, 0, 0) {}

    Vector(T _x, T _y, T _z)
    : x(_x), y(_y), z(_z) {}

    Vector operator+(Vector const& other) const
    {
        return {x+other.x, y+other.y, z+other.z};
    }

    Vector operator-(Vector const& other) const
    {
        return {x-other.x, y-other.y, z-other.z};
    }

    Vector operator*(double value) const
    {
        return {static_cast<T>(x*value), static_cast<T>(y*value), static_cast<T>(z*value)};
    }

    Vector operator/(double value) const
    {
        return {static_cast<T>(x/value), static_cast<T>(y/value), static_cast<T>(z/value)};
    }

    bool operator==(Vector const& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

    std::string to_string() const
    {
        return std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z);
    }

    T x, y, z;
};

template<class T>
class Size
{
public:
    Size()
    : Size(0, 0) {}

    Size(T _x, T _y)
    : x(_x), y(_y) {}

    Size operator+(Size const& other) const
    {
        return {x+other.x, y+other.y};
    }

    Size operator-(Size const& other) const
    {
        return {x-other.x, y-other.y};
    }

    Size operator*(double value) const
    {
        return {static_cast<T>(x*value), static_cast<T>(y*value)};
    }

    Size operator/(double value) const
    {
        return {static_cast<T>(x/value), static_cast<T>(y/value)};
    }

    bool operator==(Size const& other) const
    {
        return x == other.x && y == other.y;
    }

    std::string to_string() const
    {
        return std::to_string(x) + "x" + std::to_string(y);
    }

    T x, y;
};

}

namespace std
{

template<class T>
struct hash<evo::Vector<T>>
{
    size_t operator()(evo::Vector<T> const& vector) const
    {
        return hash<T>()(vector.x) ^ (hash<T>()(vector.y) << 24) ^ (hash<T>()(vector.z) << 48);
    }
};

template<class T>
struct hash<evo::Size<T>>
{
    size_t operator()(evo::Size<T> const& size) const
    {
        return hash<T>()(size.x) ^ (hash<T>()(size.y) << 24);
    }
};

}
