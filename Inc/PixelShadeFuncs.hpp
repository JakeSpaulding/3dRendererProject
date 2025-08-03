#pragma once
#include "geometry.hpp"
// contains functions to shade pixels

// calculates the alpha of a on b
inline Color alphaBlend(Color src, Color dst) {
	float srcA = src.a / 255.0f;
	float dstA = dst.a / 255.0f;
	Color out;
	out.r = static_cast<uint8_t>(src.r * srcA + dst.r * (1.0f - srcA));
	out.g = static_cast<uint8_t>(src.g * srcA + dst.g * (1.0f - srcA));
	out.b = static_cast<uint8_t>(src.b * srcA + dst.b * (1.0f - srcA));
	out.a = static_cast<uint8_t>((srcA + dstA * (1.0f - srcA)) * 255.0f);
	return out;
}

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
inline Color shadeBilinear(Vert const& v0, Vert const& v1, Vert const& v2, vec3 const& p, vec3 const& w, Material material) {
	// interpolate
	vec2 UV(baryInterpolate(w, v0.UV, v1.UV, v2.UV, p[2]));
	// get filtered pixel
	return material.texture.bilinear(UV.u, UV.v);
}

// shades a pixel using nearest neighbor filtering
inline Color shadeNearestN(Vert const& v0, Vert const& v1, Vert const& v2, vec3 const& p, vec3 const& w, Material material) {
	vec2 UV(baryInterpolate(w, v0.UV, v1.UV, v2.UV, p[2]));
	return material.texture.nearestN(UV.u, UV.v);
}
