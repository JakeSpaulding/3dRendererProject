#ifndef GEOMETRY_HPP  
#define GEOMETRY_HPP  

// contains the data type for actually making geometry  
#include <vector>  
#include "matlib.hpp"  
#include "VertSettings.hpp"  
#include "Materials.hpp"



struct Vert {  
    vec3 pos;  
    vec3 N;  

    // adding color to the verts  
    union {  
        float color[4];  
        struct { float r, g, b, a; };
    };  

    Vert(vec3 const& p, const float R = 1, const float G = 1, const float B = 1) : pos(vec3(p[0], p[1], p[2])), color{ R,G,B }, N(vec3(0,0,0)){}  
    Vert(vec3 const& p, const float c[3]) : pos(vec3(p[0], p[1], p[2])), color{ c[0],c[1],c[2] } {}  
    bool operator==(Vert const& vert2) const {  
        return (pos == vert2.pos && vert2.r == r && vert2.g == g && vert2.b == b && a == vert2.a, vert2.UV == UV);
    }  
#ifdef USING_UV  

    vec2 UV;  
    // default constructor  

    Vert(vec3 p, vec2 tx) : pos(vec3(p[0], p[1], p[2])), UV(vec2(tx[0], tx[1])), N(vec3(0, 0, 0)), color{ 0,0,0,0 } {}
    // compare values  
    bool operator==(Vert const& vert2) const {  
        return (pos == vert2.pos && vert2.UV == UV);  
    }  
#endif  

};  
// define a vertex buffer  
typedef std::vector<Vert> VBO;  
// define an index array  
typedef std::vector<uint32_t> EBO;  
void projectVBO(VBO& VBOout, VBO const& VBOin, mat4 projMat, const float w, const float h);
struct Mesh {  
    VBO VB; // vertex buffer  
    EBO EB; // index buffer  
    Material material; // material data
    void computeNormals();

    Mesh(VBO v, EBO id) : VB(v), EB(id) { computeNormals(); }
    Mesh() {}

    void loadOBJ(const char* filename); // loads an obj 3d model

    // adds the vertex to the buffers  
    void addVert(Vert const& v);

    // takes vertex data to add a new triangle  
    void newTri(Vert const& a, Vert const& b, Vert const& c) {  
        addVert(a);  
        addVert(b);  
        addVert(c);  
    }  
    // takes point and color data to add a new triangle  
    void newTri(vec3 const& ap, const uint32_t ac, vec3 const& bp, const uint32_t bc, vec3 const& cp, const uint32_t cc) {  
        addVert(Vert(ap,ac));  
        addVert(Vert(bp,bc));  
        addVert(Vert(cp,cc));  
    }  
};  

#endif