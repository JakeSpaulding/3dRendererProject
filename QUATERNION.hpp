#ifndef QUATERNION_HPP
#define QUATERNION_HPP
#include "vectors.hpp"
// contains the quaternion struct

struct quaternion {
    union {
        struct {
            float x, y, z, w; // uses xyzw order
        };
        float data[4];
    };
    quaternion(float a = 0, float b = 0, float c = 0, float d = 1) {
        data[0] = a;
        data[1] = b;
        data[2] = c;
        data[3] = d;
    }
    quaternion(float nums[4]) {
        std::copy(&nums[0], &nums[0] + 4, data);
    }
    quaternion(vec4 const& nums) {
        for (int i = 0; i < 4; i++) { data[i] = nums[i]; }
    }
    quaternion(vec3 const& v) {
        data[0] = v[0];
        data[1] = v[1];
        data[2] = v[2];
        data[3] = 0;
    }
    quaternion conj() const {
        quaternion q(-1 * data[0], -1 * data[1], -1 * data[2], data[3]);
        return(q);
    }
    quaternion operator*(quaternion const& q2) const {
        float a = data[3] * q2.data[0] + data[0] * q2.data[3] + data[1] * q2.data[2] - data[2] * q2.data[1];
        float b = data[3] * q2.data[1] - data[0] * q2.data[2] + data[1] * q2.data[3] + data[2] * q2.data[0];
        float c = data[3] * q2.data[2] + data[0] * q2.data[1] - data[1] + q2.data[0] + data[2] * q2.data[3];
        float d = data[3] * q2.data[3] - data[0] * q2.data[0] - data[1] * q2.data[1] - data[2] * q2.data[2];
        return(quaternion(a, b, c, d));
    }
    quaternion operator*(vec4 const& vec) const {
        quaternion v(vec[0], vec[1], vec[2], 0);
        return(*this * v);
    }
    vec4 qtv4() const {
        return(vec4(data[0], data[1], data[2], data[3]));
    }
    vec4 qtv4h() const {
        return(vec4(data[0], data[1], data[2], 1));
    }
    vec3 qtv3() const {
        return(vec3(data[0], data[1], data[2]));
    }
    vec4 rotate(vec4 const& v) const {
        vec4 q = this->qtv4h();
        vec4 qv = q.cross3(v);
        vec4 qqv = q.cross3(qv);
        return(v + 2 * w * qv + 2 * qqv);
    }
    vec3 rotate(vec3 const& v) const {
        vec3 q = this->qtv3();
        vec3 qv = q.cross(v);
        vec3 qqv = q.cross(qv);
        return(v + 2 * w * qv + 2 * qqv);
    }
};



#endif