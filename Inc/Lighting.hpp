// contains functions and structs for lighting

#pragma once
#include "ScreenBuffer.hpp"

// point light, no shadows, radiates in all dirrections
struct pointLight {
	float intensity;
	vec3 pos;
	float radius;
	Color color;
};