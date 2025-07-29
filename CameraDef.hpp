#pragma once

// defines the camera structs including orthographic and perspective
#include "quaternion.hpp"
#include "matlib.hpp"

// orthographic cam
struct CameraO {
	vec3 pos;
	quaternion rot;
	float aspect;
	float scale; // how big the viewbox is
	float zfar; // how far the render distance is
	mat4 projMat;

	// updates the projection matrix with the new data
	void updateMat(void) {
		projMat = orthoSpaceTransMat(scale, zfar, aspect) * vulkanAxisRotate() * qtm4(rot.conj()) * translation4(-1 * pos);
	}

	// creates a camera at position p with rotation q and generates the projection matrix
	CameraO(vec3 p = vec3(0, 0, 0), quaternion r = quaternion(0, 0, 0, 1), float s = 1, float f = 100, float a = 16 / 9) : pos(vec3(p[0],p[1],p[2])), rot(r), zfar(f), aspect(a), scale(s) {
		// constuct the matricies
		updateMat();
	}
	void setRotQ(quaternion const& q) {
		rot = q;
		updateMat();
	}
	// allows the camera to be set to a new position
	void setPos(vec3 const& np) {
		// move the position
		pos = np;
		updateMat();
	}
	// update the aspect ratio and matrix
	void setAspectRatio(const float a) {
		aspect = a;
		updateMat();
	}

	// update the render distance
	void setZFar(const float f) {
		zfar = f;
		updateMat();
	}
	// let you just use radians in the fov
	void setScale(const float f) {
		scale = f;
		updateMat();
	}
};

// perspective cam
struct CameraP {
	vec3 pos;
	quaternion rot;
	float fov;
	float aspect;
	float znear;
	float zfar;
	mat4 projMat;

	// updates the projection matrix with the new data
	void updateMat(void) {
		projMat = perspectiveProj(fov, znear, zfar, aspect) * vulkanAxisRotate() * qtm4(rot.conj()) * translation4(-1 * pos);
	}

	// creates a camera at position p with rotation q and generates the projection matrix
	CameraP(vec3 p = vec3(0, 0, 0), quaternion r = quaternion(0, 0, 0, 1), float ang = 100, float n = 1, float f = 100, float a = 16 / 9) : pos(vec3(p[0], p[1], p[2])), znear(n), rot(r), zfar(f), aspect(a), fov(ang) {
		// constuct the matricies
		updateMat();
	}

	// allows for updating the rotation with a quaternion
	void setRotQ(quaternion const& q) {
		rot = q;
		updateMat();
	}
	// allows the camera to be set to a new position
	void setPos(vec3 const& np) {
		// move the position
		pos = np;
		updateMat();
	}
	// update the aspect ratio and matrix
	void setAspectRatio(const float a) {
		aspect = a;
		updateMat();
	}
	// update the fov ( ind degrees
	void setFOV(const float f) {
		fov = f;
		updateMat();
	}
	// update the render distance
	void setZFar(const float f) {
		zfar = f;
		updateMat();
	}
	// let you just use radians in the fov
	void setFovRad(const float f) {
		fov = f;
		updateMat();
	}
};