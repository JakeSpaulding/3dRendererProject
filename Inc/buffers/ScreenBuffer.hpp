#pragma once
#include "Materials.hpp"
#include "CameraDef.hpp"
#include "TriCastLib.hpp"
#include "Lights.hpp"

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
		for (unsigned int i = 0; i < samples; i++) sampleOffset[i] = sam[i];  // Changed to unsigned int
	}

	// constructor width height and samples and sample offset
	fbo(unsigned int sw, unsigned int sh, std::vector<vec2> sam = { vec2(0,0) }) : screen_height(sh), screen_width(sw) {
		samples = sam.size(); // get the number of samples from the msaa buffer itself

		// Cast to size_t for vector operations
		size_t buffer_size = static_cast<size_t>(screen_height) * screen_width * samples;
		buffer.resize(buffer_size); // allocate size

		fill(&buffer[0], &buffer[0] + buffer.size(), 0); // fill with zeros

		Zbuffer.resize(buffer_size);
		fill(&Zbuffer[0], &Zbuffer[0] + Zbuffer.size(), 1.0f);

		size_t frame_size = static_cast<size_t>(screen_height) * screen_width;

		Zframe.resize(frame_size); // allocate size
		frame.resize(frame_size); // allocate size
		
		fill(&frame[0], &frame[0] + frame.size(), 0); // fill with zeros
		fill(&Zframe[0], &Zframe[0] + Zframe.size(), 1.0f);

		sampleOffset.resize(samples);
		setMsaaOffset(sam);
	}
	// draws a mesh using threads to tiles
	void threadDrawInitializer(Mesh const& mesh, vector<vec3> const& projVBO,
		vector<pointLight> const& lights, mat4 const& invmat, vec3 const& cam, uint32_t tileSize = 256);
	// converts the buffer to a frame
	void buffertFrame();
	// draws a tile of the screen within the bounds indicated
	void drawTile(Mesh const& mesh, vector<vec3> const& scVBO, int xmin, int xmax, int ymin, int ymax,
		vector<pointLight> const& lights, mat4 const& invmat, vec3 const& cam);
	//draws pixels in the range provided
	void drawRange(int xmin, int xmax, int ymin, int ymax, 
		Vert const& v0, Vert const& v1, Vert const& v2, float area, Material const& material,
		vector<pointLight> const& lights, mat4 const& invmat, vec3 const& cam);
	// draws the mesh of an array of vertexes and indexes based off of an inputted camera
	void drawMesh(Mesh const& mesh, Camera const& cam, vector<pointLight> const& lights);
	// draws the mesh multithreaded style
	void drawMeshThreaded(Mesh const& mesh, Camera const& cam, vector<pointLight> const& lights, uint32_t tileSize = 256);  
	// allows the passing of multiple meshes
	void drawMeshesThreaded(vector<Mesh> const& meshes, Camera const& cam, vector<pointLight> const& lights, uint32_t tileSize = 256);  

};


