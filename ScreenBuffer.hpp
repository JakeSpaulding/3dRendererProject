#ifndef SCREENBUFFER_HPP
#define SCREENBUFFER_HPP
#include "VertSettings.hpp"
#include "Materials.hpp"
#include "CameraDef.hpp"
#include "TriCastLib.hpp"

struct fbo {
	uint32_t screen_width;
	uint32_t screen_height;
	uint8_t samples;
	std::vector<vec2> sampleOffset;
	std::vector<Color> buffer;// initialize the buffer
	std::vector<Color> frame; // just stores the output frame
	std::vector<float> Zbuffer; // initialize the depth buffer
	std::vector<float> Zframe; // stores the output frame

	// set the msaa offset values
	void setMsaaOffset(std::vector<vec2> const& sam) {
		sampleOffset.resize(samples);
		// avoid slicing by intializing with vec2s
		for (int i = 0; i < samples; i++) sampleOffset[i] = vec2(sam[i]);
	}

	// constructor width height and samples and sample offset
	fbo(uint32_t sw, uint32_t sh, uint8_t upr = 1, std::vector<vec2> sam = { vec2(0,0) }) : screen_height(sh), samples(upr), screen_width(sw) {
		if (samples > 1) {
			buffer.resize(screen_height * screen_width * samples); // allocate size
			std::fill(&buffer[0].color, &buffer[0].color + buffer.size(), 0); // fill with zeros
			Zbuffer.resize(screen_height * screen_width * samples);
			std::fill(&Zbuffer[0], &Zbuffer[0] + Zbuffer.size(), 1.0f);
			sampleOffset.resize(samples);
			setMsaaOffset(sam);
		}
		Zframe.resize(screen_height * screen_width ); // allocate size
		frame.resize(screen_height * screen_width); // allocate size
		
		std::fill(&frame[0].color, &frame[0].color + frame.size(), 0); // fill with zeros
		std::fill(&Zframe[0], &Zframe[0] + Zframe.size(), 1.0f);
	}

	// converts the buffer to a fram
	void buffertFrame();
	// draws the range with MSAA
	void drawRangeMSAA(int xmin, int xmax, int ymin, int ymax, Vert const& v0, Vert const& v1, Vert const& v2, float area, 
		Material const& material);

	//draws pixels in the range provided
	void drawRange(int xmin, int xmax, int ymin, int ymax, 
		Vert const& v0, Vert const& v1, Vert const& v2, float area, Material const& material);

	// draws a Tri to the screen
	void drawTri(Vert const& a, Vert const& b, Vert const& c, Material const& material);

	// draws the mesh of an array of vertexes and indexes based off of an inputted camera
	void drawMesh(Mesh const& mesh, mat4 const& projMat);


};

#endif // SCREENBUFFER_HPP


