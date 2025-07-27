#ifndef MATRIXTEMPLATE_HPP
#define MATRIXTEMPLATE_HPP

#include <iostream>
#include <algorithm>
#include <string.h>
#include "vectorTemplate.hpp"

// allow for accessing rows using subscripting
struct RowProxy {
    float* rowData;

    RowProxy(float* dataStart, size_t row, size_t M)
        : rowData(dataStart + M * row) {
    }

    float& operator[](size_t col) {
        return rowData[col];
    }
};

struct ConstRowProxy {
    const float* rowData;

    ConstRowProxy(const float* dataStart, size_t row, size_t M)
        : rowData(dataStart + M * row) {
    }

    const float& operator[](size_t col) const {
        return rowData[col];
    }
};


template<uint32_t M> // size of matrix

struct Mbase {
    float data[M * M];
    // default constructor
    Mbase() {
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < M; j++) {
                data[i * M +j] = 0;
            }
        }
    }
    // lets the user enter "eye" to get an identity matrix
    Mbase(const char txt[4]) :data{ 0 } {
        if (!strcmp(txt, "eye")) {
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < M; j++) {
                    data[i * M + j] = 0;
                }
            }
            for (int i = 0; i < M; i++) {
                data[i * M + i] = 1;
            }
        }
    }
    // takes float array inputs
    Mbase(const float* nums) {
        std::copy(&nums[0], &nums[0]+M*M, &data[0]);
    }

    // subscript access
    float& operator()(size_t row, size_t col) {
        return data[row * M + col];
    }
    const float& operator()(size_t row, size_t col) const{
        return data[row * M + col];
    }

    // allows for treatment like a 2d array

    RowProxy operator[](const size_t row) {
        return RowProxy(&data[0], row, M);
    }
    ConstRowProxy operator[](const size_t row) const{
        return ConstRowProxy(&data[0], row, M);
    }

    // matrix multiplication
    Mbase operator*(Mbase const& m2) const {
        Mbase tmp = Mbase();
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < M; j++) {
                for (int n = 0; n < M; n++) {
                    tmp.data[i * M + j] += data[i * M + n] * m2.data[n * M + j];
                }
            }
        }
        return(tmp);
    }

    // matrix-matrix addition
    Mbase operator+(Mbase const& m2) const {
        Mbase tmp = Mbase();
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < M; j++) {
                tmp.data[i * M + j] = data[i * M + j] + m2.data[i * M + j];
            }
        }
        return(tmp);
    }



    // matrix constant multiplication
    Mbase operator*(float n) const{
        Mbase tmp = Mbase();
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < M; j++) {
                tmp.data[i * M + j] = data[i * M + j] * n;
            }
        }
        return tmp;
    }
    // transpose
    Mbase T() {
        Mbase tmp;
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < M; j++) {
                tmp.data[i * M + j] = data [j * M + i];
            }
        }
        return(tmp);
    }
};
// defines scalar matrix multiplication again (in reverse order)
template<int M>
Mbase<M> operator*(float n, Mbase<M> const& m) {
    return(m * n);
}
// add printability

template<int M> std::ostream& operator<<(std::ostream& os, Mbase<M> const& m) {
    os << "\n";
    for (int j = 0; j < M; j++) {
        os << "[";
        for (int i = 0; i < M; ++i) {
            os << m(j,i);
            if (i != M - 1) {
                os << ", ";
            }
        }
        os << "]\n";
    }
    return os;
}

#endif // MATRIXTEMPLATE_HPP