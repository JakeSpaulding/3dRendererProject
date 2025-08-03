#pragma once
#include "ScreenBuffer.hpp"
#include "PixelShadeFuncs.hpp"
#include <thread>
#include <chrono>

using namespace std;

// moves the buffer into the frame
void fbo::buffertFrame() {
	for (unsigned int y = 0; y < screen_height; y++) {
		// loop through the pixels
		for (unsigned int x = 0; x < screen_width; x++) {
			const size_t idf = static_cast<size_t>(y) * screen_width + x; // Buffer index - use size_t
			const size_t idb = idf * samples; // Buffer index - use size_t
			float R = 0, G = 0, B = 0, A = 0; // stores the sum of the colors
			Zframe[idf] = Zbuffer[idb]; // store the first buffer value
			// loop through samples
			for (unsigned int s = 0; s < samples; s++) { // Graphics iteration - unsigned int
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
	for (int x = xmin; x <= xmax; x++) { // Keep int - can be negative during clipping
		for (int y = ymin; y <= ymax; y++) { // Keep int - can be negative during clipping

			vec3 p(static_cast<float>(x), static_cast<float>(y), 0.0f); // assign the point we are checking
			// take barycentric weights
			vec3 w(
				edgeFunction(v1.pos, v2.pos, p),
				edgeFunction(v2.pos, v0.pos, p),
				edgeFunction(v0.pos, v1.pos, p));
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

//draws pixels in the range provided using UV coordinates, and a provided msaa offset vector
void fbo::drawRangeMSAA(int xmin, int xmax, int ymin, int ymax,
	Vert const& v0, Vert const& v1, Vert const& v2, float area, Material const& material) {

	// loop through pixels
	for (int y = ymin; y <= ymax; y++) { // Keep int - can be negative during clipping
		for (int x = xmin; x <= xmax; x++) { // Keep int - can be negative during clipping
			int cached = -1;
			const size_t id = static_cast<size_t>(y * screen_width + x) * samples; // Buffer index - use size_t

			// loop through samples
			for (unsigned int s = 0; s < samples; s++) { // Graphics iteration - unsigned int
				// save the point value
				vec3 p(static_cast<float>(x) + sampleOffset[s][0], static_cast<float>(y) + sampleOffset[s][1], 0);

				// calculate barycentric weights
				float w0 = edgeFunction(v1.pos, v2.pos, p);
				float w1 = edgeFunction(v2.pos, v0.pos, p);
				float w2 = edgeFunction(v0.pos, v1.pos, p);
				
				// take barycentric weights
				vec3 w(w0, w1, w2);
				// check if it's in the triangle
				if (w[0] >= 0 && w[1] >= 0 && w[2] >= 0) {
					// check if we have called the frag shader yet
					if (cached == -1) {
						w[0] /= (area * v0.pos[2]), w[1] /= (area * v1.pos[2]), w[2] /= (area * v2.pos[2]); // normalize & apply inverse z values
						// find the z coordinate of our pixel
						p[2] = 1.0f / (w[0] + w[1] + w[2]);
						// set z buffer
						Zbuffer[id + s] = p[2];
						buffer[id + s] = shadeBilinear(v0, v1, v2, p, w, material);
						cached = static_cast<int>(id + s); // Cast for comparison
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
	float xmin = max(min(min(a.pos[0], b.pos[0]), c.pos[0]), 0.0f);
	float xmax = min(max(max(a.pos[0], b.pos[0]), c.pos[0]), static_cast<float>(screen_width) - 1.0f);
	float ymin = max(min(min(a.pos[1], b.pos[1]), c.pos[1]), 0.0f);
	float ymax = min(max(max(a.pos[1], b.pos[1]), c.pos[1]), static_cast<float>(screen_height) - 1.0f);

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
	vector<vec3> NDC;
	projectVBO(NDC, mesh.VBO, projMat);
	// loop through the triangles (this can be multithreaded)
	for (size_t i = 0; i < mesh.VEBO.size(); i += 3) { // Container iteration - use size_t
		// initialize Vert structs to make life easier
		Vert a(NDC[mesh.VEBO[i]], mesh.UV[mesh.UVEBO[i]], mesh.Normals[mesh.NEBO[i]]);
		Vert b(NDC[mesh.VEBO[i + 1]], mesh.UV[mesh.UVEBO[i + 1]], mesh.Normals[mesh.NEBO[i + 1]]);
		Vert c(NDC[mesh.VEBO[i + 2]], mesh.UV[mesh.UVEBO[i + 2]], mesh.Normals[mesh.NEBO[i + 2]]);
		drawTri(a, b, c, mesh.material);
	}
	if (samples > 1) {
		buffertFrame();
	}
}

void fbo::drawTile(Mesh const& mesh, vector<vec3> const& NDC, int txmin, int txmax, int tymin, int tymax) {
	// loop through each tri
	for (size_t i = 0; i < mesh.VEBO.size(); i += 3) { // Container iteration - use size_t
		// initialize Vert structs to make life easier
		Vert a(NDC[mesh.VEBO[i]], mesh.UV[mesh.UVEBO[i]], mesh.Normals[mesh.NEBO[i]]);
		Vert b(NDC[mesh.VEBO[i + 1]], mesh.UV[mesh.UVEBO[i + 1]], mesh.Normals[mesh.NEBO[i + 1]]);
		Vert c(NDC[mesh.VEBO[i + 2]], mesh.UV[mesh.UVEBO[i + 2]], mesh.Normals[mesh.NEBO[i + 2]]);

		// find our bbox
		int xmin = static_cast<int>(floor(max(min({a.pos[0], b.pos[0], c.pos[0]}), static_cast<float>(txmin))));
		int xmax = static_cast<int>(ceil(min(max({a.pos[0], b.pos[0], c.pos[0]}), static_cast<float>(txmax))));
		int ymin = static_cast<int>(floor(max(min({a.pos[1], b.pos[1], c.pos[1]}), static_cast<float>(tymin))));
		int ymax = static_cast<int>(ceil(min(max({a.pos[1], b.pos[1], c.pos[1]}), static_cast<float>(tymax))));
		
		// cache the area
		float area = edgeFunction(a.pos, b.pos, c.pos);

		// if we are using msaa
		if (samples > 1) {
			drawRangeMSAA(xmin, xmax, ymin, ymax, a, b, c, area, mesh.material);
		}
		else {
			drawRange(xmin, xmax, ymin, ymax, a, b, c, area, mesh.material);
		}
	}
}

// threaded draw funcs, tile size is the sqrt of the number of pixels each thread will take
void fbo::drawMeshThreaded(Mesh const& mesh, Camera const& cam, unsigned int tileSize) { // Graphics parameter - unsigned int
	// project points
	auto start = chrono::high_resolution_clock::now();
	mat4 projMat = NDCtScWithAspect(screen_width, screen_height) * cam.getProjectionMatrix(screen_width, screen_height);
	vector<vec3> NDC;
	projectVBO(NDC, mesh.VBO, projMat);
	auto end = chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "Projection time: " << duration.count() << " milliseconds" << std::endl;
	
	size_t numThreads = static_cast<size_t>(ceil(static_cast<float>(screen_height * screen_width) / static_cast<float>(tileSize * tileSize))); // Thread count - use size_t
	vector<thread> threads;
	threads.reserve(numThreads);

	// break into tiles
	for (unsigned int y = 0; y < screen_height; y += tileSize) { // Graphics coordinates - unsigned int
		for (unsigned int x = 0; x < screen_width; x += tileSize) { // Graphics coordinates - unsigned int
			// determine if the tile is in range, if not truncate it
			int txmax = static_cast<int>(min(screen_width, x + tileSize)) - 1;
			int tymax = static_cast<int>(min(screen_height, y + tileSize)) - 1;
			// create a thread
			threads.emplace_back([=, &mesh, &NDC] {
				this->drawTile(mesh, NDC, static_cast<int>(x), txmax, static_cast<int>(y), tymax);
			});
		}
	}
	
	end = chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "Thread init time: " << duration.count() << " milliseconds" << std::endl;
	
	for (auto& t : threads) {
		t.join();
	}
	
	end = chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "Thread join time: " << duration.count() << " milliseconds" << std::endl;
	// move the buffer to the frame
	if (samples > 1) buffertFrame();
}

void fbo::drawMeshesThreaded(vector<Mesh> const& meshes, Camera const& cam, unsigned int tileSize) {
	if (meshes.empty()) return;
	
	auto start = chrono::high_resolution_clock::now();
	mat4 projMat = cam.getProjectionMatrix(screen_width,screen_height);

}