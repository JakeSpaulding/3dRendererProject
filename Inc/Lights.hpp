// contains functions and structs for lighting

#pragma once
#include "ScreenBuffer.hpp"

// point light, no shadows, radiates in all dirrections
struct pointLight {
	//Luminance values
	union {
		mat3 L;
		struct { vec3 
			La, 
			Ld, 
			Ls; };
		struct {
			float
				Lra, Lga, Lba,
				Lrd, Lgd, Lbd,
				Lrs, Lgs, Lbs;
		};
	};
	vec3 pos;
	float radius;

	// Default constructor
	pointLight()
		: La(0.2f, 0.2f, 0.2f),   // Ambient (dim white)
		  Ld(0.8f, 0.8f, 0.8f),   // Diffuse (bright white)
		  Ls(1.0f, 1.0f, 1.0f),   // Specular (full white)
		  pos(0.0f, 0.0f, 0.0f),  // Origin
		  radius(10.0f)            // Default radius
	{}
};