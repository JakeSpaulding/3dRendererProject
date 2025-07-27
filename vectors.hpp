#ifndef VECTORS_HPP
#define VECTORS_HPP

// contains structs for different sized vectors

#include "matrixTemplate.hpp"

struct vec2 : Vbase<vec2, 2> {
    float& x;
    float& y;
    float& u;
    float& v;

    // Constructor: bind references, then assign values
    vec2(float a1 = 0, float a2 = 0)
        : x(data[0]), y(data[1]), u(data[0]), v(data[1])
    {
        data[0] = a1;
        data[1] = a2;
    }

    // Conversion constructor
    vec2(const Vbase<vec2, 2>& base)
        : x(data[0]), y(data[1]), u(data[0]), v(data[1])
    {
        std::copy(&base.data[0], &base.data[0] + 2, data);
    }

    // Copy assignment operator
    vec2& operator=(const vec2& other) {
        if (this != &other) {
            for (int i = 0; i < 2; ++i) data[i] = other.data[i];
        }
        return *this;
    }
    // cross product
    float cross(vec2 const& v2) const {
        return (data[0] * v2.data[1] - data[1] * v2.data[0]);
    }
    float cross(Vbase<vec2, 2> const& v2) const {
        return (data[0] * v2.data[1] - data[1] * v2.data[0]);
    }
};

struct vec3 : Vbase<vec3, 3> {
    // allows access through v.x instead of v[0] or v.data[0]
    float& x;
    float& y;
    float& z;

    // Constructor: bind references, then assign values
    vec3(float a1 = 0, float a2 = 0, float a3 = 0)
        : x(data[0]), y(data[1]), z(data[2])
    {
        data[0] = a1;
        data[1] = a2;
        data[2] = a3;
    }

    // Conversion constructor
    vec3(const Vbase<vec3, 3>& base)
        : x(data[0]), y(data[1]), z(data[2])
    {
        std::copy(&base.data[0], &base.data[0] + 3, data);
    }

    // define cross product
    vec3 cross(vec3 const& v2) const {
        float a = data[1] * v2.data[2] - data[2] * v2.data[1];
        float b = -1 * (data[0] * v2.data[2] - data[2] * v2.data[0]);
        float c = data[0] * v2.data[1] - data[1] * v2.data[0];
        return (vec3(a, b, c));
    }
    // lets you do a cross product treating it like a vec2
    
    float cross2(vec3 const& v2) const {
        return (x*v2.y - v2.x * y);
    }

    float cross2(vec2 const& v2) const {
        return (x * v2.y - v2.x * y);
    }

    // Copy assignment operator
    vec3& operator=(const vec3& other) {
        if (this != &other) {
            for (int i = 0; i < 3; ++i) data[i] = other.data[i];
        }
        return *this;
    }
    // outputs a vec2 of this
    vec2 v3tv2(void) {
        return(vec2(x, y));
    }
};

// gets the normal of the face made by a triangle abc
inline vec3 faceNormal(vec3 const& a, vec3 const& b, vec3 const& c) {
    vec3 A(b-a);
    vec3 B(c - a);
    return A.cross(B);
}

struct vec4 : Vbase<vec4, 4> {
    float& x;
    float& y;
    float& z;
    float& w;

    // Constructor: bind references, then assign values
    vec4(float a1 = 0, float a2 = 0, float a3 = 0, float a4 = 1)
        : x(data[0]), y(data[1]), z(data[2]), w(data[3])
    {
        data[0] = a1;
        data[1] = a2;
        data[2] = a3;
        data[3] = a4;
    }

    vec4(vec3 const& v, float a = 1)
        : x(data[0]), y(data[1]), z(data[2]), w(data[3])
    {
        std::copy(&v.data[0], &v.data[0] + 3, &data[0]);
        data[3] = a;
    }

    // Conversion constructor
    vec4(const Vbase<vec4, 4>& base)
        : x(data[0]), y(data[1]), z(data[2]), w(data[3])
    {
        std::copy(&base.data[0], &base.data[0] + 4, data);
    }

    // normalizes the w (homogenous) dimension
    void wNormalize() {
        if (data[3] != 0 && data[3] != 1) {
            for (int i = 0; i < 3; i++) {
                data[i] /= data[3];
            }
        }
    }
    vec4 wNormalized() {
        vec4 tmp = *this;
        if (data[3] != 0 && data[3] != 1) {
            for (int i = 0; i < 3; i++) {
                tmp.data[i] /= data[3];
            }
        }
        return tmp;
    }
    // takes the dot product of the non homogenous part of the vector
    float dot3(vec4 const& v2) const {
        float tmp = 0;
        for (int i = 0; i < 3; i++) {
            tmp += data[i] * v2.data[i];
        }
        return tmp;
    }

    // returns the norm of the non homogenous part of the vector
    float norm3() const {
        float dp = this->dot3(*this);
        dp = sqrt(dp);
        return dp;
    }
    void normalize3() {
        float dp = this->dot3(*this);
        dp = sqrt(dp);
        for (int i = 0; i < 3; i++) {
            data[i] /= dp;
        }
    }
    vec4 cross3(vec4 const& v2) const {
        float a = data[1] * v2.data[2] - data[2] * v2.data[1];
        float b = -1 * (data[0] * v2.data[2] - data[2] * v2.data[0]);
        float c = data[0] * v2.data[1] - data[1] * v2.data[0];
        return (vec4(a, b, c, 0));
    }
    vec3 v4tv3() {
        vec4 tmp = this->wNormalized();
        return vec3(tmp[0], tmp[1], tmp[2]);
    }
    vec4& operator=(const vec4& other) {
        if (this != &other) {
            for (int i = 0; i < 4; ++i) data[i] = other.data[i];
        }
        return *this;
    }
};


#endif // VECTORS_HPP

