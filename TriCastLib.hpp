#pragma once
// contains functions for geometry math
#include "geometry.hpp"
// applies a matrix transform to all points in a vbo and outputs it to vboout
inline void projectVBO(std::vector<vec3>& VBOout, std::vector<vec3> const& VBOin, mat4 projMat, int w, int h) {
	VBOout.clear(); // clear the output
	VBOout.resize(VBOin.size()); // reserve the propper size

	for (int i = 0; i < VBOin.size(); i++) {
		VBOout[i] = projMat * VBOin[i]; // project it

		// w = screen width, h = screen height
		VBOout[i][0] = ((VBOout[i][0] + 1.0f) * 0.5f) * w; // convert to pixel centered coordinates
		VBOout[i][1] = ((VBOout[i][1] + 1.0f) * 0.5f) * h;
		std::cout << "\n" << VBOout[i] << "\n";
	}
}

// takes 2 vertts and a point, returns 0 if the point is on the line, > 0 of it is right, and <0 if it is left (it's the 2d cross product)
inline float edgeFunction(vec3 const& A, vec3 const& B, vec3 const& C) {
	return (A[0] - B[0]) * (A[1] - C[1]) - (A[1] - B[1]) * (A[0] - C[0]);

}
inline float edgeFunction(vec3 const& A, vec3 const& B, vec2 const& C) {
	return (A[0] - B[0]) * (A[1] - C[1]) - (A[1] - B[1]) * (A[0] - C[0]);
}

// interpolates vertex attributes c 
inline float VattInterpolate(vec3 const& w, const float c0, const float c1, const float c2, const float z) {
	return z * (w[0] * c0 + w[1] * c1 + w[2] * c2);
}

// interpolates vertex attributes of any type
template <typename T>
inline T baryInterpolate(vec3 const& w, T c0, T c1, T c2, float z) {
	return z * (w[0] * c0 + w[1] * c1 + w[2] * c2);
}