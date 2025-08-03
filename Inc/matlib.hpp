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
        0, c,-s,
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
        0,-1, 0, 0,
        0, 0,-1, 0,
        0, 0, 0, 1);
}

// converts from NDC to pixel coordinates and corrects aspect ratio
inline mat4 NDCtSc(unsigned int w, unsigned int h) {
    float halfW = static_cast<float>(w) * 0.5f;
    float halfH = static_cast<float>(h) * 0.5f;
    return mat4(
        halfW, 0,     0, halfW,  // Scale by half-width, translate by half-width
        0,     halfH, 0, halfH,  // Scale by half-height, translate by half-height  
        0,     0,     1, 0,      // Keep Z unchanged
        0,     0,     0, 1       // Homogeneous coordinate
    );
}

// Alternative version that handles aspect ratio correction in the projection
inline mat4 NDCtScWithAspect(unsigned int w, unsigned int h) {
    float halfW = static_cast<float>(w) * 0.5f;
    float halfH = static_cast<float>(h) * 0.5f;
    float aspect = static_cast<float>(w) / static_cast<float>(h);
    
    // If you want to maintain square pixels and fit to the smaller dimension
    if (aspect > 1.0f) {
        // Width is larger - scale down width to maintain aspect
        return mat4(
            halfH, 0,     0, halfW,
            0,     halfH, 0, halfH,
            0,     0,     1, 0,
            0,     0,     0, 1
        );
    } else {
        // Height is larger - scale down height to maintain aspect  
        return mat4(
            halfW, 0,     0, halfW,
            0,     halfW, 0, halfH,
            0,     0,     1, 0,
            0,     0,     0, 1
        );
    }
}

// takes in the fov, znear, zfar, width and height and creates a perspective projection matrix
inline mat4 perspectiveProj(float ang = 90.0f, float n = 1, float f = 100, float a = 1.0f) {
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

// converts an euler angle to a quaternion
inline quaternion etq(vec3 const& v) {
    // Convert degrees to radians
    float yaw = v.yaw * static_cast<float>(M_PI) / 180.0f;
    float pitch = v.pitch * static_cast<float>(M_PI) / 180.0f;
    float roll = v.roll * static_cast<float>(M_PI) / 180.0f;

    // Compute half angles
    float cy = cos(yaw * 0.5f);
    float sy = sin(yaw * 0.5f);
    float cp = cos(pitch * 0.5f);
    float sp = sin(pitch * 0.5f);
    float cr = cos(roll * 0.5f);
    float sr = sin(roll * 0.5f);

    // Yaw-Pitch-Roll (Y-X-Z) order
    quaternion q;
    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;
    return q;
}
// converts eluer to quaternion
inline quaternion etq(float yaw, float pitch, float roll) {
    // Convert degrees to radians
    yaw = yaw * static_cast<float>(M_PI) / 180.0f;
    pitch = pitch * static_cast<float>(M_PI) / 180.0f;
    roll = roll * static_cast<float>(M_PI) / 180.0f;

    // Compute half angles
    float cy = cos(yaw * 0.5f);
    float sy = sin(yaw * 0.5f);
    float cp = cos(pitch * 0.5f);
    float sp = sin(pitch * 0.5f);
    float cr = cos(roll * 0.5f);
    float sr = sin(roll * 0.5f);

    // Yaw-Pitch-Roll (Y-X-Z) order
    quaternion q;
    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;
    return q;
}

// Converts a quaternion to Euler angles (yaw, pitch, roll) in degrees.
// Returns a vec3: x = yaw, y = pitch, z = roll.
inline vec3 quaternionToEuler(const quaternion& q) {
    // Extract the values from quaternion
    float x = q.x, y = q.y, z = q.z, w = q.w;

    // Yaw (Y axis rotation)
    float siny_cosp = 2.0f * (w * y + z * x);
    float cosy_cosp = 1.0f - 2.0f * (y * y + x * x);
    float yaw = std::atan2(siny_cosp, cosy_cosp);

    // Pitch (X axis rotation)
    float sinp = 2.0f * (w * x - y * z);
    float pitch;
    if (std::abs(sinp) >= 1)
        pitch = std::copysign(static_cast<float>(M_PI) / 2.0f, sinp); // use 90 degrees if out of range
    else
        pitch = std::asin(sinp);

    // Roll (Z axis rotation)
    float sinr_cosp = 2.0f * (w * z + x * y);
    float cosr_cosp = 1.0f - 2.0f * (z * z + x * x);
    float roll = std::atan2(sinr_cosp, cosr_cosp);

    // Convert radians to degrees
    return vec3(
        yaw * 180.0f / static_cast<float>(M_PI),
        pitch * 180.0f / static_cast<float>(M_PI),
        roll * 180.0f / static_cast<float>(M_PI)
    );
}

// converts angle axis to quaternion (angle axis is in vec4)
inline quaternion aatq(vec4 const& v) {
    float ang = v.w * float(M_PI) / 180.0f;
    quaternion q;
    q.x = v.x * sin(ang / 2);
    q.y = v.y * sin(ang / 2);
    q.z = v.z * sin(ang / 2);
    q.w = cos(ang / 2);
    return q;
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

// converts euler angles to a mat3 rotation matrix
inline mat3 etm3(vec3 const& v) {
    return(qtm3(etq(v)));
}

// converts euler angles to a mat3
inline mat3 etm3(float ry, float rx, float rz) {
    return(qtm3(etq(ry, rx, rz)));
}
// converts euler angles to a mat4
inline mat4 etm4(vec3 const& v) {
    return mat4(etm3(v));
}
// converts euler angles to a mat4
inline mat4 eulertM4(float ry, float rx, float rz) {
    return mat4(etm3(ry, rx, rz));
}


// it's the good old lerp
template <typename T>
T lerp(const T& a, const T& b, float t){
    return a * (1 - t) + b * t;
}   

