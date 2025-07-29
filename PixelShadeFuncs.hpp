#pragma once
#include "geometry.hpp"
// contains functions to shade pixels

// shades a pixel using color attributes

#ifdef USING_PVC
Color shadeColor(Vert const& v0, Vert const& v1, Vert const& v2, vec3 const& p, vec3 const& w) {
	Color c;
	c.r = uint8_t((VattInterpolate(w, v0.r, v1.r, v2.r, p.z) * 255) + 0.5f);
	c.g = uint8_t((VattInterpolate(w, v0.g, v1.g, v2.g, p.z) * 255) + 0.5f);
	c.b = uint8_t((VattInterpolate(w, v0.b, v1.b, v2.b, p.z) * 255) + 0.5f);
	c.a = 0xff; // no alpha for now
	return c;
}
#endif

// shades a pixel using UV attributes and bilinear filtering
Color shadeBilinear(Vert const& v0, Vert const& v1, Vert const& v2, vec3 const& p, vec3 const& w, Material material) {
	// interpolate
	vec2 UV(baryInterpolate(w, v0.UV, v1.UV, v2.UV, p[2]));
	// get filtered pixel
	return material.texture.bilinear(UV.u, UV.v);
}

// shades a pixel using nearest neighbor filtering
Color shadeNearestN(Vert const& v0, Vert const& v1, Vert const& v2, vec3 const& p, vec3 const& w, Material material) {
	vec2 UV(baryInterpolate(w, v0.UV, v1.UV, v2.UV, p[2]));
	return material.texture.nearestN(UV.u, UV.v);
}
