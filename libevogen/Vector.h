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

    std::string to_string() const
    {
        return std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z);
    }

    T x, y, z;
};

}
