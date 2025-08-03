#pragma once
#include "Textures.hpp"
#include <string>;

// contains the material data for a mesh
struct Material {

	vec3 ambientColor; // account for light scattered by the whole scene
	vec3 diffuseColor; // the main color of the material (can be mapped to a texture) (0-1 for each r g b channel)
	vec3 specularColor; // the color of specular reflections
	float opacity; // this is also handled by the alpha channel of the texture ( 0 - 1)
	float specularity; // accounts for how glossy it looks (0-1)
	float IOR;
	string textureFile;
	texture2d texture;



	Material(){
	}
};