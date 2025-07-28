#pragma once
#include "ScreenBuffer.hpp"
#include "PixelShadeFuncs.hpp"
#include <array>

//draws pixels in the range provided using UV coordinates
void fbo::drawRange(int xmin, int xmax, int ymin, int ymax, 
	Vert const& v0, Vert const& v1, Vert const& v2, float area, Material const& material) {
	for (int x = xmin; x <= xmax; x++) {
		for (int y = ymin; y <= ymax; y++) {

			vec3 p(static_cast<float>(x), static_cast<float>(y), 0.0f); // assign the point we are checking
			// take barycentric weights
			vec3 w(
				static_cast<float>(edgeFunction(v1.pos, v2.pos, p)),
				static_cast<float>(edgeFunction(v2.pos, v0.pos, p)),
				static_cast<float>(edgeFunction(v0.pos, v1.pos, p))
			);

			// check if the point is actually in the triangle
			if (w[0] >= 0 && w[1] >= 0 && w[2] >= 0) {

				w[0] /= (area * v0.pos[2]), w[1] /= (area * v1.pos[2]), w[2] /= (area * v2.pos[2]); // normalize & apply inverse z values

				// find the z coordinate of our pixel
				p.z = static_cast<float>(1.0) / (w[0] + w[1] + w[2]);

				// ensure it is on the top of the frame
				if (p.z >= 0 && p.z < Zframe[y * screen_width + x]) {
					// update our frame buffers
					Zframe[y * screen_width + x] = p.z;
					frame[y * screen_width + x] = shadeBilinear(v0, v1, v2, p, w, material);
				}
			}
		}
	}
}

//draws pixels in the range provided using UV coordinates, abd a provided msaa offset vector vector
void fbo::drawRangeMSAA(int xmin, int xmax, int ymin, int ymax,
	Vert const& v0, Vert const& v1, Vert const& v2, float area, Material const& material) {

	// loop through pixels
	for (int x = xmin; x <= xmax; x++) {
		for (	int y = ymin; y <= ymax; y++) {
			std::vector<vec3> p(samples);
			std::vector<vec3> w(samples); // stores the weights
			std::vector<bool> inTri(samples ); // tells if it is in the tri
			int edgeCheck = 0;
			const int id = y * screen_width + x;
			for (int s = 0; s < samples; s++) {
				p.push_back(vec3(
					static_cast<float>(x) + static_cast<float>(sampleOffset[s].x),
					static_cast<float>(y) + static_cast<float>(sampleOffset[s].y),
					0.0f
				));
				float w0 = static_cast<float>(edgeFunction(v1.pos, v2.pos, p[s]));
				float w1 = static_cast<float>(edgeFunction(v2.pos, v0.pos, p[s]));
				float w2 = static_cast<float>(edgeFunction(v0.pos, v1.pos, p[s]));
				
				// take barycentric weights
				w.push_back(vec3(w0, w1, w2));
				// check if it's in the triangle
				inTri.push_back(w[s][0] >= 0 && w[s][1] >= 0 && w[s][2] >= 0);
				edgeCheck += inTri[s];
			}
			// if it's fully in the triangle we can just shade one sample
			std::vector<Color> c;
			if (edgeCheck == samples) {
				w[0][0] /= (area * v0.pos[2]), w[0][1] /= (area * v1.pos[2]), w[0][2] /= (area * v2.pos[2]); // normalize & apply inverse z values
				p[0][2] = static_cast<float>(1.0) / (w[0][0] + w[0][1] + w[0][2]); // find z
				// update the buffers
				std::fill(Zbuffer.data() + id, Zbuffer.data() + id + samples, p[0][2]);
				std::fill(buffer.data() + id, buffer.data() + id + samples, shadeBilinear(v0, v1, v2, p[0], w[0], material));// shade from one pixel
			}

			// if at least one sample is in the triangle we can then continue with the samples
			else if (edgeCheck > 0){
				// shade subpixels
				for (int s = 0; s < samples; s++){
					// if the sample is in the tri shade it, if not keep the old values
					if (inTri[s]) {
						w[s][0] /= (area * v0.pos[2]), w[s][1] /= (area * v1.pos[2]), w[s][2] /= (area * v2.pos[2]); // normalize & apply inverse z values
						// find the z coordinate of our pixel
						p[s][2]= static_cast<float>(1.0) / (w[s][0] + w[s][1] + w[s][2]);
						// set z buffer
						Zbuffer[id + s] = p[s][2];
						buffer[id + s] = shadeBilinear(v0, v1, v2, p[s], w[s], material);
					}
				}
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

	// if we are using msaa
	if (samples > 1) {
		drawRangeMSAA(static_cast<int>(floor(xmin)), static_cast<int>(ceil(xmax)), static_cast<int>(floor(ymin)), static_cast<int>(ceil(ymax)), a, b, c, area, material);
	}
	else {
		drawRange(static_cast<int>(floor(xmin)), static_cast<int>(ceil(xmax)), static_cast<int>(floor(ymin)), static_cast<int>(ceil(ymax)), a, b, c, area, material);
	}

}

void fbo::drawMesh(Mesh const& mesh, mat4 const& projMat) {
	// project the points
	VBO NDC;
	projectVBO(NDC, mesh.VB, projMat, screen_width, screen_height);
	// loop through the triangles (this can be multithreaded)
	for (int i = 0; i < mesh.EB.size(); i += 3) {
		drawTri(NDC[mesh.EB[i]], NDC[mesh.EB[i + 1]], NDC[mesh.EB[i + 2]], mesh.material);
	}
}