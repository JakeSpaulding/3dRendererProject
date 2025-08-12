// contains the texture struct

#pragma once
#include "Color.hpp"
#include "stb_image.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <cmath>
#include <string>


// wraps uv coordinates to be normalized
inline float wrap(float n) {
    n = fmod(n, 1.0f);
    if (n < 0) return n + 1.0f;
    return n;
}

// stores a texture as a vector of colors with a width and height param enter a color value to make the texture a plain color
struct texture2d {
    bool opaque; // if it has any transparancy this is false
    bool hasTexture; // indicates if there is a texture ascociated with this struct
    unsigned int w, h;  // Changed to unsigned int
    std::vector<Color> img; // the texture data

    void loadTexturePNG(const char* filename); // loads the data from the file
    void loadTextureBMP(const char* filename); // loads the data from the file
    
    texture2d() : w(1), h(1), opaque(1), hasTexture(0) {}
 
    texture2d(Color c) : w(1), h(1), opaque(c.a == 255), hasTexture(1) {
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
    const Color bilinear(float u, float v) const;
};
