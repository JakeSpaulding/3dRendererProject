#ifndef VECTORTEMPLATE_HPP
#define VECTORTEMPLATE_HPP

#include <algorithm>
#include <iostream>
#include <cstdint>
using namespace std;

// the base of the vector struct, allows for access using subscripts
template <typename Derived, unsigned int N> struct Vbase { // N is the size of the vector
    float data[N];

    // default constructor, allows an array of N floats to be passed
    Vbase(float const nums[N]) {
        std::copy(nums, nums + N, data);
    }
    // the default function is to make data 
    Vbase() {
        for (int i = 0; i < N; i++) {
            data[i] = 0;
        }
        // bind the x y z w access
    }

    // subscript access
    float& operator[](int n) {
        return(data[n]);
    }
    const float operator[](int n) const {
        return(data[n]);
    }
    float& operator()(int n) {
        return(data[n]);
    }
    const float operator()(int n) const {
        return(data[n]);
    }

    // vector addition
    Derived operator+(Derived const& v2) const {
        Derived v;
        for (int i = 0; i < N; i++) {
            v[i] = data[i] + v2[i];
        }
        return v;
    }

    // compare value
    bool operator==(Derived const& v) const{
        for (int i = 0; i < N; i++) {
            if (data[i] != v.data[i]) {
                return 0;
            }
        }
        return 1;
    }

    // the dot product
    float operator*(Derived const& v2) const {
        float tmp(0.0f);
        for (int i = 0; i < N; i++)
            tmp += v2[i] * data[i];
        return tmp;
    }

    // returns the norm
    float norm() const {
        float tmp(0.0f);
        for (int i = 0; i < N; i++) {
            tmp += data[i] * data[i];
        }
        return(sqrt(tmp));
    }
    // scalar multiplication
    Derived operator*(float n) const{
        float v[N]; // make a temporary float array
        std::copy(&data[0], &data[0] + N, v);
        for (int i = 0; i < N; i++) {
            v[i] *= n;
        }
        return Derived(v);
    }
    // normalizes the vector
    void normalize() {
        float n = this->norm();
        if(n){
            for (int i = 0; i < N; i++) {
                data[i] /= n;
            }
        }
    }

    // outputs a normalized vector
    Derived normalized() const{
        Derived v = *this;
        float n = this->norm();
        if(n){ // avoid 0 division
            for (int i = 0; i < N; i++) {
                v[i] /= n;
            }
        }
        return v;
    }

    // define subtraction
    Derived operator-(Derived const& v2) const {
        Derived v;
        for (int i = 0; i < N; i++) {
            v[i] = data[i] - v2[i];
        }
        return v;
    }
};
// provides the scalar multiplication in the other order
template <typename Derived>
inline Derived operator*(float const& n, const Derived& v) {
    return v * n;
}

// give the ability to be printed
template <typename Derived, int N>
inline std::ostream& operator<<(std::ostream& os, Vbase<Derived, N> const& v) {
    os << "[";
    for (int i = 0; i < N; ++i) {
        os << v[i];
        if (i != N - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

#endif // VECTORTEMPLATE_HPP