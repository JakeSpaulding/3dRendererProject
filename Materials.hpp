#pragma once
#include "Textures.hpp"

// contains the material data for a mesh
struct Material {
	texture2d texture;

	Material(const char* filename) {
		texture.loadTexturePNG(filename);
	}
	Material(){
	}
};