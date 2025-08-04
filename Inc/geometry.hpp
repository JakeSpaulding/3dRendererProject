#pragma once
// contains the data type for actually making geometry  
#include <vector>  
#include "matlib.hpp"  
#include "VertSettings.hpp"  
#include "Materials.hpp"

struct Vert {  
    vec3 pos;  
    vec2 UV;
    vec3 N;  

#ifdef USING_PVC
    // adding color to the verts  
    union {  
        float color[4];  
        struct { float r, g, b, a; };
    };  

    Vert(vec3 const& p, const float R = 1, const float G = 1, const float B = 1) : pos(vec3(p[0], p[1], p[2])), color{ R,G,B }, N(vec3(0,0,0)){}  
    Vert(vec3 const& p, const float c[3]) : pos(vec3(p[0], p[1], p[2])), color{ c[0],c[1],c[2] } {}  
    bool operator==(Vert const& vert2) const {  
        return (pos == vert2.pos && vert2.r == r && vert2.g == g && vert2.b == b && a == vert2.a && vert2.UV == UV);
    }  
#endif
#ifndef USING_PVC
    bool operator==(Vert const& vert2) const {
        return (pos == vert2.pos && vert2.UV == UV);
    }
#endif
#ifdef USING_UV  


    // default constructor  

    Vert(vec3 p, vec2 tx, vec3 No = vec3(0,0,0)) : pos(vec3(p[0], p[1], p[2])), UV(vec2(tx[0], tx[1])), N(No) {}
#endif  

};  

// takes in an array of points and transforms them
struct Mesh {  
    std::vector <unsigned int> VEBO;
    std::vector <vec3> VBO;
    std::vector <vec2> UV;
    std::vector <unsigned int> UVEBO;
    std::vector <vec3> Normals;
    std::vector <unsigned int> NEBO;
    Material material; // material data

    // computes normals for the normal buffer
    void computeNormals();

    Mesh() {}

    void loadOBJ(const char* filename); // loads an obj 3d model

    // maps all uv to 0 0 if there are no coordinates
    void fillUV();

    // loads VBO data
    void loadVBO(std::vector <float> V) {
        VBO.resize(V.size());
        std::copy(V.data(), V.data() + V.size(), VBO.data());
    }
    // adds the vertex to the buffers  
    void addVert(Vert const& v, bool takingNormal = 0);

    // takes vertex data to add a new triangle  
    void newTri(Vert const& a, Vert const& b, Vert const& c) {  
        addVert(a);  
        addVert(b);  
        addVert(c);  
    }  
    // takes point and color data to add a new triangle  
    void newTri(vec3 const& ap, vec2 const& ac, vec3 const& bp, vec2 const& bc, vec3 const& cp, vec2 const& cc) {  
        addVert(Vert(ap,ac));  
        addVert(Vert(bp,bc));  
        addVert(Vert(cp,cc));  
    }  
};  
