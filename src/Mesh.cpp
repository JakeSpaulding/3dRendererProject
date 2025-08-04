#include "geometry.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// adds a norm vec3 to the norm buffer
void Mesh::computeNormals() {
	// store all of the normals (inefficiently) in an array which corresponds to each point
	Normals.resize(VBO.size());
	// set the Normal vectors all to 0
	for (size_t i = 0; i < Normals.size(); i++) Normals[i] = vec3(0, 0, 0);  // Container iteration - use size_t
	// loop through all tris
	for (size_t i = 0; i < VEBO.size(); i += 3) {  // Container iteration - use size_t
		// vectors to store our vert positions
		vec3 a = VBO[VEBO[i]];
		vec3 b = VBO[VEBO[i + 1]];
		vec3 c = VBO[VEBO[i + 2]];

		// compute normal
		vec3 norm(faceNormal(a, b, c));
		// add the normals to each vert
		for (unsigned int j = 0; j < 3; j++) Normals[VEBO[i + j]] = Normals[VEBO[i + j]] + norm;  // Graphics loop - unsigned int
	}
	// loop through all verts and normalize
	for (size_t i = 0; i < Normals.size(); i++) Normals[i].normalize();  // Container iteration - use size_t
	// update the normal indices
	NEBO.resize(VEBO.size());
	for (size_t i = 0; i < NEBO.size(); i++) NEBO[i] = VEBO[i];  // Container iteration - use size_t
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
		// tri acd
		Fout.push_back(Fin[0] - 1);
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
	std::vector<float> va; // stores the optional v-attributes
	
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
			std::vector<unsigned int> facePOS; // store the face positions
			std::vector<unsigned int> faceUV; // store the face UVs
			std::vector<unsigned int> faceN; // store the face Normals
			std::string id; // will store the current id

			// loop through all ids on the row
			while (iss >> id) {
				std::vector<unsigned int> vertmp; // temporarily stores all of the indexes for the vertex attributes of one of the vertices
				std::replace(id.begin(), id.end(), '/', ' '); // replace all slashes with spaces
				std::istringstream viss(id); // turn the current vert into a stream
				std::string tmp;
				while (viss >> tmp) {
					vertmp.push_back(static_cast<unsigned int>(std::stoi(tmp))); // push back the current attribute index - cast to unsigned int
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
			if (faceUV.size()) {
				loadFaceBuffer(faceUV, UVEBO);  // Fixed: was using facePOS instead of faceUV
			}
			// if there is normal data load it
			if (faceN.size()) {  // Fixed: was checking faceUV.size() instead of faceN.size()
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
	bool newpos = true;
	bool newuv = true;
	// check if the position is already there
	for (size_t i = 0; i < VBO.size(); i++) {  // Container iteration - use size_t
		if (VBO[i] == v.pos) {
			newpos = false;
			// just add a pointer to the pre-existing VEBO spot
			VEBO.push_back(static_cast<unsigned int>(i));  // Cast to unsigned int for graphics index
		}
	}
	// if it is new add it to the VBO
	if (newpos) {
		VEBO.push_back(static_cast<unsigned int>(VBO.size()));  // Cast to unsigned int for graphics index
		VBO.push_back(v.pos);
	}
	// check if the UV is already there
	for (size_t i = 0; i < UV.size(); i++) {  // Container iteration - use size_t
		if (UV[i] == v.UV) {
			newuv = false;
			// just add a pointer to the pre-existing UVEBO spot
			UVEBO.push_back(static_cast<unsigned int>(i));  // Cast to unsigned int for graphics index
		}
	}
	// if it is new add it to the UVs
	if (newuv) {
		UVEBO.push_back(static_cast<unsigned int>(UV.size()));  // Cast to unsigned int for graphics index
		UV.push_back(v.UV);
	}
	// if we are taking normals
	if (takingNormal) {
		bool newn = true;
		// check if the normal is already there
		for (size_t i = 0; i < Normals.size(); i++) {  // Container iteration - use size_t
			if (Normals[i] == v.N) {
				newn = false;
				// just add a pointer to the pre-existing NEBO spot
				NEBO.push_back(static_cast<unsigned int>(i));  // Cast to unsigned int for graphics index
			}
		}
		// if it is new add it to the normals
		if (newn) {
			NEBO.push_back(static_cast<unsigned int>(Normals.size()));  // Cast to unsigned int for graphics index
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