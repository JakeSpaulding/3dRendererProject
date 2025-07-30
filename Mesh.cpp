#include "geometry.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// split an n-gon using fan method

// adds a vertex to the mesh (if it is not already there)
void Mesh::addVert(Vert const& v) {
	bool found = 0;
	for (int i = 0; i < VB.size() && !found; i++) {
		if (v == VB[i]) {
			EB.push_back(i);
			found = 1;
		}
	}
	// add the vertex to the arrays if not found  
	if (!found) {
		EB.push_back(VB.size());
		VB.push_back(v);
	}
}

// computes and sets the normals for the verticies in a mesh
void Mesh::computeNormals() {
	// loop through all tris
	for (int i = 0; i < EB.size(); i += 3) {
		// create addresses for our verts
		Vert& a = VB[i];
		Vert& b = VB[i + 1];
		Vert& c = VB[i + 2];
		// compute normal
		vec3 norm(faceNormal(a.pos, b.pos, c.pos));
		// add the normals to each vert
		a.N = a.N + norm;
		b.N = b.N + norm;
		c.N = c.N + norm;
	}
	// loop through all verts and normalize
	for (int i = 0; i < VB.size(); i++) VB[i].N.normalize();
}

// takes an obj file and loads the vertex data
void Mesh::loadOBJ(const char* filename) {
	// open the file
	std::ifstream file(filename);
	if (!file) throw std::runtime_error("Cannot open file");

	std::string line; // will store each line of the file
	std::vector<vec3> vp; // store all of the positions
	std::vector<int> ids; // store all of the iindexes for the tris
	std::vector <vec2> vt; // store the texture data
	std::vector <vec3> vn; // stores normals
	std::vector<float>va; // stores the optional v-attribues
	
	while (std::getline(file, line)){
		std::istringstream iss(line); // treat the line like a stream
		std::string prefix; // store the prefix
		iss >> prefix;
		
		// check if it's positions
		if (prefix == "v") {
			float x, y, z;
			iss >> x >> y >> z;
			vp.push_back(vec3(x, y, z));
		}
		// if it's texture coords
		else if (prefix == "vt") {
			float u, v;
			iss >> u >> v;
			vt.push_back(vec2(u, v));
		}
		// if it's normals
		else if (prefix == "vn") {
			float nx, ny, nz;
			iss >> nx >> ny >> nz;
			vn.push_back(vec3(nx, ny, nz));
		}
		// if it's face data
		else if (prefix == "f") {
			// fill the face up
			std::vector<int> face; // store the face
			std::string id; // will store the current id

			// loop through all ids on the row
			while (iss >> id) {

			}

			// if it's a triangle just add it to the ids
			if (face.size() == 3) {
				for (float v : face) ids.push_back(v);
			}

			// if it's a quad
			if (face.size() == 4) {
				// push back abc
				ids.push_back(face[0]);
				ids.push_back(face[1]);
				ids.push_back(face[2]);

				// push back bcd
				ids.push_back(face[1]);
				ids.push_back(face[2]);
				ids.push_back(face[3]);
			}
		}
}
// projects the data in a VBO to screenspace and puts it into VBOout
void projectVBO(VBO& VBOout, VBO const& VBOin, mat4 projMat, const float w, const float h) {
	VBOout.clear(); // clear the output
	VBOout.reserve(VBOin.size()); // reserve the propper size

	for (int i = 0; i < VBOin.size(); i++) {
		vec3 pos = vec3(projMat * VBOin[i].pos); // project it

		// w = screen width, h = screen height
		pos[0] = ((pos[0] + 1.0f) * 0.5f) * w; // convert to pixel centered coordinates
		pos[1] = ((pos[1] + 1.0f) * 0.5f) * h;
		std::cout << "\n" << pos << "\n";
#ifdef USING_COLOR
		VBOout.push_back(Vert(vec3(pos), VBOin[i].color)); // add it to the new buffer
#endif
#ifdef USING_UV
		VBOout.push_back(Vert(vec3(pos), vec2(VBOin[i].UV)));
#endif
	}
}
