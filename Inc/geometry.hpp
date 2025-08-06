#pragma once
// contains mesh and vertex structs
#include <vector>  
#include <string>
#include "matlib.hpp"  
#include "Materials.hpp"

struct Vert {  
    vec3 pos;  
    vec2 UV;
    vec3 N;  

    bool operator==(Vert const& vert2) const {
        return (pos == vert2.pos && vert2.UV == UV);
    }

    // default constructor  

    Vert(vec3 p, vec2 tx, vec3 No = vec3(0,0,0)) : pos(vec3(p[0], p[1], p[2])), UV(vec2(tx[0], tx[1])), N(No) {}

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
    vec3 bboxMin; // stores the min x y and z
    vec3 bboxMax; // stores the max x y and z
    vec3 bboxCenter; // stores the center position of the bbox
    
    void computeBBOX(); // computes the bbox

    // computes normals for the normal buffer
    void computeNormals();

    Mesh() {}

    void loadOBJ(string filename); // loads an obj 3d model

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
