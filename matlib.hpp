#pragma once

// contains matrix math functions
#define _USE_MATH_DEFINES
#include <math.h>
#include "matrix.hpp"
#include "QUATERNION.hpp"

inline mat4 translation4(vec4 const& vec) {
return (mat4(
    1, 0, 0, vec[0],
    0, 1, 0, vec[1],
    0, 0, 1, vec[2],
    0, 0, 0, vec[3]));
}
inline mat4 translation4(vec3 const& vec) {
    return (mat4(
        1, 0, 0, vec[0],
        0, 1, 0, vec[1],
        0, 0, 1, vec[2],
        0, 0, 0, 1));
}
inline mat4 translation4(float vec[4]) {
    return (mat4(
        1, 0, 0, vec[0],
        0, 1, 0, vec[1],
        0, 0, 1, vec[2],
        0, 0, 0, vec[3]));
}
inline mat4 translation4(float a, float b, float c, float d = 1) {
    return (mat4(
        1, 0, 0, a,
        0, 1, 0, b,
        0, 0, 1, c,
        0, 0, 0, d));
}

// rotation matricies across each axis

// rotation about the z axis (roll
inline mat3 Rz(float a) {
    float s = sin(a * static_cast<float>(M_PI) / 180.0f);
    float c = cos(a * static_cast<float>(M_PI) / 180.0f);
    return mat3(
        c, -s, 0,
        s,  c, 0,
        0,  0, 1);
}
// rotation about the x axis (yaw)
inline mat3 Rx(float a) {
    float s = sin(a * static_cast<float>(M_PI) / 180.0f);
    float c = cos(a * static_cast<float>(M_PI) / 180.0f);
    return mat3(
        1, 0, 0,
        0, c, -s,
        0, s, c );
}
// rotation about the y axis (pitch)
inline mat3 Ry(float a) {
    float s = sin(a * static_cast<float>(M_PI) / 180.0f);
    float c = cos(a * static_cast<float>(M_PI) / 180.0f);
    return mat3(
        c, 0, s,
        0, 1, 0,
       -s, 0, c);
}

// rotates y and z axis to correct for vulkan wierdness
inline mat4 vulkanAxisRotate() {
    return mat4(
        1, 0, 0, 0,
        0, -1, 0, 0,
        0, 0, -1, 0,
        0, 0, 0, 1);
}

// takes in the fov, znear, zfar, width and height and creates a perspective projection matrix
inline mat4 perspectiveProj(float ang = 90.0f, float n = 1, float f = 100, float a = 16.0f/9.0f) {
    const float q = f / (f - n); // get the z stretch
    const float t = 1 / tan(ang * 0.5f); // the scaling factor for x and y
    return mat4(
        t / a, 0, 0, 0,
        0, t, 0, 0,
        0, 0, q, -n * q,
        0, 0, 1, 0
    );
}
inline mat4 GLperspectiveProj(float ang = 90.0f, float n = 1, float f = 100, float a = 16.0f / 9.0f)
{
    const float t = 1 / tan(ang * static_cast<float>(M_PI) / 360.0f); // the scaling factor for x and y
    return mat4(
        t / a, 0, 0, 0,
        0, t, 0, 0,
        0, 0, (f + n) / (n - f), 2 * f * n / (n - f),
        0, 0, -1, 0);
}

// makes a matrix to project any orthographic volume to vulkan's cannonical orthographic volume (left right top bottom near far)
// not as useful if you are using a default camera
inline mat4 orthoSpaceTransMat(const float s, const float f, const float a) {
    // combination of a translation of the center to the origin and a stretch of the sides to fit the viewbox
    return mat4(
        1/(a * s), 0, 0, 0,
        0, 1/(s), 0, 0,
        0, 0, 1 / f, 0,
        0, 0, 0, 1);
}
inline mat4 makeVulkanOrtho(float l, float r, float b, float t, float n, float f) {
    return mat4(
        2.0f / (r - l), 0, 0, -(r + l) / (r - l),
        0, 2.0f / (t - b), 0, -(t + b) / (t - b),
        0, 0, 1.0f / (f - n), -n / (f - n),
        0, 0, 0, 1
    );
}

// converts a quaternion to a rotation matrix4
inline mat4 qtm4(quaternion const& q) {
    return mat4(
        1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z, 2.0f * q.x * q.y - 2.0f * q.z * q.w, 2.0f * q.x * q.z + 2.0f * q.y * q.w, 0.0f,
        2.0f * q.x * q.y + 2.0f * q.z * q.w, 1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z, 2.0f * q.y * q.z - 2.0f * q.x * q.w, 0.0f,
        2.0f * q.x * q.z - 2.0f * q.y * q.w, 2.0f * q.y * q.z + 2.0f * q.x * q.w, 1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}
// converts a quaternion to a rotation matrix3
inline mat3 qtm3(quaternion const& q) {
    return mat3(
        1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z, 2.0f * q.x * q.y - 2.0f * q.z * q.w, 2.0f * q.x * q.z + 2.0f * q.y * q.w, 
        2.0f * q.x * q.y + 2.0f * q.z * q.w, 1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z, 2.0f * q.y * q.z - 2.0f * q.x * q.w, 
        2.0f * q.x * q.z - 2.0f * q.y * q.w, 2.0f * q.y * q.z + 2.0f * q.x * q.w, 1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y);
}
inline vec3 Rq(vec3 const& v, quaternion q) {
    return q.rotate(v);
}

// it's the good old lerp
template <typename T>
T lerp(const T& a, const T& b, float t){
    return a * (1 - t) + b * t;
}   