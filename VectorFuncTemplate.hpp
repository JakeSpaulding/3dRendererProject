// contains templates for vector math

#pragma once
#include <algorithm>
#include <iostream>
#include <cstdint>

// vector addition
template<typename T, int N>
inline void vecAdd(T& v, T const& v1, T const& v2) {
    for (int i = 0; i < N; i++) {
        v[i] = v1[i] + v2[i];
    }
}

// vector subtraction
template<typename T, int N>
inline void vecSub(T& v, T const& v1, T const& v2) {
    for (int i = 0; i < N; i++) {
        v[i] = v1[i] - v2[i];
    }
}

// scalar multiplication
template<typename T, int N>
inline void vecScale(T& v, T const& v1, float n) {
    for (int i = 0; i < N; i++) {
        v[i] = v1[i] * n;
    }
}

// dot product
template<typename T, int N>
inline float vecDot(T const& v1, T const& v2) {
    float tmp(0.0f);
    for (int i = 0; i < N; i++)
        tmp += v1[i] * v2[i];
    return tmp;
}

// norm (length)
template<typename T, int N>
inline float vecNorm(T const& v) {
    return std::sqrt(vecDot<T, N>(v, v));
}

// normalize in place
template<typename T, int N>
inline void vecNormalize(T& v) {
    float n = vecNorm<T, N>(v);
    if (n) {
        for (int i = 0; i < N; i++) {
            v[i] /= n;
        }
    }
}

// returns a normalized copy
template<typename T, int N>
inline void vecNormalized(T& out, T const& v) {
    float n = vecNorm<T, N>(v);
    if (n) {
        for (int i = 0; i < N; i++) {
            out[i] = v[i] / n;
        }
    } else {
        for (int i = 0; i < N; i++) {
            out[i] = v[i];
        }
    }
}

// checks if the two vectors have equal values
template<typename T, int N>
inline bool eq(T const& v1, T const& v2) {
    for (int i = 0; i < N; i++) if (v1[i] != v2[i]) return false;
    return true;
}

// print vector
template<typename T, int N>
inline void vecPrint(T const& v) {
    std::cout << "[";
    for (int i = 0; i < N; ++i) {
        std::cout << v[i];
        if (i != N - 1) std::cout << ", ";
    }
    std::cout << "]";
}