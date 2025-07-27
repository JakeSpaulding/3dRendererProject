#include "ScreenBuffer.hpp"
#include <cstdint>

unsigned int Screen_x = 1920;
unsigned int Screen_y = 1080;
unsigned int filesize = 3 * Screen_x * Screen_y + 54;
unsigned int imagesize = 3 * Screen_x * Screen_y;

#pragma pack(push, 1)
struct BMPFileHeader {
    uint16_t bfType = 0x4D42; // 'BM'
    uint32_t bfSize = filesize;
    uint16_t bfReserved1 = 0;
    uint16_t bfReserved2 = 0;
    uint32_t bfOffBits = 54;
};

struct BMPInfoHeader {
    uint32_t biSize = 40;
    int32_t  biWidth = Screen_x;
    int32_t  biHeight = Screen_y;
    uint16_t biPlanes = 1;
    uint16_t biBitCount = 24;
    uint32_t biCompression = 0;
    uint32_t biSizeImage = imagesize;
    int32_t  biXPelsPerMeter = 2835;
    int32_t  biYPelsPerMeter = 2835;
    uint32_t biClrUsed = 0;
    uint32_t biClrImportant = 0;
};
#pragma pack(pop)

// makes a bmp out of a frame buffer
static void FBOtBMP(const std::vector<Color>& FB, const std::string& filename, int scalex, int scaley) {
    BMPFileHeader fheader;
    BMPInfoHeader iheader;
    iheader.biHeight = scaley;
    iheader.biWidth = scalex;
    std::ofstream file(filename, std::ios::binary);
    file.write(reinterpret_cast<char*>(&fheader), sizeof(fheader));
    file.write(reinterpret_cast<char*>(&iheader), sizeof(iheader));

    int rowSize = scalex * 3;
    int padding = (4 - (rowSize % 4)) % 4;
    unsigned char pad[3] = {0, 0, 0};

    // BMP stores rows bottom-to-top
    for (int y = scaley - 1; y >= 0; --y) {
        for (int x = 0; x < scalex; ++x) {
            int i = y * scalex + x;
            file.write(reinterpret_cast<const char*>(&FB[i].b), 1);
            file.write(reinterpret_cast<const char*>(&FB[i].g), 1);
            file.write(reinterpret_cast<const char*>(&FB[i].r), 1);
        }
        file.write(reinterpret_cast<const char*>(pad), padding);
    }
    file.close();
}

static void ZBtBMP(const std::vector<float>& ZB, const std::string& filename) {
    BMPFileHeader fheader;
    BMPInfoHeader iheader;
    std::ofstream file(filename, std::wios::binary); // open the file
    file.write(reinterpret_cast<char*>(&fheader), sizeof(fheader));
    file.write(reinterpret_cast<char*>(&iheader), sizeof(iheader));

    // hopefully writes to the file
    for (int i = Screen_x * Screen_y - 1; i >= 0; i--) {
        uint8_t c = uint8_t(((1 - ZB[i]) * 255) + 0.5f);
        file.write(reinterpret_cast<const char*>(&c), 1);
        file.write(reinterpret_cast<const char*>(&c), 1);
        file.write(reinterpret_cast<const char*>(&c), 1);
    }
    file.close();
}
int main(void) {
    Vert a(vec3(1.0f, 0, -1.0f), vec2(0,0));
    Vert b(vec3(-1.0f, 0, -6.0f), vec2(1,0));
    Vert c(vec3(0, 1.0f, -3.0f), vec2 (0,1));

    texture2d pix;
    pix.loadTexturePNG("Untitled Drawing.png");
    VBO vb;
    EBO eb;
    Mesh mesh(vb, eb);

    // Add triangles to the mesh
    mesh.newTri(a, b, c); // Main triangle
    FBOtBMP(pix.img, "textestout.bmp", pix.w, pix.h);
    // Create a camera looking at the origin from z = 5
    CameraP camera1(vec3(0, 0, 1), quaternion(0,0,0,1), 90.0f, 1.0f, 10.0f, float(Screen_x) / float(Screen_y));
    std::cout << camera1.projMat << "\n";
    CameraO cam2(vec3(0, 0, 1), quaternion(0, 0, 0, 1), 5 ,10, float(Screen_x) / float(Screen_y));
    std::cout << camera1.projMat << "\n";

    // Create framebuffer and render
    fbo screenframe(Screen_x, Screen_y);
    screenframe.drawMesh(mesh, camera1.projMat, pix);

    // Write framebuffer to BMP
    FBOtBMP(screenframe.buffer, "test2.bmp", Screen_x, Screen_y);
    ZBtBMP(screenframe.Zbuffer, "zbtest.bmp");

    return 0;
}