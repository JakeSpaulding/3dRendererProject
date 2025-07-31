#pragma once
#include "ScreenBuffer.hpp"
#include "PixelShadeFuncs.hpp"

// moves the buffer into the frame
void fbo::buffertFrame() {
	for (unsigned int y = 0; y < screen_height; y++) {
		// loop through the pixels
		for (unsigned int x = 0; x < screen_width; x++) {
			const int idf = (y * screen_width + x); // the frame id
			const int idb = idf * samples; // the buffer id
			float R = 0, G = 0, B = 0, A = 0; // stores the sum of the colors
			Zframe[idf] = Zbuffer[idb]; // store the first buffer value
			// loop through samples
			for (int s = 0; s < samples; s++) {
				// add the buffer colors
				R += static_cast<float>(buffer[idb + s].r);
				G += static_cast<float>(buffer[idb + s].g);
				B += static_cast<float>(buffer[idb + s].b);
				A += static_cast<float>(buffer[idb + s].a);

				// compare the zbuffer and update if it is lower
				if (Zbuffer[idb + s] < Zframe[idf]) Zframe[idf] = Zbuffer[idb + s];
			}
			// average the color values
			R /= samples, G /= samples, B /= samples, A /= samples;
			// update frame value by converting to int and rounding
			frame[idf].r = static_cast<uint8_t>(R + 0.5f);
			frame[idf].g = static_cast<uint8_t>(G + 0.5f);
			frame[idf].b = static_cast<uint8_t>(B + 0.5f);
			frame[idf].a = static_cast<uint8_t>(A + 0.5f);
		}
	}
}
//draws pixels in the range provided using UV coordinates
void fbo::drawRange(int xmin, int xmax, int ymin, int ymax, 
	Vert const& v0, Vert const& v1, Vert const& v2, float area, Material const& material) {
	for (int x = xmin; x <= xmax; x++) {
		for (int y = ymin; y <= ymax; y++) {

			vec3 p(static_cast<float>(x), static_cast<float>(y), 0.0f); // assign the point we are checking
			// take barycentric weights
			//std::cout << "\nv0: " << v0.pos << v0.UV << "\nv1: " << v1.pos << v1.UV << "\nv2: " << v2.pos << v2.UV << " P: " << p;
			//std::cout << "edge func: " << edgeFunction(v1.pos, v2.pos, p);
			vec3 w(
				edgeFunction(v1.pos, v2.pos, p),
				edgeFunction(v2.pos, v0.pos, p),
				edgeFunction(v0.pos, v1.pos, p));
			//std::cout << "\nv0: "<< v0.pos<<v0.UV<<"\nv1: "<<v1.pos<<v1.UV<<"\nv2: "<<v2.pos<<v2.UV<<"weights:  "<< w<<"\n";
			// check if the point is actually in the triangle
			if (w[0] >= 0 && w[1] >= 0 && w[2] >= 0) {

				w[0] /= (area * v0.pos[2]), w[1] /= (area * v1.pos[2]), w[2] /= (area * v2.pos[2]); // normalize & apply inverse z values

				// find the z coordinate of our pixel
				p[2] = 1.0f / (w[0] + w[1] + w[2]);

				// ensure it is on the top of the frame
				if (p[2] >= 0 && p[2] < Zframe[y * screen_width + x]) {
					// update our frame buffers
					Zframe[y * screen_width + x] = p[2];
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
		for (int y = ymin; y <= ymax; y++) {
			int cached = -1;
			const int id = (y * screen_width + x) * samples;

			// loop through samples
			for (int s = 0; s < samples; s++) {
				// save the point value
				vec3 p(static_cast<float>(x) + sampleOffset[s][0], static_cast<float>(y) + sampleOffset[s][0], 0);

				// calculate barycentric weights
				float w0 = edgeFunction(v1.pos, v2.pos, p);
				float w1 = edgeFunction(v2.pos, v0.pos, p);
				float w2 = edgeFunction(v0.pos, v1.pos, p);
				
				// take barycentric weights
				vec3 w(w0, w1, w2);
				//::cout << "weights: " << w;
				// check if it's in the triangle
				if (w[0] >= 0 && w[1] >= 0 && w[2] >= 0) {
					// check if we have called the frag shader yet
					if (cached == -1) {
						w[0] /= (area * v0.pos[2]), w[1] /= (area * v1.pos[2]), w[2] /= (area * v2.pos[2]); // normalize & apply inverse z values
						// find the z coordinate of our pixel
						p[2] = 1.0f / (w[0] + w[1] + w[2]);
						// set z buffer
						Zbuffer[id + s] = p[2];
						buffer[id + s] = shadeBilinear(v0, v1, v2, p[s], w[s], material);
						cached = id + s;
					}
					// add cached values
					else {
						Zbuffer[id + s] = Zbuffer[cached];
						buffer[id + s] = buffer[cached];
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
	float xmin = std::max(std::min(std::min(a.pos[0], b.pos[0]), c.pos[0]), 0.0f);
	float xmax = std::min(std::max(std::max(a.pos[0], b.pos[0]), c.pos[0]),screen_width * 1.0f - 1.0f);
	float ymin = std::max(std::min(std::min(a.pos[1], b.pos[1]), c.pos[1]), 0.0f);
	float ymax = std::min(std::max(std::max(a.pos[1], b.pos[1]), c.pos[1]),screen_height * 1.0f - 1.0f);


	// cache the area
	float area = edgeFunction(a.pos, b.pos, c.pos);

	// if we are using msaa
	if (samples > 1) {
		drawRangeMSAA(static_cast<int>(floor(xmin)), static_cast<int>(ceil(xmax)), static_cast<int>(floor(ymin)), static_cast<int>(ceil(ymax)), a, b, c, area, material);
		buffertFrame();
	}
	else {
		drawRange(static_cast<int>(floor(xmin)), static_cast<int>(ceil(xmax)), static_cast<int>(floor(ymin)), static_cast<int>(ceil(ymax)), a, b, c, area, material);
	}

}

void fbo::drawMesh(Mesh const& mesh, mat4 const& projMat) {
	// project the points
	std::vector<vec3> NDC;
	projectVBO(NDC, mesh.VBO, projMat);
	// loop through the triangles (this can be multithreaded)
	for (int i = 0; i < mesh.VEBO.size(); i += 3) {
		// initialize Vert structs to make life easier
		Vert a(NDC[mesh.VEBO[i]], mesh.UV[mesh.UVEBO[i]], mesh.Normals[mesh.NEBO[i]]);
		Vert b(NDC[mesh.VEBO[i + 1]], mesh.UV[mesh.UVEBO[i + 1]], mesh.Normals[mesh.NEBO[i + 1]]);
		Vert c(NDC[mesh.VEBO[i + 2]], mesh.UV[mesh.UVEBO[i + 2]], mesh.Normals[mesh.NEBO[i + 2]]);
		drawTri(a, b, c, mesh.material);
	}
}


