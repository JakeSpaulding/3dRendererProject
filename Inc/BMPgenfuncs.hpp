// contains functions to convert FBO and Zbuffer to bitmaps

#pragma once
#include "screenBuffer.hpp"

#pragma pack(push, 1)
struct BMPFileHeader {
    uint16_t bfType = 0x4D42; // 'BM'
    uint32_t bfSize = 54;
    uint16_t bfReserved1 = 0;
    uint16_t bfReserved2 = 0;
    uint32_t bfOffBits = 54;
};

struct BMPInfoHeader {
    uint32_t biSize = 40;
    int32_t  biWidth = 0;
    int32_t  biHeight = 0;
    uint16_t biPlanes = 1;
    uint16_t biBitCount = 24;
    uint32_t biCompression = 0;
    uint32_t biSizeImage = 0;
    int32_t  biXPelsPerMeter = 2835;
    int32_t  biYPelsPerMeter = 2835;
    uint32_t biClrUsed = 0;
    uint32_t biClrImportant = 0;
};
#pragma pack(pop)

// makes a bmp out of a frame buffer
static void FBOtBMP(const std::vector<Color>& FB, const std::string& filename, unsigned int w, unsigned int h) {

    // handle padding offsets
    unsigned int rowSize = w * 3;  // Changed to unsigned int
    unsigned int padding = (4 - (rowSize % 4)) % 4;  // Changed to unsigned int
    unsigned char pad[3] = { 0, 0, 0 };
    BMPFileHeader fheader;
    fheader.bfSize += 3 * w * h + 3 * padding;

    BMPInfoHeader iheader;
    iheader.biHeight = static_cast<int32_t>(h);  // Explicit cast for BMP format requirement
    iheader.biWidth = static_cast<int32_t>(w);   // Explicit cast for BMP format requirement
    iheader.biSizeImage = 3 * w * h;

    std::ofstream file(filename, std::ios::binary);
    file.write(reinterpret_cast<char*>(&fheader), sizeof(fheader));
    file.write(reinterpret_cast<char*>(&iheader), sizeof(iheader));

    // BMP stores rows bottom-to-top
    for (uint32_t y = 0; y < h; y++) {
        for (unsigned int x = 0; x < w; ++x) {  
            size_t i = static_cast<size_t>(y) * w + x;  
            file.write(reinterpret_cast<const char*>(&FB[i].b), 1);
            file.write(reinterpret_cast<const char*>(&FB[i].g), 1);
            file.write(reinterpret_cast<const char*>(&FB[i].r), 1);
        }
        file.write(reinterpret_cast<const char*>(pad), padding);
    }
    file.close();
}

// converts a zbuffer to a bmp
inline void ZBtBMP(const std::vector<float>& ZB, const std::string& filename, unsigned int w, unsigned int h) {     

    // handle padding offsets
    unsigned int rowSize = w * 3;  // Changed to unsigned int
    unsigned int padding = (4 - (rowSize % 4)) % 4;  // Changed to unsigned int
    unsigned char pad[3] = { 0, 0, 0 };

    BMPFileHeader fheader;
    fheader.bfSize = 3 * w * h + 54 + 3 * padding;

    BMPInfoHeader iheader;
    iheader.biHeight = static_cast<int32_t>(h);  // Explicit cast for BMP format requirement
    iheader.biWidth = static_cast<int32_t>(w);   // Explicit cast for BMP format requirement
    iheader.biSizeImage = 3 * w * h;
    
    std::ofstream file(filename, std::ios::binary); // Fixed from std::wios::binary
    file.write(reinterpret_cast<char*>(&fheader), sizeof(fheader));
    file.write(reinterpret_cast<char*>(&iheader), sizeof(iheader));

    // Write z-buffer data
    for (int y = static_cast<int>(h) - 1; y >= 0; --y) {  // Keep int for countdown loop
        for (unsigned int x = 0; x < w; ++x) {  // Changed to unsigned int
            size_t i = static_cast<size_t>(y) * w + x;  // Buffer index - use size_t
            uint8_t c = static_cast<uint8_t>(((1.0f - ZB[i]) * 255.0f) + 0.5f);  // Added missing variable declaration
            file.write(reinterpret_cast<const char*>(&c), 1);
            file.write(reinterpret_cast<const char*>(&c), 1);
            file.write(reinterpret_cast<const char*>(&c), 1);
        }
        file.write(reinterpret_cast<const char*>(pad), padding);
    }
    file.close();
}