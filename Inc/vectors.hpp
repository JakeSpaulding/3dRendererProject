#pragma once
#include "matrixTemplate.hpp"
#include "VectorFuncTemplate.hpp"
#include <ostream>
using namespace std;
struct vec2 {
    union {
        float data[2];
        struct { float u, v; };
        struct { float x, y; };
    };

    // Constructors
    vec2(float a1 = 0, float a2 = 0) : data{ a1, a2 } {}
    vec2(const float a[2]) : data{ a[0], a[1] } {}

    // Subscript access
    float& operator[](int n) { return data[n]; }
    const float operator[](int n) const { return data[n]; }

    // Addition
    vec2 operator+(const vec2& v2) const {
        vec2 tmp;
        vecAdd<vec2, 2>(tmp, *this, v2);
        return tmp;
    }
    // Subtraction
    vec2 operator-(const vec2& v2) const {
        vec2 tmp;
        vecSub<vec2, 2>(tmp, *this, v2);
        return tmp;
    }
    // Dot product
    float operator*(const vec2& v2) const {
        return vecDot<vec2, 2>(*this, v2);
    }
    // Scaling
    vec2 operator*(float n) const {
        vec2 tmp;
        vecScale<vec2, 2>(tmp, *this, n);
        return tmp;
    }
    // Cross product
    float cross(const vec2& v2) const {
        return data[0] * v2.data[1] - data[1] * v2.data[0];
    }
    // Norm
    float norm() const {
        return vecNorm<vec2, 2>(*this);
    }
    // Norm squared
    float normsq() const {
        return *this * *this;
    }
    // Normalize in place
    void normalize() {
        vecNormalize<vec2, 2>(*this);
    }
    // Returns normalized copy
    vec2 normalized() const {
        vec2 tmp;
        vecNormalized<vec2, 2>(tmp, *this);
        return tmp;
    }
    // Comparison
    bool operator==(const vec2& v2) const {
        return eq<vec2, 2>(*this, v2);
    }
};
inline vec2 operator*(float n, const vec2& v2) {
    return v2 * n;
}
// add io compatability
inline std::ostream& operator<<(std::ostream& os, const vec2& v) {
    os << "[" << v[0] << ", " << v[1] << "]";
    return os;
}

// ===================== vec3 =====================
struct vec3 {
    union {
        float data[3];
        struct { float x, y, z; };
        struct { float r, g, b; };
        struct { float yaw, pitch, roll; };
        struct { float ry, rx, rz; };
    };

    // Constructors
    vec3(float a1 = 0, float a2 = 0, float a3 = 0) : data{ a1, a2, a3 } {}
    vec3(const float a[3]) : data{ a[0], a[1], a[2] } {}

    // Subscript access
    float& operator[](int n) { return data[n]; }
    const float operator[](int n) const { return data[n]; }

    // Addition
    vec3 operator+(const vec3& v2) const {
        vec3 tmp;
        vecAdd<vec3, 3>(tmp, *this, v2);
        return tmp;
    }
    // Subtraction
    vec3 operator-(const vec3& v2) const {
        vec3 tmp;
        vecSub<vec3, 3>(tmp, *this, v2);
        return tmp;
    }
    // Dot product
    float operator*(const vec3& v2) const {
        return vecDot<vec3, 3>(*this, v2);
    }
    // Scaling
    vec3 operator*(float n) const {
        vec3 tmp;
        vecScale<vec3, 3>(tmp, *this, n);
        return tmp;
    }
    // Cross product
    vec3 cross(const vec3& v2) const {
        return vec3(
            data[1] * v2.data[2] - data[2] * v2.data[1],
            data[2] * v2.data[0] - data[0] * v2.data[2],
            data[0] * v2.data[1] - data[1] * v2.data[0]
        );
    }
    // Cross product treating as vec2
    float cross2(const vec3& v2) const {
        return data[0] * v2.data[1] - v2.data[0] * data[1];
    }
    float cross2(const vec2& v2) const {
        return data[0] * v2.data[1] - v2.data[0] * data[1];
    }
    // Norm
    float norm() const {
        return vecNorm<vec3, 3>(*this);
    }
    // Norm squared
    float normsq() const {
        return *this * *this;
    }
    // Normalize in place
    void normalize() {
        vecNormalize<vec3, 3>(*this);
    }
    // Returns normalized copy
    vec3 normalized() const {
        vec3 tmp;
        vecNormalized<vec3, 3>(tmp, *this);
        return tmp;
    }
    // Comparison
    bool operator==(const vec3& v2) const {
        return eq<vec3, 3>(*this, v2);
    }
    // Convert to vec2
    vec2 v3tv2() const {
        return vec2(x, y);
    }
};
inline vec3 operator*(float n, const vec3& v2) {
    return v2 * n;
}
// io compatability
inline std::ostream& operator<<(std::ostream& os, const vec3& v) {
    os << "[" << v[0] << ", " << v[1] << ", " << v[2] << "]";
    return os;
}

// ===================== vec4 =====================
struct vec4 {
    union {
        float data[4];
        struct { float x, y, z, w; };
        struct { float r, g, b, a; };
    };

    // Constructors
    vec4(float a1 = 0, float a2 = 0, float a3 = 0, float a4 = 1) : data{ a1, a2, a3, a4 } {}
    vec4(const float a[4]) : data{ a[0], a[1], a[2], a[3] } {}
    vec4(const vec3& v, float a = 1) : data{ v.x, v.y, v.z, a } {}

    // Subscript access
    float& operator[](int n) { return data[n]; }
    const float operator[](int n) const { return data[n]; }

    // Addition
    vec4 operator+(const vec4& v2) const {
        vec4 tmp;
        vecAdd<vec4, 4>(tmp, *this, v2);
        return tmp;
    }
    // Subtraction
    vec4 operator-(const vec4& v2) const {
        vec4 tmp;
        vecSub<vec4, 4>(tmp, *this, v2);
        return tmp;
    }
    // Dot product
    float operator*(const vec4& v2) const {
        return vecDot<vec4, 4>(*this, v2);
    }
    // Scaling
    vec4 operator*(float n) const {
        vec4 tmp;
        vecScale<vec4, 4>(tmp, *this, n);
        return tmp;
    }
    // Norm
    float norm() const {
        return vecNorm<vec4, 4>(*this);
    }
    // Norm squared
    float normsq() const {
        return *this * *this;
    }
    // Normalize in place
    void normalize() {
        vecNormalize<vec4, 4>(*this);
    }
    // Returns normalized copy
    vec4 normalized() const {
        vec4 tmp;
        vecNormalized<vec4, 4>(tmp, *this);
        return tmp;
    }
    // Comparison
    bool operator==(const vec4& v2) const {
        return eq<vec4, 4>(*this, v2);
    }
    // Homogeneous normalization
    void wNormalize() {
        if (data[3] != 0 && data[3] != 1) {
            for (int i = 0; i < 3; i++) {
                data[i] /= data[3];
            }
        }
    }
    vec4 wNormalized() const {
        vec4 tmp = *this;
        if (data[3] != 0 && data[3] != 1) {
            for (int i = 0; i < 3; i++) {
                tmp.data[i] /= data[3];
            }
        }
        return tmp;
    }
    // Dot product of non-homogeneous part
    float dot3(const vec4& v2) const {
        float tmp = 0;
        for (int i = 0; i < 3; i++) {
            tmp += data[i] * v2.data[i];
        }
        return tmp;
    }
    // Norm of non-homogeneous part
    float norm3() const {
        float dp = this->dot3(*this);
        dp = std::sqrt(dp);
        return dp;
    }
    void normalize3() {
        float dp = this->dot3(*this);
        dp = std::sqrt(dp);
        for (int i = 0; i < 3; i++) {
            data[i] /= dp;
        }
    }
    vec4 cross3(const vec4& v2) const {
        return vec4(
            data[1] * v2.data[2] - data[2] * v2.data[1],
            data[2] * v2.data[0] - data[0] * v2.data[2],
            data[0] * v2.data[1] - data[1] * v2.data[0],
            0
        );
    }
    vec3 v4tv3() const {
        vec4 tmp = this->wNormalized();
        return vec3(tmp[0], tmp[1], tmp[2]);
    }
};
// scalar in oposite dirrection
inline vec4 operator*(float n, const vec4& v2) {
    return v2 * n;
}
// ad io compatatbility
inline std::ostream& operator<<(std::ostream& os, const vec4& v) {
    os << "[" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << "]";
    return os;
}
// Face normal for triangle
inline vec3 faceNormal(const vec3& a, const vec3& b, const vec3& c) {
    vec3 A = b - a;
    vec3 B = c - a;
    return A.cross(B);
}

