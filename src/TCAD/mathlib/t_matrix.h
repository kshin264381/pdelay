/**
 *
 * t_matrix.h
 *
 *  Matrix implementation with uBlas library
 *  (actually, it's just a header file...)
 *  from Boost.
 *
 *  Created on: Mar 22, 2016
 *      Author: Taylor Shin
 *
**/

#ifndef SRC_T_MATRIX_H_
#define SRC_T_MATRIX_H_

#ifdef _OPENMP
    #include <omp.h>
#endif

#include "unique_ptr.h"
#include "fputils.h"

// use uBlas if cygwin...
#if defined(__CYGWIN__) && !defined(_WIN32)
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>

// using Matrix library from boost::numeric::ublas
using namespace boost::numeric::ublas;

// Setting up NDEBUG for release ublas library
#ifndef NDEBUG
#define NDEBUG
#endif

// Defining a few tuple stuff
using t_m_size = __tuple__<uint64_t>;


template <class T>
class t_matrix
{
private:
    std::unique_ptr<matrix<T>> matrix_data;

    matrix<T> matrix_inverse(); // returns inversed matrix.

public:
    //
    // Operator overloading.
    //
    t_matrix& operator=(const t_matrix& rhs); // = operator with t_matrix
    t_matrix& operator=(const matrix<T>& rhs); // = operator with matrix
    t_matrix& operator+(const t_matrix& rhs); // + operator with t_matrix
    t_matrix& operator+(const matrix<T>& rhs); // + operator with matrixj
    t_matrix& operator+(const T& rhs); // + operator with a scalar
    t_matrix& operator+=(const t_matrix& rhs); // += operator with t_matrix
    t_matrix& operator+=(const matrix<T>& rhs); // += operator with matrixj
    t_matrix& operator+=(const T& rhs); // += operator with a scalar
    t_matrix& operator-(const t_matrix& rhs); // - operator with t_matrix
    t_matrix& operator-(const matrix<T>& rhs); // - operator with matrix
    t_matrix& operator-(const T& rhs); // - operator with a scalar
    t_matrix& operator-=(const t_matrix& rhs); // -= operator with t_matrix
    t_matrix& operator-=(const matrix<T>& rhs); // -= operator with matrix
    t_matrix& operator-=(const T& rhs); // -= operator with a scalar
    t_matrix& operator*(const t_matrix& rhs); // * operator with t_matrix
    t_matrix& operator*(const matrix<T>& rhs); // * operator with matrix
    t_matrix& operator*(const T& rhs); // * operator with a scalar
    t_matrix& operator*=(const t_matrix& rhs); // *= operator with t_matrix
    t_matrix& operator*=(const matrix<T>& rhs); // *= operator with matrix
    t_matrix& operator*=(const T& rhs); // *= operator with a scalar
    t_matrix& operator/(const T& rhs); // / operator with a scalar
    t_matrix& operator/=(const T& rhs); // / operator with a scalar
    bool operator==(const t_matrix& rhs); // == operator
    bool operator==(const matrix<T>& rhs); // == operator with matrix
    bool operator!=(const t_matrix& rhs); // != operator
    bool operator!=(const matrix<T>& rhs); // != operator with matrix

    //
    // Other mathematical operations.
    //
    void Inv(); // Inverse itself
    t_matrix GetInv(); // Returns inverse matrix

    void Tran(); // Transpose self
    t_matrix GetTran(); // Returns transposed matrix

    T Det(); // Returns determinent

    void LU(t_matrix& L, t_matrix& U); // LU decompisition

    std::vector<T> EigenVec(); // Returns eigenvector
    T EigenVal(); // Returns eigenvalue
    void Eigen(std::vector<T>& EigenVector, T& EigenValue); // Obtains Eigenvalue stuff.

    //
    // Utility operations...
    //
    t_m_size Size();
    matrix<T> GetMatrix();
    bool isSquare();
    void SetMatrix(const matrix<T>& input_matrix);

    //
    // Constructors and Destructors.
    //
    t_matrix(); // 1 x 1 zero matrix.
    t_matrix(const uint64_t& row, const uint64_t& col); // Zero row x col matrix
    t_matrix(const uint64_t& n); // Zero n x n matrix
    t_matrix(const uint64_t& n, const T& init_value); // Unit n x n matrix with given init_value
    t_matrix(const matrix<T>& Matrix); // Initialize with matrix
    t_matrix(const std::vector<std::vector<T>> two_D_vec); // initialize with 2D vector
    t_matrix(const T* two_D_arr[], const uint64_t& rows, const uint64_t& cols); // initialize with 2D array

    virtual ~t_matrix() {}; // Destructor
};


#else
#include <mkl.h>

template<class T>
class t_matrix
{
private:
    /**
     *
     * private stuff
     *
     *
    **/


public:
    /**
     *
     * public stuff
     *
    **/


    /**
     *
     * Constructors and destructors
     *
    **/
    t_matrix();
};





#endif /* #if defined(__CYGWIN__) && !defined(_WIN32) */






#include "t_matrix.tcc"

#endif /* SRC_T_MATRIX_H_ */
