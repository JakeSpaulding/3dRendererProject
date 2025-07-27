#ifndef TRICASTLIB_HPP
#define TRICASTLIB_HPP
// contains functions for geometry math
#include "geometry.hpp"

// takes 2 vertts and a point, returns 0 if the point is on the line, > 0 of it is right, and <0 if it is left (it's the 2d cross product)
inline float edgeFunction(vec3 const& A, vec3 const& B, vec3 const& C) {
	return (A[0] - B[0]) * (A[1] - C[1]) - (A[1] - B[1]) * (A[0] - C[0]);

}
inline float edgeFunction(vec3 const& A, vec3 const& B, vec2 const& C) {
	return (A[0] - B[0]) * (A[1] - C[1]) - (A[1] - B[1]) * (A[0] - C[0]);
}

// interpolates vertex attributes c 
inline float VattInterpolate(const float w0, const float w1, const float w2, const float c0, const float c1, const float c2, const float z) {
	return z * (w0 * c0 + w1 * c1 + w2 * c2);
}
template <typename T>
inline T baryInterpolate(float w0, float w1, float w2, T c0, T c1, T c2, float z) {
	return z * (w0 * c0 + w1 * c1 + w2 * c2);
}

#endif