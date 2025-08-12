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
    unsigned int Screen_x = 15360;
    unsigned int Screen_y = 8640;

    Vert a(vec3(1.0f, 0, -1.0f), vec2(0, 0));
    Vert b(vec3(-1.0f, 0, -1.0f), vec2(1, 0));
    Vert c(vec3(0, 1.0f, -1.0f), vec2(0, 1));
    texture2d pix;
    pix.loadTexturePNG("C:\\Users\\jakem\\source\\repos\\RasterEngine2\\resources\\instagram_icon.png");
    Mesh mesh;
    mesh.material.texture = pix;

    // Add triangles to the mesh
    mesh.loadOBJ("C:\\Users\\jakem\\source\\repos\\RasterEngine2\\resources\\beast.obj");
   
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
    vector<pointLight> lights;
    pointLight sampleLight;
    sampleLight.pos = vec3(0.0f, 10.0f, -100.0f); // Position above and in front of the origin
    sampleLight.radius = 1000.0f; // Maximum range of the light

    Mesh dragon;
    dragon.loadOBJ("C:\\Users\\jakem\\source\\repos\\RasterEngine2\\resources\\xyzrgb_dragon.obj");
    dragon.material.setTexture(texture2d(0xFF8080ff));
    Camera dragonCam(vec3(0, 0, 100), quaternion(0, 0, 0, 1), 100.0f, 1.0f, 1000.0f);
    dragon.computeBBOX();
    //dragon.computeNormals();
    dragon.fillUV();
    dragon.material.setIllum(2);

    // Ambient, diffuse, and specular colors (white light)
    sampleLight.La = vec3(0.3f, 0.3f, 0.3f); // Ambient
    sampleLight.Ld = vec3(0.8f, 0.8f, 0.8f); // Diffuse
    sampleLight.Ls = vec3(1.0f, 1.0f, 1.0f); // Specular
    // Create test meshes with different alpha and color
    lights.push_back(sampleLight);

    Mesh meshOpaque = makeTestMesh(0xff0000ff, -1.0f);   // Opaque blue
    Mesh meshSemi = makeTestMesh(0x80ff0080, -0.95f);    // Semi-transparent green
    Mesh meshTrans = makeTestMesh(0x40ffffff, -0.9f);    // More transparent white
    meshOpaque.material.illum = 0;
    mesh.material.illum = 2;
    meshSemi.material.illum = 0;
    meshTrans.material.illum = 0;
    // Create framebuffer and render
    fbo screenframe(Screen_x, Screen_y, MSAA16xRotated);
    fbo MSAATEST(Screen_x, Screen_y, MSAA16xRotated);

    // Render all test meshes
    vector<Mesh> msaaMeshes = {meshOpaque, meshSemi, meshTrans };
    
    fbo dragonframe(Screen_x, Screen_y, MSAA4xRotated);

    dragonframe.drawMeshThreaded(dragon, dragonCam, lights, 2048);
    screenframe.drawMeshThreaded(mesh, cam2, lights, 2048);
    MSAATEST.drawMeshesThreaded(msaaMeshes, cam0, lights, 2048);

    // Write framebuffer to BMP
    FBOtBMP(screenframe.frame, "C:\\Users\\jakem\\source\\repos\\RasterEngine2\\resources\\test.bmp", Screen_x, Screen_y);
    FBOtBMP(screenframe.buffer, "C:\\Users\\jakem\\source\\repos\\RasterEngine2\\resources\\testbuffer.bmp", Screen_x * screenframe.samples, Screen_y);

    FBOtBMP(MSAATEST.frame, "C:\\Users\\jakem\\source\\repos\\RasterEngine2\\resources\\MSAAtest.bmp", Screen_x, Screen_y);
    FBOtBMP(MSAATEST.buffer, "C:\\Users\\jakem\\source\\repos\\RasterEngine2\\resources\\MSAAtestbuffer.bmp", Screen_x * MSAATEST.samples, Screen_y);

    ZBtBMP(screenframe.Zframe, "C:\\Users\\jakem\\source\\repos\\RasterEngine2\\resources\\zbtest.bmp", Screen_x, Screen_y);
    ZBtBMP(screenframe.Zbuffer, "C:\\Users\\jakem\\source\\repos\\RasterEngine2\\resources\\zbtestbuffer.bmp", Screen_x * screenframe.samples, Screen_y);

    ZBtBMP(MSAATEST.Zframe, "C:\\Users\\jakem\\source\\repos\\RasterEngine2\\resources\\zbtest2.bmp", Screen_x, Screen_y);
    ZBtBMP(MSAATEST.Zbuffer, "C:\\Users\\jakem\\source\\repos\\RasterEngine2\\resources\\zbtest2buffer.bmp", Screen_x * MSAATEST.samples, Screen_y);

    FBOtBMP(dragonframe.frame, "C:\\Users\\jakem\\source\\repos\\RasterEngine2\\resources\\dragon.bmp", Screen_x, Screen_y);
    FBOtBMP(dragonframe.buffer, "C:\\Users\\jakem\\source\\repos\\RasterEngine2\\resources\\dragonbuffer.bmp", Screen_x * dragonframe.samples, Screen_y);


    ZBtBMP(dragonframe.Zframe, "C:\\Users\\jakem\\source\\repos\\RasterEngine2\\resources\\dragonZB.bmp", Screen_x, Screen_y);
    ZBtBMP(dragonframe.Zbuffer, "C:\\Users\\jakem\\source\\repos\\RasterEngine2\\resources\\dragonZB.bmp", Screen_x * dragonframe.samples, Screen_y);

    return 0;
}