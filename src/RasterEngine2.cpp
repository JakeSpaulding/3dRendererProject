// RasterEngine2.cpp : Defines the entry point for the application.
//
#pragma once
#include "RasterEngine2.h"
#include <cstdint>
#include "MSAAsamples.hpp"
#include <chrono>
#include "BMPgenfuncs.hpp"
using namespace std;
int main(void) {
    unsigned int Screen_x = 1920;
    unsigned int Screen_y = 1080;

    Vert a(vec3(1.0f, 0, -1.0f), vec2(0, 0));
    Vert b(vec3(-1.0f, 0, -1.0f), vec2(1, 0));
    Vert c(vec3(0, 1.0f, -1.0f), vec2(0, 1));
    texture2d pix(0xff0000ff);
    //pix.loadTexturePNG("Instagram_icon.png");
    Mesh mesh;
    mesh.material.texture = pix;

    // Add triangles to the mesh
    mesh.loadOBJ("xyzrgb_dragon.obj");
    projectVBO(mesh.VBO, mesh.VBO, etm4(vec3(0, -25, 0)));
    // Create a camera looking at the origin from z = 5
    Camera camera1(vec3(0, 0, 800), quaternion(0, 0, 0, 1), 150.0f, 1.0f, 1000.0f);
    Camera cam0(vec3(0, 0, 0), quaternion(0, 0, 0, 1), 2, 2);
    Camera cam2(vec3(0, 100, 150), quaternion(0, 0, 0, 1), 150, 300);
    mesh.fillUV();
    mesh.computeNormals();
    
    Mesh small_tri;
    small_tri.material.texture = texture2d(0xffffffff);

    // Create framebuffer and render
    fbo screenframe(Screen_x, Screen_y, 4, MSAA4xRotated);
    fbo MSAATEST(Screen_x, Screen_y, 16, MSAA16xRotated);


 
    screenframe.drawMeshThreaded(mesh, cam2, 256);
    MSAATEST.drawMeshesThreaded(small_tri, cam0, 256);

    // Write framebuffer to BMP
    FBOtBMP(screenframe.frame, "test.bmp", Screen_x, Screen_y);
    FBOtBMP(MSAATEST.frame, "MSAAtest.bmp", Screen_x, Screen_y);
    ZBtBMP(screenframe.Zframe, "zbtest.bmp", Screen_x, Screen_y);

    return 0;
}