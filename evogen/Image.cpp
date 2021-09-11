#include <evogen/Image.h>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION

// There are some warnings in stb_image which prevent compilation
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wsign-compare"
#include "stb_image.h"
#pragma GCC diagnostic pop

namespace evo
{

bool Image::load_from_file(std::string const& name)
{
    // open image file
    FILE* file = fopen(name.c_str(), "r");
    if(!file)
    {
        std::cout << "Image: couldn't open file '" << name << "'" << std::endl;
        return false;
    }
    m_data = stbi_load_from_file(file, &m_size.x, &m_size.y, &m_channels, 4);
    fclose(file);
    if(!m_data)
    {
        std::cout << "Image: couldn't load image from file" << std::endl;
        return false;
    }

    std::cout << "Loaded image '" << name << "': " << to_string() << std::endl;
    return true;
}

Image::~Image()
{
    if(m_data)
        stbi_image_free(m_data);
}

}
