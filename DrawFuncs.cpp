#pragma once
#include "ScreenBuffer.hpp"
#include "PixelShadeFuncs.hpp"
#include <array>

//draws pixels in the range provided using UV coordinates
void fbo::drawRange(int xmin, int xmax, int ymin, int ymax, 
	Vert const& v0, Vert const& v1, Vert const& v2, float area, Material const& material) {
	for (unsigned int x = xmin; x <= xmax; x++) {
		for (unsigned int y = ymin; y <= ymax; y++) {

			vec3 p(x, y, 0); // assign the point we are checking

			// take barycentric weights
			float w0 = edgeFunction(v1.pos, v2.pos, p);
			float w1 = edgeFunction(v2.pos, v0.pos, p);
			float w2 = edgeFunction(v0.pos, v1.pos, p);

			// check if the point is actually in the triangle
			if (w0 >= 0 && w1 >= 0 && w2 >= 0) {

				w0 /= (area * v0.pos[2]), w1 /= (area * v1.pos[2]), w2 /= (area * v2.pos[2]); // normalize & apply inverse z values

				// find the z coordinate of our pixel
				p.z = 1 / (w0 + w1 + w2);

				// ensure it is on the top of the frame
				if (p.z >= 0 && p.z < Zbuffer[y * screen_width + x]) {
					Zbuffer[y * screen_width + x] = p.z;
					
				}
			}
		}
	}
}

//draws pixels in the range provided using UV coordinates
void fbo::drawRangeMSAA(int xmin, int xmax, int ymin, int ymax,
	Vert const& v0, Vert const& v1, Vert const& v2, float area, Material const& material, std::vector<vec2> const& sampleOffset) {
	int numSamples = sampleOffset.size();// get our number of samples

	// loop through pixels
	for (unsigned int x = xmin; x <= xmax; x++) {
		for (unsigned int y = ymin; y <= ymax; y++) {
			std::vector<vec3> p(numSamples);
			std::vector<float> w(numSamples * 3); // stores the weights
			for (int s = 0; s < numSamples; s++) {

			}
			vec3 p(x, y, 0); // assign the point we are checking

			// take barycentric weights
			float w0 = edgeFunction(v1.pos, v2.pos, p);
			float w1 = edgeFunction(v2.pos, v0.pos, p);
			float w2 = edgeFunction(v0.pos, v1.pos, p);

			// check if the point is actually in the triangle
			if (w0 >= 0 && w1 >= 0 && w2 >= 0) {

				w0 /= (area * v0.pos[2]), w1 /= (area * v1.pos[2]), w2 /= (area * v2.pos[2]); // normalize & apply inverse z values

				// find the z coordinate of our pixel
				p.z = 1 / (w0 + w1 + w2);

				// ensure it is on the top of the frame
				if (p.z >= 0 && p.z < Zbuffer[y * screen_width + x]) {
					Zbuffer[y * screen_width + x] = p.z;
				}
			}
		}
	}
}


// draws a Tri to the screen
void fbo::drawTri(Vert const& a, Vert const& b, Vert const& c, texture2d const& tex) {
	// compute bbox
	// Replace std::fminf with std::min and std::fmaxf with std::max  
	float xmin = std::min(std::min(a.pos[0], b.pos[0]), c.pos[0]);
	float xmax = std::max(std::max(a.pos[0], b.pos[0]), c.pos[0]);
	float ymin = std::min(std::min(a.pos[1], b.pos[1]), c.pos[1]);
	float ymax = std::max(std::max(a.pos[1], b.pos[1]), c.pos[1]);


	// cache the area
	float area = edgeFunction(a.pos, b.pos, c.pos);

	drawRange(floor(xmin), ceil(xmax), floor(ymin), ceil(ymax), a, b, c, area, tex);
}