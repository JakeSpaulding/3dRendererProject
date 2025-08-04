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
    unsigned int Screen_x = 3840;
    unsigned int Screen_y = 2160;

    Vert a(vec3(1.0f, 0, -1.0f), vec2(0, 0));
    Vert b(vec3(-1.0f, 0, -1.0f), vec2(1, 0));
    Vert c(vec3(0, 1.0f, -1.0f), vec2(0, 1));
    texture2d pix;
    pix.loadTexturePNG("C:\\Users\\jakem\\source\\repos\\RasterEngine2\\resources\\Instagram_icon.png");
    Mesh mesh;
    mesh.material.texture = pix;

    // Add triangles to the mesh
    mesh.loadOBJ("C:\\Users\\jakem\\source\\repos\\RasterEngine2\\resources\\beast.obj");
   
    // Create a camera looking at the origin from z = 5
    Camera camera1(vec3(0, 0, 800), quaternion(0, 0, 0, 1), 150.0f, 1.0f, 1000.0f);
    Camera cam0(vec3(0, 0, 0), quaternion(0, 0, 0, 1), 2, 2);
    Camera cam2(vec3(0, 100, 150), quaternion(0, 0, 0, 1), 150, 300);
    //mesh.fillUV();
    mesh.computeNormals();
    
    Mesh small_tri;
    small_tri.material.texture = texture2d(0xffffffff);

    // Helper: Create a mesh with three triangles and a given texture
    auto makeTestMesh = [](uint32_t texColor, float zOffset) -> Mesh {
        Mesh m;
        m.material.texture = texture2d(texColor);

        // Triangle 1
        m.newTri(
            vec3(-0.7f, -0.7f, zOffset), vec2(0, 0),
            vec3(0.7f, -0.7f, zOffset), vec2(1, 0),
            vec3(0.0f, 0.7f, zOffset), vec2(0.5f, 1)
        );
        // Triangle 2 (overlapping, different orientation)
        m.newTri(
            vec3(-0.5f, 0.0f, zOffset + 0.05f), vec2(0, 0),
            vec3(0.5f, 0.0f, zOffset + 0.05f), vec2(1, 0),
            vec3(0.0f, 0.9f, zOffset + 0.05f), vec2(0.5f, 1)
        );
        // Triangle 3 (smaller, more transparent)
        m.newTri(
            vec3(-0.2f, -0.2f, zOffset + 0.1f), vec2(0, 0),
            vec3(0.2f, -0.2f, zOffset + 0.1f), vec2(1, 0),
            vec3(0.0f, 0.2f, zOffset + 0.1f), vec2(0.5f, 1)
        );
        m.fillUV();
        m.computeNormals();
        m.computeBBOX();
        return m;
    };

    // Create test meshes with different alpha and color
    Mesh meshOpaque = makeTestMesh(0xff0000ff, -1.0f);   // Opaque blue
    Mesh meshSemi = makeTestMesh(0x80ff0080, -0.95f);    // Semi-transparent green
    Mesh meshTrans = makeTestMesh(0x40ffffff, -0.9f);    // More transparent white

    // Create framebuffer and render
    fbo screenframe(Screen_x, Screen_y);
    fbo MSAATEST(Screen_x, Screen_y, 16, MSAA16xRotated);

    // Render all test meshes
    vector<Mesh> msaaMeshes = {meshOpaque, meshSemi, meshTrans };
    

    screenframe.drawMeshThreaded(mesh, cam2, 256);
    MSAATEST.drawMeshesThreaded(msaaMeshes, cam0, 256);

    // Write framebuffer to BMP
    FBOtBMP(screenframe.frame, "C:\\Users\\jakem\\source\\repos\\RasterEngine2\\resources\\test.bmp", Screen_x, Screen_y);
    FBOtBMP(MSAATEST.frame, "C:\\Users\\jakem\\source\\repos\\RasterEngine2\\resources\\MSAAtest.bmp", Screen_x, Screen_y);
    ZBtBMP(screenframe.Zframe, "C:\\Users\\jakem\\source\\repos\\RasterEngine2\\resources\\zbtest.bmp", Screen_x, Screen_y);
    ZBtBMP(MSAATEST.Zframe, "C:\\Users\\jakem\\source\\repos\\RasterEngine2\\resources\\zbtest2.bmp", Screen_x, Screen_y);
    return 0;
}