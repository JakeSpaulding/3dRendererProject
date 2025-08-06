#pragma once
// this contains the structs for different sized matrices
#include "vectors.hpp"
#include <span>

// 2x2 matrix
struct mat2 : Mbase<2> {
    mat2(const Mbase<2>& base) {
        std::copy(&base.data[0], &base.data[0] + 4, &data[0]);
    }

    mat2(vec2 a, vec2 b) {
        std::copy(&a.data[0], &a.data[0] + 2, &data[0]);
        std::copy(&b.data[0], &b.data[0] + 2, &data[0] + 2);
    }

    mat2(float a1 = 0, float a2 = 0,
        float b1 = 0, float b2 = 0) {
        float nums[4] = { a1, a2, b1, b2 };
        std::copy(nums, nums + 4, data);
    }

    // Constructor from array of vec2
    mat2(const vec2 arr[2]) {
        std::copy(&arr[0].data[0], &arr[0].data[0] + 2, &data[0]);
        std::copy(&arr[1].data[0], &arr[1].data[0] + 2, &data[2]);
    }

    float det() const {
        return data[0] * data[3] - data[1] * data[2];  // Fixed: was data[1] * data[0]
    }

    mat2 inv() const {
        float d = this->det();
        if (d) {
            d = 1 / d;
            return mat2(
                data[3] * d, -data[1] * d,
                -data[2] * d, data[0] * d);
        }
        return mat2();
    }

    vec2 operator*(vec2 const& vec) const {
        vec2 tmp(0, 0);
        for (unsigned int i = 0; i < 2; i++) {  // Changed to unsigned int
            for (unsigned int n = 0; n < 2; n++) {  // Changed to unsigned int
                tmp[i] += data[i * 2 + n] * vec[n];
            }
        }
        return(tmp);
    }
    // Copy assignment operator
    mat2& operator=(const mat2& other) {
        if (this != &other) {
            for (unsigned int i = 0; i < 4; ++i) data[i] = other.data[i];  // Changed to unsigned int
        }
        return *this;
    }
    mat2 operator*(const mat2& rhs) const {
        return mat2(Mbase<2>::operator*(rhs));
    }
};

// 3x3 matrix
struct mat3 : Mbase<3> {
    mat3(const Mbase<3>& base) {
        std::copy(&base.data[0], &base.data[0] + 9, &data[0]);
    }
    // allow initialization using vec3s
    mat3(vec3 a, vec3 b, vec3 c) {
        std::copy(&a.data[0], &a.data[0] + 3, &data[0]);  // Fixed: was &a.data[3]
        std::copy(&b.data[0], &b.data[0] + 3, &data[3]);  // Fixed: was &b.data[3]
        std::copy(&c.data[0], &c.data[0] + 3, &data[6]);  // Fixed: was &c.data[3]
    }
    // allow the passing of individual floats
    mat3(float a1 = 0, float a2 = 0, float a3 = 0,
        float b1 = 0, float b2 = 0, float b3 = 0,
        float c1 = 0, float c2 = 0, float c3 = 0) {
        float nums[9] = {
            a1, a2, a3,
            b1, b2, b3,
            c1, c2, c3
        };
        std::copy(nums, nums + 9, data);
    }
    // Constructor from array of vec3
    mat3(const vec3 arr[3]) {
        std::copy(&arr[0].data[0], &arr[0].data[0] + 3, &data[0]);
        std::copy(&arr[1].data[0], &arr[1].data[0] + 3, &data[3]);
        std::copy(&arr[2].data[0], &arr[2].data[0] + 3, &data[6]);
    }

    vec3 operator*(vec3 const& vec) const {
        vec3 tmp(0, 0, 0);
        for (unsigned int i = 0; i < 3; i++) {  // Changed to unsigned int
            for (unsigned int n = 0; n < 3; n++) {  // Changed to unsigned int
                tmp[i] += data[i * 3 + n] * vec[n];
            }
        }
        return(tmp);
    }

    float det() const {
        float n = data[0] * (data[4] * data[8] - data[5] * data[7]) -
            data[1] * (data[3] * data[8] - data[5] * data[6]) +
            data[2] * (data[3] * data[7] - data[4] * data[6]);
        return n;
    }

    mat3 operator*(const mat3& rhs) const {
        return mat3(Mbase<3>::operator*(rhs));
    }

    // Copy assignment operator
    mat3& operator=(const mat3& other) {
        if (this != &other) {
            for (unsigned int i = 0; i < 9; ++i) data[i] = other.data[i];  // Changed to unsigned int
        }
        return *this;
    }
};

// 4x4 matrix
struct mat4 : Mbase<4> {
    mat4(const Mbase<4>& base) {
        std::copy(&base.data[0], &base.data[0] + 16, &data[0]);
    }

    mat4(vec4 a, vec4 b, vec4 c, vec4 d) {
        std::copy(&a.data[0], &a.data[0] + 4, &data[0]);
        std::copy(&b.data[0], &b.data[0] + 4, &data[0] + 4);
        std::copy(&c.data[0], &c.data[0] + 4, &data[0] + 8);
        std::copy(&d.data[0], &d.data[0] + 4, &data[0] + 12);
    }
    mat4() {};

    mat4(float a1, float a2, float a3, float a4,
        float b1, float b2, float b3, float b4,
        float c1, float c2, float c3, float c4,
        float d1, float d2, float d3, float d4) {
        float nums[16] = {
            a1, a2, a3, a4,
            b1, b2, b3, b4,
            c1, c2, c3, c4,
            d1, d2, d3, d4
        };
        std::copy(nums, nums + 16, data);
    }

    // casts an mat3 into a homogeneous mat4
    mat4(mat3 const& m, vec3 v = vec3(0,0,0)) {
        float nums[16] = {
            m[0][0], m[0][1], m[0][2], v.x,
            m[1][0], m[1][1], m[1][2], v.y,
            m[2][0], m[2][1], m[2][2], v.z,
            0,  0, 0, 1
        };
        std::copy(nums, nums + 16, data);
    }

    // Constructor from array of vec4
    mat4(const vec4 arr[4]) {
        std::copy(&arr[0].data[0], &arr[0].data[0] + 4, &data[0]);
        std::copy(&arr[1].data[0], &arr[1].data[0] + 4, &data[4]);
        std::copy(&arr[2].data[0], &arr[2].data[0] + 4, &data[8]);
        std::copy(&arr[3].data[0], &arr[3].data[0] + 4, &data[12]);
    }

    float det3() const {
        float n = data[0] * (data[5] * data[10] - data[6] * data[9]) -
            data[1] * (data[4] * data[10] - data[6] * data[8]) +
            data[2] * (data[4] * data[9] - data[5] * data[8]);
        return n;
    }
    mat4 operator*(const mat4& rhs) const {
        return mat4(Mbase<4>::operator*(rhs));
    }

    // matrix - vector multiplication
    vec4 operator*(vec4 const& vec) const {
        vec4 tmp(0, 0, 0, 0);
        for (unsigned int i = 0; i < 4; i++) {  // Changed to unsigned int
            for (unsigned int n = 0; n < 4; n++) {  // Changed to unsigned int
                tmp[i] += data[i * 4 + n] * vec[n];
            }
        }
        return(tmp);
    }

    //apply homogeneous transform to a vec3
    vec3 operator*(vec3 const& v) const {
        vec4 tmp(v, 1); // cast v onto a vec4
        tmp = *this * tmp;
        return (tmp.v4tv3()); // apply transform and cast back
    }

    // Copy assignment operator     
    mat4& operator=(const mat4& other) {
        if (this != &other) {
            for (unsigned int i = 0; i < 16; ++i) data[i] = other.data[i];  // Changed to unsigned int
        }
        return *this;
    }
};