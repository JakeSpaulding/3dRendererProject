#pragma once
#include "geometry.hpp"
#include "Lights.hpp"
// contains functions to shade pixels

// calculates the alpha of a on b (assuming no pre-multiplication)
inline Color alphaBlend(Color src, Color dst) {
	float srcA = src.a / 255.0f;
	float dstA = dst.a / 255.0f;
	// A0 = Aa + Ab * (1 - Aa)
	float A = srcA + dstA * (1 - srcA); // cache the blended alpbha
	Color out;
	// C0 = (Ca*Aa + Cb * Ab *(1 - Aa)) / A0 <- straight, not for pre multiplied
	out.r = static_cast<uint8_t>((src.r * srcA + dst.r * (1.0f - srcA))/A + 0.5f);
	out.g = static_cast<uint8_t>((src.g * srcA + dst.g * (1.0f - srcA))/A + 0.5f);
	out.b = static_cast<uint8_t>((src.b * srcA + dst.b * (1.0f - srcA))/A + 0.5f);
	out.a = static_cast<uint8_t>(A * 255.0f + 0.5f);
	return out;
}

// calculates the alpha of a on b (assuming premultiplied alpha
inline Color alphaBlendPremultiplied(Color src, Color dst) {
	// normalize alpha
	float srcA = src.a / 255.0f;
	float dstA = dst.a / 255.0f;

	Color out;
	// C0 = Ca*Aa + Cb * Ab *(1 - Aa) <- premultiplied
	out.r = static_cast<uint8_t>(src.r * srcA + dst.r * (1.0f - srcA) + 0.5f);
	out.g = static_cast<uint8_t>(src.g * srcA + dst.g * (1.0f - srcA) + 0.5f);
	out.b = static_cast<uint8_t>(src.b * srcA + dst.b * (1.0f - srcA) + 0.5f);
	out.a = static_cast<uint8_t>((srcA + dstA * (1 - srcA)) * 255.0f + 0.5f);
	return out;
}

// maps a pixel using UV attributes and bilinear filtering
inline Color mapBilinear(Vert const& v0, Vert const& v1, Vert const& v2, vec3 const& p, vec3 const& w, Material material) {
	// interpolate
	vec2 UV(baryInterpolate(w, v0.UV, v1.UV, v2.UV, p[2]));
	// get filtered pixel
	return material.texture.bilinear(UV.u, UV.v);
}

// maps a pixel using nearest neighbor filtering
inline Color mapNearestN(Vert const& v0, Vert const& v1, Vert const& v2, vec3 const& p, vec3 const& w, Material material) {
	vec2 UV(baryInterpolate(w, v0.UV, v1.UV, v2.UV, p[2]));
	return material.texture.nearestN(UV.u, UV.v);
}

// shades a pixel flat 
// params are pixel position (p) color of the texture (Ctex) normal (n) material, vector from pixel to light(l) and the light (in world space coords)
inline vec3 shadeLight(vec3 const& p, Color const& Ctex, vec3 const& n, vec3 const& cam, Material const& material, pointLight const& light) {
	vec3 l = (light.pos - p).normalized(); // vector from pixel to light
	float d = l.norm(); // distance from pixel to light
	if (d > light.radius) return vec3(0,0,0); // check if it is close enough to the light
	vec3 v = (p - cam).normalized();
	const float a = 1.0f;
	const float b = 1.0f;
	const float c = 1.0f;

	vec3 Kd(Ctex.r * material.Kd.r / 255.0f, Ctex.g * material.Kd.g / 255.0f, Ctex.b * material.Kd.b / 255.0f); // multiply the texture color by material Kd
	// compute halfway vector
	vec3 h = (l + v).normalized();

	vec3 Ia = material.Ka.elementWise(light.La);
	vec3 Id = Kd.elementWise(light.Ld) * max(n * l, 0.0f);
	vec3 Is = material.Ks.elementWise(light.Ls) * powf(n * h, material.shininess);

	// compute the rgb float values
	return Ia + (Id + Is) * (1.0f / (a + b * d + c * d * d));
}