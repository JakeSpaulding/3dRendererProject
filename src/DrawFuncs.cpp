#pragma once
#include "buffers/ScreenBuffer.hpp"
#include "PixelShadeFuncs.hpp"
#include <thread>
#include <chrono>
#include "matlib.hpp"
#include "RasterEngine2.h"
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

				// set Zframe to the nearest z value
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

//draws pixels in the range provided using UV coordinates, and a provided msaa offset vector
void fbo::drawRange(int xmin, int xmax, int ymin, int ymax,
	Vert const& v0, Vert const& v1, Vert const& v2, float area, Material const& material,
	vector<pointLight> const& lights, mat4 const& invmat, vec3 const& cam) {

	// compute face normal
	vec3 Normal = (v1.pos - v0.pos).cross(v2.pos - v0.pos).normalized();

	// loop through pixels
	for (int y = ymin; y <= ymax; y++) {
		for (int x = xmin; x <= xmax; x++) {

			// indicate if we have called the frag shader yet
			int cached = -1;
			const size_t id = static_cast<size_t>(y * screen_width + x) * samples; // Buffer index - use size_t

			// loop through samples
			for (unsigned int s = 0; s < samples; s++) { // Graphics iteration - unsigned int
				// save the point value
				vec3 p(static_cast<float>(x) + sampleOffset[s][0], static_cast<float>(y) + sampleOffset[s][1], 0);
				// cout << "x: " << x << " y: " << y << " id: " << id << " sampleOffset: " << sampleOffset[s] << " p: " << p << endl;
				// take barycentric weights
				vec3 w(edgeFunction(v1.pos, v2.pos, p), edgeFunction(v2.pos, v0.pos, p), edgeFunction(v0.pos, v1.pos, p));

				//  skip if the pixel is not in the triangle
				if (w[0] < 0 || w[1] < 0 || w[2] < 0) {
					continue;
				}

				w[0] /= (area * v0.pos[2]), w[1] /= (area * v1.pos[2]), w[2] /= (area * v2.pos[2]); // apply inverse z to weights
				// find the z coordinate of our pixel
				p.z = 1.0f / (w[0] + w[1] + w[2]);

				// depth test
				if (p.z <  0 || p.z > Zbuffer[id + s]) {
					continue;
				}

				// check if we have called the frag shader yet
				if (cached != -1) {
					Zbuffer[id + s] = Zbuffer[cached];
					buffer[id + s] = buffer[cached];
					continue;
				}

				// map the texture to the pixel
				Color c = mapBilinear(v0, v1, v2, p, w, material);

				// apply flat lighting if appropriate
				if (material.illum) {
					vec3 worldPos = invmat * p; // get the world pos
					vec3 colorSum = material.Ie; // set the initial lit color to be the emmisive color
					// shade smooth
					if (material.illum > 1) {
						Normal = baryInterpolate<vec3>(w, v0.N, v1.N, v2.N, worldPos.z).normalized();
					}
					// loop through lights
					for (int i = 0; i < lights.size(); i++) {
						// calculate the lighting
						colorSum = colorSum + shadeLight(worldPos, c, Normal, cam, material, lights[i]);
					}
					// apply lighting
					c.r = uint8_t(min(gammaCorrect(colorSum.r, GAM) * 255.0f + 0.5f, 255.0f));
					c.g = uint8_t(min(gammaCorrect(colorSum.g, GAM) * 255.0f + 0.5f, 255.0f));
					c.b = uint8_t(min(gammaCorrect(colorSum.b, GAM) * 255.0f + 0.5f, 255.0f));
				}

				//apply alpha blend if transparent
				if (c.a == 255) {
					// set z buffer
					Zbuffer[id + s] = p.z;
					buffer[id + s] = c;
				}
				else {
					buffer[id + s] = alphaBlend(c, buffer[id + s]);
				}
					// cache the shaded value
				cached = static_cast<unsigned int>(id + s); // Cast for comparison
			}
		}
	}
}

void fbo::drawTile(Mesh const& mesh, vector<vec3> const& NDC, int txmin, int txmax, int tymin, int tymax,
	vector<pointLight> const& lights, mat4 const& invmat, vec3 const& cam){
	// loop through each tri
	for (size_t i = 0; i < mesh.VEBO.size(); i += 3) {
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
		if (area == 0.0f) continue; // skip degenerate triangles
			drawRange(xmin, xmax, ymin, ymax, a, b, c, area, mesh.material, lights, invmat, cam);
	}
}


void fbo::threadDrawInitializer(Mesh const& mesh, vector<vec3> const& NDC, 
	vector<pointLight> const& lights, mat4 const& invmat, vec3 const& cam, uint32_t tileSize) {
	size_t numThreads = static_cast<size_t>(ceil(static_cast<float>(screen_height * screen_width) / static_cast<float>(tileSize * tileSize))); // Thread count - use size_t
	vector<thread> threads;
	threads.reserve(numThreads);
	// break into tiles
	uint32_t tcount = 0;
	for (unsigned int y = 0; y < screen_height; y += tileSize) { // Graphics coordinates - unsigned int
		for (unsigned int x = 0; x < screen_width; x += tileSize) { // Graphics coordinates - unsigned int
			// determine if the tile is in range, if not truncate it
			int txmax = static_cast<int>(min(screen_width, x + tileSize)) - 1;
			int tymax = static_cast<int>(min(screen_height, y + tileSize)) - 1;
			// create a thread
			//cout << "thread# " << tcount << " innit\n";
			threads.emplace_back([=, &mesh, &NDC] {
				this->drawTile(mesh, NDC, static_cast<int>(x), txmax, static_cast<int>(y), tymax, lights, invmat, cam);
				});
			tcount++;
		}
	}
	for (auto& t : threads) {
		t.join();
	}
}


// threaded draw funcs, tile size is the sqrt of the number of pixels each thread will take
void fbo::drawMeshThreaded(Mesh const& mesh, Camera const& cam, vector<pointLight> const& lights, unsigned int tileSize) { // Graphics parameter - unsigned int
	// project points

	mat4 projMat = NDCtSc(screen_width, screen_height) * cam.getProjectionMatrix(screen_width,screen_height);
	mat4 invmat = cam.getInvProjectionMatrix(screen_width,screen_height) * invNDCtSc(screen_width, screen_height);
	vector<vec3> NDC;
	projectVBO(NDC, mesh.VBO, projMat);

	threadDrawInitializer(mesh, NDC, lights, invmat, cam.pos, tileSize);
	// move the buffer to the frame
	buffertFrame();
}

// draws multiple meshes and handles transparency
void fbo::drawMeshesThreaded(vector<Mesh> const& meshes, Camera const& cam, vector<pointLight> const& lights, unsigned int tileSize) {
	if (meshes.empty()) return;

	mat4 projMat = NDCtSc(screen_width, screen_height) * cam.getProjectionMatrix(screen_width,screen_height); // create the projection mat
	mat4 invmat = cam.getInvProjectionMatrix(screen_width,screen_height) * invNDCtSc(screen_width, screen_height);

	// make a vector of projected vbos
	vector<vector<vec3>> projVBOs(meshes.size());
	vector<float> depths(meshes.size()); // the projected centers
	// project the vbos
	for (size_t i = 0; i < meshes.size(); i++) {
		projectVBO(projVBOs[i], meshes[i].VBO, projMat);
		depths[i] = (projMat * meshes[i].bboxCenter).z;
	}

	vector<size_t> opqEBO; // stores the index of opaque meshes
	vector<size_t> tEBO; // stores the index of transparent meshes
	opqEBO.reserve(meshes.size()); // reserve the max size, just to be safe
	tEBO.reserve(meshes.size()); // reserve the max size, just to be safe

	// sort by opacity
	for (size_t i = 0; i < meshes.size(); i++) {
		if (meshes[i].material.texture.opaque) {
			opqEBO.push_back(i);
		}
		else {
			tEBO.push_back(i);
		}
	}
	// sort the transparent meshes
	std::sort(tEBO.begin(), tEBO.end(), [&](size_t a, size_t b) {
		return depths[a] > depths[b]; // furthest first
		});


	// draw opaque
	for (size_t i : opqEBO) {
		threadDrawInitializer(meshes[i], projVBOs[i], lights, invmat, cam.pos, tileSize);
	}


	// draw alpha
	for (size_t i : tEBO) {
		threadDrawInitializer(meshes[i], projVBOs[i], lights, invmat, cam.pos, tileSize);
	}

	// swap msaa buffer
	buffertFrame();
}