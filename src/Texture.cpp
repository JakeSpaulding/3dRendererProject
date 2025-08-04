#include "Textures.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <fstream>

// loads a bmp file into the texture buffer
void texture2d::loadTextureBMP(const char* filename) {
	std::ifstream file(filename, std::ios::binary);
	if (!file) throw std::runtime_error("Cannot open file");
	int width, height;
	// get our size parameters
	file.seekg(18); // move to the width byte
	file.read(reinterpret_cast<char*>(&width), 4); // set the width
	file.read(reinterpret_cast<char*>(&height), 4); // set the height
	w = width;
	h = height;
	img.resize(static_cast<size_t>(w) * h); // Buffer size - use size_t

	file.seekg(54); // set our read head to the beginning of the pixel data
	
	// create a vector to store the row
	unsigned int rowSize = ((static_cast<unsigned int>(w) * 3 + 3) / 4) * 4;  // Changed to unsigned int

	std::vector<unsigned char> row(rowSize);
	for (int y = h - 1; y >= 0; y--) {  // Keep int for countdown loop
		// read the whole row at once
		file.read(reinterpret_cast<char*>(row.data()), rowSize);
		for (int x = w - 1; x >= 0; x--) {  // Keep int for countdown loop
			// write the color data
			size_t idx = static_cast<size_t>(y) * w + x;  // Buffer index - use size_t
			img[idx].b = row[x * 3];
			img[idx].g = row[x * 3 + 1];
			img[idx].r = row[x * 3 + 2];
			img[idx].a = 0xff; // set the alpha channel because we don't have one
		}
	}
	opaque = 1;
	file.close();
}

// loads a png file into the texture struct
void texture2d::loadTexturePNG(const char* filename) {
	int width, height, channels;
	unsigned char* data = stbi_load(filename, &width, &height, &channels, 4); // force RGBA

	if (!data) throw std::runtime_error("Failed to load PNG texture");

	w = width;
	h = height;
	img.resize(static_cast<size_t>(w) * h);  // Buffer size - use size_t

	for (size_t i = 0; i < static_cast<size_t>(w) * h; ++i) {  // Container iteration - use size_t
		img[i].r = data[4 * i + 0];
		img[i].g = data[4 * i + 1];
		img[i].b = data[4 * i + 2];
		img[i].a = data[4 * i + 3];
		if (img[i].a != 255) {
			opaque = 0;
		}
	}

	stbi_image_free(data);
}

// applies bilinear filtering
const Color texture2d::bilinear(float u, float v) const{
	// wrap coordinates
	u = wrap(u), v = wrap(v);
	// get the pixel coords
	float x = u * (w - 1), y = v * (h - 1);

	unsigned int x0 = static_cast<unsigned int>(floor(x));  // Changed to unsigned int
	unsigned int x1 = std::min(x0 + 1, w - 1);  // Changed to unsigned int
	unsigned int y0 = static_cast<unsigned int>(floor(y));  // Changed to unsigned int
	unsigned int y1 = std::min(y0 + 1, h - 1);  // Changed to unsigned int

	float fx = x - x0;
	float fy = y - y0;

	// Get corner pixels
	Color c00 = getPix(x0, y0);
	Color c10 = getPix(x1, y0);
	Color c01 = getPix(x0, y1);
	Color c11 = getPix(x1, y1);

	// Interpolate each channel
	Color result;
	result.r = static_cast<uint8_t>(
		(1 - fx) * (1 - fy) * c00.r +
		fx * (1 - fy) * c10.r +
		(1 - fx) * fy * c01.r +
		fx * fy * c11.r
		);
	result.g = static_cast<uint8_t>(
		(1 - fx) * (1 - fy) * c00.g +
		fx * (1 - fy) * c10.g +
		(1 - fx) * fy * c01.g +
		fx * fy * c11.g
		);
	result.b = static_cast<uint8_t>(
		(1 - fx) * (1 - fy) * c00.b +
		fx * (1 - fy) * c10.b +
		(1 - fx) * fy * c01.b +
		fx * fy * c11.b
		);
	result.a = static_cast<uint8_t>(
		(1 - fx) * (1 - fy) * c00.a +
		fx * (1 - fy) * c10.a +
		(1 - fx) * fy * c01.a +
		fx * fy * c11.a
		);
	return result;
}