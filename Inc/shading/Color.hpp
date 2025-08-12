#pragma once
#include <cstdint>
// includes the struct for color 


// stores color as a 32 bit int with 8 bits per channel (rgba)
struct Color {
    union {
        struct { uint8_t r, g, b, a; };
        uint32_t color;
    };
    // constructors  
    Color(uint8_t x, uint8_t y, uint8_t z, uint8_t w) : r(x), g(y), b(z), a(w) {}
    Color(uint32_t c = 0xFFFFFFFF) : color(c) {}
    // lets you add colors
    Color operator+(Color const& c2) const{
        Color c;
        c.r = r + c2.r;
        c.g = g + c2.g;
        c.b = b + c2.b;
        c.a = a + c2.a;
        return c;
    }
    Color operator+=(Color const& c2) {
        r = r + c2.r;
        g = g + c2.g;
        b = b + c2.b;
        a = a + c2.a;
        return *this;
    }
}; // define the color vars, each channel is an 8 bit int and the color is stored as a 32 bit int  