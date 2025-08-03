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
	}

	stbi_image_free(data);
}