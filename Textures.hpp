// contains the texture struct
#pragma once
#include "stb_image.h"
#include "geometry.hpp"
#include <fstream>

struct Color {
    union {
        struct { uint8_t r, g, b, a; };
        uint32_t color;
    };
    // constructors  
    Color(uint8_t x, uint8_t y, uint8_t z, uint8_t w) : r(x), g(y), b(z), a(w) {}
    Color(uint32_t c = 0xFFFFFFFF) : color(c) {}
    // lets you add colors
    Color operator+(Color const& c2) {
        Color c;
        c.r = r + c2.r;
        c.g = g + c2.g;
        c.b = b + c2.b;
        c.a = a + c2.a;
        return c;
    }
}; // define the color vars, each channel is an 8 bit int and the color is stored as a 32 bit int  


// stores a texture as a vector of colors with a width and height param enter a color value to make the texture a plain color
struct texture2d {
    unsigned int w, h;
    std::vector<Color> img; // the texture data
    void loadTexturePNG(const char* filename); // loads the data from the file
    void loadTextureBMP(const char* filename); // loads the data from the file
    
    // default constructor
    texture2d(unsigned int wi = 1, unsigned int he = 1) : w(wi), h(he){}
    texture2d(Color c) : w(1), h(1) {
        img.resize(1);
        img[0] = c;
    }
    // overload subscript
    Color operator[](unsigned int n) { 
        return img[n]; 
    }
    const Color operator[](unsigned int n) const {
        return img[n];
    }
    // returns the pixel at the desired coordinates
    Color getPix(unsigned int u, unsigned int v) {
        return img[v * w + u];
    }
    const Color getPix(unsigned int u, unsigned int v) const {
        return img[v * w + u];
    }
    // nearest neighbor
    const Color nearestN(float u, float v) const {
        unsigned int x = std::min(static_cast<unsigned int>(u * w + 0.5f), w - 1);
        unsigned int y = std::min(static_cast<unsigned int>(v * h + 0.5f), h - 1); // round coords
        return img[y * w + x];
    }
    // bilinear filter
    const Color bilinear(float u, float v) const{
        // get the pixel coords
        float x = u * (w - 1), y = v * (h - 1);

        unsigned int x0 = static_cast<unsigned int>(floor(x));
        unsigned int x1 = std::min(static_cast<unsigned int>(x0 + 1), w - 1);
        unsigned int y0 = static_cast<unsigned int>(floor(y));
        unsigned int y1 = std::min(static_cast<unsigned int>(y0 + 1), h - 1);

        float fx = x - x0;
        float fy = y - y0;

        // Helper to get pixel
        auto get = [&](unsigned int xx, unsigned int yy) -> Color {
            return img[yy * w + xx];
            };

        Color c00 = get(x0, y0);
        Color c10 = get(x1, y0);
        Color c01 = get(x0, y1);
        Color c11 = get(x1, y1);

        // Interpolate each channel
        Color result;
        result.r = static_cast<uint8_t>(
            (1 - fx) * (1 - fy) * c00.r +
            fx * (1 - fy) * c10.r +
            (1 - fx) * fy * c01.r +
            fx * fy * c11.r
            );
        result.g = static_cast<uint8_t>(
            (1 - fx) * (1 - fy) * c00.g +
            fx * (1 - fy) * c10.g +
            (1 - fx) * fy * c01.g +
            fx * fy * c11.g
            );
        result.b = static_cast<uint8_t>(
            (1 - fx) * (1 - fy) * c00.b +
            fx * (1 - fy) * c10.b +
            (1 - fx) * fy * c01.b +
            fx * fy * c11.b
            );
        result.a = static_cast<uint8_t>(
            (1 - fx) * (1 - fy) * c00.a +
            fx * (1 - fy) * c10.a +
            (1 - fx) * fy * c01.a +
            fx * fy * c11.a
            );
        return result;
    }

};

texture2d White_texture(Color(0xFFFFFFFF));
texture2d Red_texture(Color(0xFF0000FF));
texture2d Green_texture(Color(0x00FF00FF));
texture2d Blue_texture(Color(0x0000FFFF));