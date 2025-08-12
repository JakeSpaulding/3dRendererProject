#pragma once


#include "Textures.hpp"

#include "vectors.hpp"
#include <string>;

// contains the material data for a mesh
struct Material {
	union {
		vec3 emission; // emmision color
		vec3 Ie;
	};
	union {
		vec3 ambientColor; // account for light scattered by the whole scene
		vec3 Ka;
	};
	union {
		vec3 diffuseColor; // the main color of the material (can be mapped to a texture) (0-1 for each r g b channel) (coeeficient of difuse reflection)
		vec3 Kd;
	};
	union {
		vec3 specularColor; // the color of specular reflections
		vec3 Ks;
	};
	union {
		float opacity;
		float d;
	};
	union{
		float shininess;
		float Ns;
	};
	union {
		float IOR;
		float Ni;
	};

	texture2d texture;
	uint8_t illum; // illumination model
    Material(vec3 ambient = vec3(1.0f,1.0f,1.0f), vec3 diffuse = vec3(0.64f, 0.64f, 0.64f),  vec3 specCol = vec3(0.5f, 0.5f, 0.5f ), float spec = 32.0f, float IO = 1.0f, float o = 1.0f, uint8_t il= 2, vec3 em = vec3(0.0f,0.0f,0.0f) ) 
						: ambientColor(ambient), diffuseColor(diffuse), specularColor(specCol), shininess(spec),IOR(IO),opacity(o), illum(il), emission(em) {}
	// loads the the texture of a given filename, if none is inputted it just loads whatever is stored in textureFile
	void loadTexture(const char* fname) {
		texture.loadTexturePNG(fname);

	}
	// loads an mtl file
	void loadMTL(const char* fname);

	// setter functions
	void setAmbientColor(const vec3& color) { ambientColor = color; }
	void setDiffuseColor(const vec3& color) { diffuseColor = color; }
	void setSpecularColor(const vec3& color) { specularColor = color; }
	void setOpacity(float value) { opacity = value; }
	void setSpecularity(float value) { shininess = value; }
	void setIOR(float value) { IOR = value; }
	void setTexture(const texture2d& tex) { texture = tex; }
	void setIllum(uint8_t model) { illum = model; }

};