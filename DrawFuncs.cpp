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
			vec3 w(edgeFunction(v1.pos, v2.pos, p), edgeFunction(v2.pos, v0.pos, p), edgeFunction(v0.pos, v1.pos, p));
			

			// check if the point is actually in the triangle
			if (w[0] >= 0 && w[1] >= 0 && w[2] >= 0) {

				w[0] /= (area * v0.pos[2]), w[1] /= (area * v1.pos[2]), w[2] /= (area * v2.pos[2]); // normalize & apply inverse z values

				// find the z coordinate of our pixel
				p.z = 1 / (w[0] + w[1] + w[2]);

				// ensure it is on the top of the frame
				if (p.z >= 0 && p.z < Zbuffer[y * screen_width + x]) {
					Zbuffer[y * screen_width + x] = p.z;
				}
			}
		}
	}
}

//draws pixels in the range provided using UV coordinates, abd a provided msaa offset vector vector
void fbo::drawRangeMSAA(int xmin, int xmax, int ymin, int ymax,
	Vert const& v0, Vert const& v1, Vert const& v2, float area, Material const& material, std::vector<vec2> const& sampleOffset) {

	// loop through pixels
	for (unsigned int x = xmin; x <= xmax; x++) {
		for (unsigned int y = ymin; y <= ymax; y++) {
			std::vector<vec3> p(samples);
			std::vector<vec3> w(samples); // stores the weights
			std::vector<bool> inTri(samples); // tells if it is in the tri
			std::vector<Color> c; // stores the sample color value
			int edgeCheck = 0;
			for (int s = 0; s < samples; s++) {
				p.push_back(vec3(x + sampleOffset[s].x, y + sampleOffset[s].y, 0));
				float w0 = edgeFunction(v1.pos, v2.pos, p[s]);
				float w1 = edgeFunction(v2.pos, v0.pos, p[s]);
				float w2 = edgeFunction(v0.pos, v1.pos, p[s]);
				
				// take barycentric weights
				w.push_back(vec3(w0,w1,w2));
				// check if it's in the triangle
				inTri.push_back(w[s][0] >= 0 && w[s][1] >= 0 && w[s][2] >= 0);
				edgeCheck += inTri[s];
			}
			// if it's fully in the triangle we can just shade one sample
			std::vector<Color> c;
			if (edgeCheck == samples) {
				w[0][0] /= (area * v0.pos[2]), w[0][1] /= (area * v1.pos[2]), w[0][2] /= (area * v2.pos[2]); // normalize & apply inverse z values
				p[0].z = 1 / (w[0][0] + w[0][1] + w[0][2]); // find z
				// update the z buffer
				Zbuffer[y * screen_width + x] = p[0].z;
				buffer[y * screen_width + x] = shadeBilinear(v0, v1, v2, p[0], w[0], material); // shade from one pixel
			}

			// if at least one sample is in the triangle we can then continue with the samples
			else if (edgeCheck > 0){
				// shade subpixels
				for (int s = 0; s < samples; s++){
					// if the sample is in the tri shade it, if not keep the old values
					if (inTri[s]) {
						w[s][0] /= (area * v0.pos[2]), w[s][1] /= (area * v1.pos[2]), w[s][2] /= (area * v2.pos[2]); // normalize & apply inverse z values
						// find the z coordinate of our pixel
						p[s].z = 1 / (w[s][0] + w[s][1] + w[s][2]);
					}
					else {
						// load the buffer data in for the samples outside
						p[s].z = Zbuffer[y * screen_width + x];
						c[s] = buffer[y * screen_height + x];
					}
				}
				// apply color and z buffer
			}
		}
	}
}


// draws a Tri to the screen
void fbo::drawTri(Vert const& a, Vert const& b, Vert const& c, Material const& material) {
	// compute bbox
	// Replace std::fminf with std::min and std::fmaxf with std::max  
	float xmin = std::min(std::min(a.pos[0], b.pos[0]), c.pos[0]);
	float xmax = std::max(std::max(a.pos[0], b.pos[0]), c.pos[0]);
	float ymin = std::min(std::min(a.pos[1], b.pos[1]), c.pos[1]);
	float ymax = std::max(std::max(a.pos[1], b.pos[1]), c.pos[1]);


	// cache the area
	float area = edgeFunction(a.pos, b.pos, c.pos);

	drawRange(floor(xmin), ceil(xmax), floor(ymin), ceil(ymax), a, b, c, area, material);
}