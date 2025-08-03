// contains the texture struct

#pragma once
#include "stb_image.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <cmath>

struct Color {
    union {
        struct { uint8_t a,b,g,r; };
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

// wraps uv coordinates to be normalized
inline float wrap(float n) {
    n = fmod(n, 1.0f);
    if (n < 0) return n + 1.0f;
    return n;
}

// stores a texture as a vector of colors with a width and height param enter a color value to make the texture a plain color
struct texture2d {
    bool opaque; // if it has any transparancy this is false
    unsigned int w, h;  // Changed to unsigned int
    std::vector<Color> img; // the texture data
    void loadTexturePNG(const char* filename); // loads the data from the file
    void loadTextureBMP(const char* filename); // loads the data from the file
    
    texture2d() : w(1), h(1) {}
    // default constructor
    texture2d(unsigned int wi, unsigned int he) : w(wi), h(he) {  // Changed to unsigned int
        img.resize(static_cast<size_t>(w) * h);  // Use size_t for vector resize
    }
    texture2d(Color c) : w(1), h(1) {
        img.resize(1);
        img[0] = c;
    }
    // overload subscript
    Color operator[](size_t n) {  // Changed to size_t for array indexing
        return img[n]; 
    }
    const Color operator[](size_t n) const {  // Changed to size_t for array indexing
        return img[n];
    }
    // returns the pixel at the desired coordinates
    Color getPix(unsigned int u, unsigned int v) {  // Changed to unsigned int
        return img[static_cast<size_t>(v) * w + u];  // Use size_t for indexing calculation
    }
    const Color getPix(unsigned int u, unsigned int v) const {  // Changed to unsigned int
        return img[static_cast<size_t>(v) * w + u];  // Use size_t for indexing calculation
    }
    // nearest neighbor
    const Color nearestN(float u, float v) const {
        // wrap the coordinates
        u = wrap(u);
        v = wrap(v);
        unsigned int x = std::min(static_cast<unsigned int>(u * w + 0.5f), w - 1);  // Changed to unsigned int
        unsigned int y = std::min(static_cast<unsigned int>(v * h + 0.5f), h - 1);  // Changed to unsigned int
        return img[static_cast<size_t>(y) * w + x];  // Use size_t for indexing calculation
    }
    // bilinear filter
    const Color bilinear(float u, float v) const {
        // wrap coordinates
        u = wrap(u), v = wrap(v);
        // get the pixel coords
        float x = u * (w - 1), y = v * (h - 1);

        unsigned int x0 = static_cast<unsigned int>(floor(x));  // Changed to unsigned int
        unsigned int x1 = std::min(x0 + 1, w - 1);  // Changed to unsigned int
        unsigned int y0 = static_cast<unsigned int>(floor(y));  // Changed to unsigned int
        unsigned int y1 = std::min(y0 + 1, h - 1);  // Changed to unsigned int

        float fx = x - x0;
        float fy = y - y0;

        // Get corner pixels
        Color c00 = getPix(x0, y0);
        Color c10 = getPix(x1, y0);
        Color c01 = getPix(x0, y1);
        Color c11 = getPix(x1, y1);

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
