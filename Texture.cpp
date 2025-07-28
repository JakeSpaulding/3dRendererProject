#include "Textures.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <fstream>
#include <iostream>


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
	img.resize(w * h); // set the size of the vector

	file.seekg(54); // set our read head to the beggining of the pixel data
	
	// create a vector to store the row
	int rowSize = ((w * 3 + 3) / 4) * 4;


	std::vector<unsigned char> row(rowSize);
	for (int y = h - 1; y > 0; y--) {
		// read the whole row at once
		file.read(reinterpret_cast<char*>(row.data()), rowSize);
		for (int x = w - 1; x > 0; x--) {
			// write the color data
			img[y * w + x].b = row[x * 3];
			img[y * w + x].g = row[x * 3 + 1];
			img[y * w + x].r = row[x * 3 + 2];
			img[y * w + x].a = 0xff; // set the alpha channel because we don't have one
		}
	}
	file.close();
}
// loads a png file into the texture stuct
void texture2d::loadTexturePNG(const char* filename) {
	int width, height, channels;
	unsigned char* data = stbi_load(filename, &width, &height, &channels, 4); // force RGBA

	if (!data) throw std::runtime_error("Failed to load PNG texture");

	w = width;
	h = height;
	img.resize(w * h);

	for (unsigned int i = 0; i < w * h; ++i) {
		img[i].r = data[4 * i + 0];
		img[i].g = data[4 * i + 1];
		img[i].b = data[4 * i + 2];
		img[i].a = data[4 * i + 3];
	}

	stbi_image_free(data);
}