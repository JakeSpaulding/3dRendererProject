#ifndef SCREENBUFFER_HPP
#define SCREENBUFFER_HPP
#include "VertSettings.hpp"
#include "Textures.hpp"
#include "CameraDef.hpp"
#include "TriCastLib.hpp"

struct fbo {
	uint32_t screen_width;
	uint32_t screen_height;
	uint8_t samples;
	std::vector<Color> buffer;// initialize the buffer
	std::vector<Color> frame; // just stores the output frame
	std::vector<float> Zbuffer; // initialize the depth buffer
	std::vector<float> Zframe; // stores the output frame

	// constructor width height and samples
	fbo(uint32_t sw, uint32_t sh, uint8_t upr = 1): screen_height(sh), samples(upr), screen_width(sw) {
		if (samples > 1) {
			buffer.resize(screen_height * screen_width * samples); // allocate size
			std::fill(&frame[0].color, &frame[0].color + frame.size(), 0); // fill with zeros
			Zbuffer.resize(screen_height * screen_width * samples);
			std::fill(&Zbuffer[0], &Zbuffer[0] + Zbuffer.size(), 1.0f);

		}
		Zframe.resize(screen_height * screen_width ); // allocate size
		frame.resize(screen_height * screen_width); // allocate size
		
		std::fill(&frame[0].color, &frame[0].color + frame.size(), 0); // fill with zeros
		std::fill(&Zframe[0], &Zframe[0] + Zframe.size(), 1.0f);
	}

	// draws the range with MSAA
	void drawRangeMSAA(int xmin, int xmax, int ymin, int ymax, Vert const& v0, Vert const& v1, Vert const& v2, float area, 
		Material const& material, std::vector<vec2> const& sampleOffsets);

	//draws pixels in the range provided
	void drawRange(int xmin, int xmax, int ymin, int ymax, 
		Vert const& v0, Vert const& v1, Vert const& v2, float area, Material const& naterial);

	// draws a Tri to the screen
	void drawTri(Vert const& a, Vert const& b, Vert const& c, Material const& material);

	// draws the mesh of an array of vertexes and indexes based off of an inputted camera
	void drawMesh(Mesh const& mesh,mat4 const& projMat) {
		// project the points
		VBO NDC;
		projectVBO(NDC, mesh.VB, projMat, screen_width, screen_height);
		// loop through the triangles (this can be multithreaded)
		for (int i = 0; i < mesh.EB.size(); i += 3) {
			drawTri(NDC[mesh.EB[i]], NDC[mesh.EB[i + 1]], NDC[mesh.EB[i + 2]], mesh.material);
		}
	}
};

#endif // SCREENBUFFER_HPP


