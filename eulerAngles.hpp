#ifndef EULER_HPP
#define EULER_HPP


// Euler angle definition (consider the angles to be in the YXZ order)
struct Euler {
    // define our angles
    union {
        float y, x, z;
        float ang[3];
        float yaw, pitch, roll;
        float ry, rx, rz;
    };
    // constructors
    Euler(float ya = 0, float pi = 0, float ro = 0) : ang{ ya, pi, ro } {}
    Euler(float nums[3]) : ang{ nums[0], nums[1], nums[2] } {}

    // subscript access
    float operator[](int n) {
        return ang[n];
    }
    // subscript access
    const float operator[](int n) const {
        return ang[n];
    }
};



#endif