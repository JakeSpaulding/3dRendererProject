#pragma once

// defines the unified camera struct for both orthographic and perspective projection
#include "quaternion.hpp"
#include "matlib.hpp"

enum class ProjectionType {
    PERSPECTIVE,
    ORTHOGRAPHIC
};

struct Camera {
    vec3 pos;
    quaternion rot;
    ProjectionType projection_type;
    
    // Perspective-specific parameters
    float fov;      // Field of view (perspective only)
    float znear;    // Near plane (perspective only)
    
    // Orthographic-specific parameters  
    float scale;    // Orthographic scale (orthographic only)
    
    // Common parameters
    float zfar;     // Far plane (both)

    // Perspective camera constructor
    Camera(vec3 p, quaternion r, float ang, float n, float f)
        : pos(p), rot(r), projection_type(ProjectionType::PERSPECTIVE), 
          fov(ang), znear(n), scale(1.0f), zfar(f) {}
    
    // Orthographic camera constructor  
    Camera(vec3 p, quaternion r, float s, float f)
        : pos(p), rot(r), projection_type(ProjectionType::ORTHOGRAPHIC),
          fov(90.0f), znear(1.0f), scale(s), zfar(f) {}

    // Default constructor (perspective)
    Camera(vec3 p = vec3(0, 0, 0), quaternion r = quaternion(0, 0, 0, 1))
        : pos(p), rot(r), projection_type(ProjectionType::PERSPECTIVE),
          fov(90.0f), znear(1.0f), scale(1.0f), zfar(100.0f) {}

    // Generate projection matrix with provided aspect ratio
    mat4 getProjectionMatrix(float aspectRatio) const {
        mat4 projMatrix;
        
        if (projection_type == ProjectionType::PERSPECTIVE) {
            projMatrix = perspectiveProj(fov, znear, zfar, aspectRatio);
        } else {
            projMatrix = orthoSpaceTransMat(scale, zfar, aspectRatio);
        }
        
        return projMatrix * vulkanAxisRotate() * qtm4(rot.conj()) * translation4(-1 * pos);
    }
    
    // Generate projection matrix with screen dimensions (for NDC scaling)
    mat4 getProjectionMatrix() const {
        mat4 projMatrix;
        
        if (projection_type == ProjectionType::PERSPECTIVE) {
            projMatrix = perspectiveProj(fov, znear, zfar);
        } else {
            projMatrix = orthoSpaceTransMat(scale, zfar);
        }
        
        return projMatrix * vulkanAxisRotate() * qtm4(rot.conj()) * translation4(-1 * pos);
    }

    // Common setters
    void setRotQ(quaternion const& q) { rot = q; }
    void setPos(vec3 const& np) { pos = np; }
    void setZFar(const float f) { zfar = f; }
    
    // Perspective-specific setters
    void setFOV(const float f) { 
        if (projection_type == ProjectionType::PERSPECTIVE) {
            fov = f; 
        }
    }
    void setZNear(const float n) { 
        if (projection_type == ProjectionType::PERSPECTIVE) {
            znear = n; 
        }
    }
    void setFovDeg(const float f) { setFOV(f); }
    
    // Orthographic-specific setters
    void setScale(const float s) { 
        if (projection_type == ProjectionType::ORTHOGRAPHIC) {
            scale = s; 
        }
    }
    
    // Utility functions
    bool isPerspective() const { return projection_type == ProjectionType::PERSPECTIVE; }
    bool isOrthographic() const { return projection_type == ProjectionType::ORTHOGRAPHIC; }
    
    // Switch projection type (preserves common parameters)
    void switchToPerspective(float ang = 90.0f, float n = 1.0f) {
        projection_type = ProjectionType::PERSPECTIVE;
        fov = ang;
        znear = n;
    }
    
    void switchToOrthographic(float s = 1.0f) {
        projection_type = ProjectionType::ORTHOGRAPHIC;
        scale = s;
    }
};

// Convenience factory functions for backward compatibility
inline Camera CreatePerspectiveCamera(vec3 pos = vec3(0, 0, 0), quaternion rot = quaternion(0, 0, 0, 1), 
                                     float fov = 90.0f, float znear = 1.0f, float zfar = 100.0f) {
    return Camera(pos, rot, fov, znear, zfar);
}

inline Camera CreateOrthographicCamera(vec3 pos = vec3(0, 0, 0), quaternion rot = quaternion(0, 0, 0, 1),
                                      float scale = 1.0f, float zfar = 100.0f) {
    return Camera(pos, rot, scale, zfar);
}