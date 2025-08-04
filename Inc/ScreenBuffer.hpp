#pragma once
#include "VertSettings.hpp"
#include "Materials.hpp"
#include "CameraDef.hpp"
#include "TriCastLib.hpp"

struct fbo {
	unsigned int screen_width;
	unsigned int screen_height;
	uint8_t samples;
	std::vector<vec2> sampleOffset;
	std::vector<Color> buffer;// initialize the buffer
	std::vector<Color> frame; // just stores the output frame
	std::vector<float> Zbuffer; // initialize the depth buffer
	std::vector<float> Zframe; // stores the output frame

	// set the msaa offset values
	void setMsaaOffset(std::vector<vec2> const& sam) {
		sampleOffset.resize(samples);
		// avoid slicing by initializing with vec2s
		for (unsigned int i = 0; i < samples; i++) sampleOffset[i] = vec2(sam[i]);  // Changed to unsigned int
	}

	// constructor width height and samples and sample offset
	fbo(unsigned int sw, unsigned int sh, uint8_t upr = 1, std::vector<vec2> sam = { vec2(0,0) }) : screen_height(sh), samples(upr), screen_width(sw) {
		if (samples > 1) {
			// Cast to size_t for vector operations
			size_t buffer_size = static_cast<size_t>(screen_height) * screen_width * samples;
			buffer.resize(buffer_size); // allocate size
			std::fill(&buffer[0], &buffer[0] + buffer.size(), 0); // fill with zeros
			Zbuffer.resize(buffer_size);
			std::fill(&Zbuffer[0], &Zbuffer[0] + Zbuffer.size(), 1.0f);
			sampleOffset.resize(samples);
			setMsaaOffset(sam);
		}
		size_t frame_size = static_cast<size_t>(screen_height) * screen_width;
		Zframe.resize(frame_size); // allocate size
		frame.resize(frame_size); // allocate size
		
		std::fill(&frame[0], &frame[0] + frame.size(), 0); // fill with zeros
		std::fill(&Zframe[0], &Zframe[0] + Zframe.size(), 1.0f);
	}
	// draws a mesh using threads to tiles
	void threadDrawInitializer(Mesh const& mesh, vector<vec3> const& projVBO, uint32_t tileSize = 256);
	// converts the buffer to a frame
	void buffertFrame();
	// draws the range with MSAA
	void drawRangeMSAA(int xmin, int xmax, int ymin, int ymax, Vert const& v0, Vert const& v1, Vert const& v2, float area, 
		Material const& material);
	// draws a tile of the 
	void drawTile(Mesh const& mesh, vector<vec3> const& scVBO, int xmin, int xmax, int ymin, int ymax);
	//draws pixels in the range provided
	void drawRange(int xmin, int xmax, int ymin, int ymax, 
		Vert const& v0, Vert const& v1, Vert const& v2, float area, Material const& material);

	// draws a Tri to the screen
	void drawTri(Vert const& a, Vert const& b, Vert const& c, Material const& material);

	// draws the mesh of an array of vertexes and indexes based off of an inputted camera
	void drawMesh(Mesh const& mesh, mat4 const& projMat);
	// draws the mesh multithreaded style
	void drawMeshThreaded(Mesh const& mesh, Camera const& cam, unsigned int tileSize = 32);  // Changed to unsigned int

	// allows the passing of multiple meshes
	void drawMeshesThreaded(vector<Mesh> const& meshes, Camera const& cam, unsigned int tileSize = 32);  // Changed to unsigned int

};


