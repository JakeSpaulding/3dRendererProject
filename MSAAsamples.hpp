// contains sample offsets for the msaa

#pragma once
#include "vectors.hpp"
#include <vector>

// 4x with a standard and equal, square offset
std::vector<vec2> MSAA4xSquare({
	vec2(-0.25f, -0.25f),
	vec2(0.25f, -0.25f),
	vec2(-0.25f, 0.25f),
	vec2(0.25f, 0.25f)
	});

// 2x MSAA - Rotated grid
std::vector<vec2> MSAA2xRotated({
    vec2(-0.25f, -0.25f),
    vec2(0.25f, 0.25f)
    });

// 4x MSAA - Rotated grid (DirectX style)
std::vector<vec2> MSAA4xRotated({
    vec2(-0.125f, -0.375f),
    vec2(0.375f, -0.125f),
    vec2(-0.375f, 0.125f),
    vec2(0.125f, 0.375f)
    });

// 8x MSAA - Rotated grid (DirectX style)
std::vector<vec2> MSAA8xRotated({
    vec2(-0.1875f,  -0.5625f),
    vec2(0.5625f,  -0.4375f),
    vec2(-0.5625f,  -0.1875f),
    vec2(0.4375f,   0.1875f),
    vec2(0.1875f,   0.5625f),
    vec2(-0.4375f,   0.4375f),
    vec2(0.0625f,  -0.0625f),
    vec2(-0.3125f,   0.3125f)
    });

// 16x MSAA - Rotated grid (NVIDIA/DirectX style, example pattern)
std::vector<vec2> MSAA16xRotated({
    vec2(0.0625f,  0.5625f),
    vec2(0.3125f,  0.3125f),
    vec2(0.5625f,  0.0625f),
    vec2(-0.0625f, -0.4375f),
    vec2(-0.3125f, -0.6875f),
    vec2(-0.5625f, -0.3125f),
    vec2(-0.4375f,  0.1875f),
    vec2(-0.6875f,  0.4375f),
    vec2(-0.1875f,  0.6875f),
    vec2(0.4375f, -0.1875f),
    vec2(0.6875f, -0.5625f),
    vec2(0.1875f, -0.6875f),
    vec2(-0.6875f, -0.0625f),
    vec2(-0.4375f,  0.5625f),
    vec2(-0.1875f, -0.3125f),
    vec2(0.5625f, -0.6875f)
    });