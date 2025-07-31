#include "geometry.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// split an n-gon using fan method


// computes and sets the normals for the verticies in a mesh (depricated)
void Mesh::computeNormalsVertsBuffer() {
	// loop through all tris
	for (int i = 0; i < VertEBO.size(); i += 3) {
		// create addresses for our verts
		Vert& a = Verts[i];
		Vert& b = Verts[i + 1];
		Vert& c = Verts[i + 2];
		// compute normal
		vec3 norm(faceNormal(a.pos, b.pos, c.pos));
		// add the normals to each vert
		a.N = a.N + norm;
		b.N = b.N + norm;
		c.N = c.N + norm;
	}
	// loop through all verts and normalize
	for (int i = 0; i < Verts.size(); i++) Verts[i].N.normalize();
}

// adds a norm vec3 to the norm buffer out
void Mesh::computeNormals() {
	// store all of the normals (inefficiently) in an array which corresponds to each point
	Normals.resize(VBO.size());
	// set the Normal vectors all to 0
	for (int i = 0; i < Normals.size(); i++) Normals[i] = vec3(0, 0, 0);
	// loop through all tris
	for (int i = 0; i < VEBO.size(); i += 3) {
		// vectors to store our vert positions
		vec3 a = VBO[VEBO[i]];
		vec3 b = VBO[VEBO[i + 1]];
		vec3 c = VBO[VEBO[i + 2]];

		// compute normal
		vec3 norm(faceNormal(a, b, c));
		// add the normals to each vert
		for (int j = 0; j < 3; j++) Normals[VEBO[i + j]] = Normals[VEBO[i + j]] + norm;
	}
	// loop through all verts and normalize
	for (int i = 0; i < Normals.size(); i++) Normals[i].normalize();
	// update the normal indicies
	NEBO.resize(VEBO.size());
	for (int i = 0; i < NEBO.size(); i++) NEBO[i] = VEBO[i];
}

// pushes the face data into the buffer
inline void loadFaceBuffer(std::vector<unsigned int> const& Fin, std::vector<unsigned int>& Fout) {
	// if it's a tri just load it
	if (Fin.size() == 3) {
		for (unsigned int v : Fin) Fout.push_back(v - 1);
	}

	// if it's a quad load tri abc and tri bcd
	if (Fin.size() == 4) {
		// tri abc
		Fout.push_back(Fin[0] - 1);
		Fout.push_back(Fin[1] - 1);
		Fout.push_back(Fin[2] - 1);
		// tri bcd
		Fout.push_back(Fin[1] - 1);
		Fout.push_back(Fin[2] - 1);
		Fout.push_back(Fin[3] - 1);
	}
}

// takes an obj file and loads the vertex data
void Mesh::loadOBJ(const char* filename) {
	// open the file
	std::ifstream file(filename);
	if (!file) throw std::runtime_error("Cannot open file");

	std::string line; // will store each line of the file
	std::vector<float>va; // stores the optional v-attribues
	
	while (std::getline(file, line)) {
		std::istringstream iss(line); // treat the line like a stream
		std::string prefix; // store the prefix
		iss >> prefix;

		// check if it's positions
		if (prefix == "v") {
			float x, y, z;
			iss >> x >> y >> z;
			VBO.push_back(vec3(x, y, z));
		}
		// if it's texture coords
		else if (prefix == "vt") {
			float u, v;
			iss >> u >> v;
			UV.push_back(vec2(u, v));
		}
		// if it's normals
		else if (prefix == "vn") {
			float nx, ny, nz;
			iss >> nx >> ny >> nz;
			Normals.push_back(vec3(nx, ny, nz));
		}
		// if it's face data
		else if (prefix == "f") {
			// fill the face up
			std::vector<unsigned int> facePOS; // store the face UVs
			std::vector<unsigned int> faceUV; // store the face UVs
			std::vector<unsigned int> faceN; // store the face Normals
			std::string id; // will store the current id
			

			// loop through all ids on the row
			while (iss >> id) {
				std::vector<unsigned int> vertmp; // temporarlily stores all of the indexes for the vertex attributes of one of the verticies
				std::replace(id.begin(), id.end(), '/', ' '); // replace all slashes with spaces
				std::istringstream viss(id); // turn the current vert into a stream
				std::string tmp;
				while (viss >> tmp) {
					vertmp.push_back(std::stoi(tmp)); // push back the current attribute index
				}
				facePOS.push_back(vertmp[0]);
				// if there is uv data add it
				if (vertmp.size() > 1) {
					faceUV.push_back(vertmp[1]);
				}
				// add normals if they are there
				if (vertmp.size() > 2) {
					faceN.push_back(vertmp[2]);
				}
			}
			// load the Pos buffer
			loadFaceBuffer(facePOS, VEBO);
			// load uv buffer if there is data to load
			if(faceUV.size()){
				loadFaceBuffer(facePOS, UVEBO);
			}
			// if there is normal data load it
			if (faceUV.size()) {
				loadFaceBuffer(faceN, NEBO);
			}
		}
	}
	file.close();
	// compute normals if there are none
	if (NEBO.size() == 0) {
		computeNormals();
	}
}

// adds a vertex to the buffers (will not take normal data)
void Mesh::addVert(Vert const& v, bool takingNormal) {
	bool newpos = 1;
	bool newuv = 1;
	// check if the position is already there
	for (int i = 0; i < VBO.size(); i++) {
		if (VBO[i] == v.pos) {
			newpos = 0;
			// just add a pointer to the pre-existing VEBO spot
			VEBO.push_back(i);
		}
	}
	// if it is new add it to the VBO
	if (newpos) {
		VEBO.push_back(VBO.size());
		VBO.push_back(v.pos);
	}
	// check if the position is already there
	for (int i = 0; i < UV.size(); i++) {
		if (UV[i] == v.UV) {
			newuv = 0;
			// just add a pointer to the pre-existing VEBO spot
			UVEBO.push_back(i);
		}
	}
	// if it is new add it to the VBO
	if (newuv) {
		UVEBO.push_back(UV.size());
		UV.push_back(v.UV);
	}
	// if we are taking normals
	if (takingNormal) {
		bool newn = 1;
		// check if the position is already there
		for (int i = 0; i < Normals.size(); i++) {
			if (Normals[i] == v.N) {
				newn = 0;
				// just add a pointer to the pre-existing VEBO spot
				NEBO.push_back(i);
			}
		}
		// if it is new add it to the VBO
		if (newn) {
			NEBO.push_back(Normals.size());
			Normals.push_back(v.N);
		}
	}
}

void Mesh::fillUV() {
	if (UV.size() == 0) {
		UV.resize(1);
		UV.push_back(vec2(0, 0));
		UVEBO.resize(VEBO.size());
		std::fill(UVEBO.data(), UVEBO.data() + UVEBO.size(), 0);
	}
}

/* depricated, works with the old vert buffer style
// adds a vertex to the mesh (if it is not already there)
void Mesh::addVert(Vert const& v) {
	bool found = 0;
	for (int i = 0; i < VertEBO.size() && !found; i++) {
		if (v == Verts[i]) {
			VertEBO.push_back(i);
			found = 1;
		}
	}
	// add the vertex to the arrays if not found  
	if (!found) {
		VertEBO.push_back(Verts.size());
		Verts.push_back(v);
	}
}
*/