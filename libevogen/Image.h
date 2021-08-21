#pragma once

#include <libevogen/Vector.h>

#include <cassert>
#include <string>

namespace evo
{

struct Color
{
    uint8_t r = 0, g = 0, b = 0, a = 0;
};

class Image
{
public:
    ~Image();

    bool load_from_file(std::string const& name);

    Color pixel(Size<int> const& coords) const { return load_pixel(pixel_ptr(coords)); }
    void set_pixel(Size<int> const& coords, Color const& color) { store_pixel(pixel_ptr(coords), color); }

    Size<int> size() const { return m_size; }
    int channels() const { return m_channels; }

    std::string to_string() const { return m_size.to_string() + " @ " + std::to_string(m_channels) + " channels"; }

private:
    uint8_t* pixel_ptr(Size<int> const& coords)
    {
        assert(coords.x < m_size.x && coords.y < m_size.y);
        return &m_data[(coords.x + coords.y * m_size.x) * m_channels];
    }

    uint8_t const* pixel_ptr(Size<int> const& coords) const
    {
        assert(coords.x < m_size.x && coords.y < m_size.y);
        return &m_data[(coords.x + coords.y * m_size.x) * m_channels];
    }

    Color load_pixel(uint8_t const* ptr) const
    {
        Color color;
        color.r = ptr[0];
        if(m_channels > 1) color.g = ptr[1];
        if(m_channels > 2) color.b = ptr[2];
        if(m_channels > 3) color.a = ptr[3];
        return color;
    }

    void store_pixel(uint8_t* ptr, Color const& color)
    {
        ptr[0] = color.r;
        if(m_channels > 1) ptr[1] = color.g;
        if(m_channels > 2) ptr[2] = color.g;
        if(m_channels > 3) ptr[3] = color.g;
    }

    uint8_t* m_data = nullptr;
    Size<int> m_size;
    int m_channels;
};

}
