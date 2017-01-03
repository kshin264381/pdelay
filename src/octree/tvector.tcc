/**
 *
 * tvector.tcc
 *
 * Simple vector implementation for octree
 * (implementation)
 *
 * Written by Taylor Shin
 * Sep. 16th, 2016
 *
**/

#include <cmath>

//#include "tvector.h"

/**
 * Operator Overloading
 *
**/
// Assignment operator
template<typename T, int_tvec N>
template< typename T2 >
octree::TVector<T,N>& octree::TVector<T,N>::operator= (const octree::TVector<T2,N>& v)
{
    for (auto i=0; i<N; ++i)
        _data[i] = v(i);
    return *this;
}

// Binary operators
// +
template<typename T1, typename T2, int_tvec N>
octree::TVector<typename PreferFP<T1,T2>::Result, N> operator+ (
    const octree::TVector<T1,N>& v1, const octree::TVector<T2,N>& v2)
{
    octree::TVector<typename PreferFP<T1,T2>::Result, N> v3(v1);
    v3 += v2;
    return v3;
}

// -
template<typename T1, typename T2, int_tvec N>
octree::TVector<typename PreferFP<T1,T2>::Result, N> operator- (
    const octree::TVector<T1,N>& v1, const octree::TVector<T2,N>& v2)
{
    octree::TVector<typename PreferFP<T1,T2>::Result, N> v3(v1);
    v3 -= v2;
    return v3;
}

// * vector
template<typename T1, typename T2, int_tvec N>
octree::TVector<typename PreferFP<T1,T2>::Result, N> operator* (
    const octree::TVector<T1,N>& v1, const octree::TVector<T2,N>& v2)
{
    octree::TVector<typename PreferFP<T1,T2>::Result, N> v3(v1);
    v3 *= v2;
    return v3;
}
// * scalar return scalar
template<typename T1, typename T2, int_tvec N>
octree::TVector<typename PreferFP<T1,T2>::Result, N> operator* (
    const octree::TVector<T1,N>& v1, const T2& a)
{
    octree::TVector<typename PreferFP<T1,T2>::Result, N> v2(v1);
    v2 *= a;
    return v2;
}
// scalar *
template<typename T, int_tvec N>
octree::TVector<T,N> operator* (const T& a, octree::TVector<T,N> v1)
{
    for(auto i=0; i<N; ++i)
        v1(i) = a*v1(i);
    return v1;
}

// scalar * returning TVector
template<typename T1, typename T2, int_tvec N>
octree::TVector<typename PreferFP<T1,T2>::Result, N> operator* (
    const T2& a, const octree::TVector<T1,N>& v1)
{
    return a * \
        octree::TVector<typename PreferFP<T1,T2>::Result, N>(v1);
}

// / vector
template<typename T1, typename T2, int_tvec N>
octree::TVector<typename PreferFP<T1,T2>::Result, N> operator/ (
    const octree::TVector<T1,N>& v1, const octree::TVector<T2,N>& v2)
{
    octree::TVector<typename PreferFP<T1,T2>::Result, N> v3(v1);
    v3 /= v2;
    return v3;
}
// / scalar return scalar
template<typename T1, typename T2, int_tvec N>
octree::TVector<typename PreferFP<T1,T2>::Result, N> operator/ (
    const octree::TVector<T1,N>& v1, const T2& a)
{
    octree::TVector<typename PreferFP<T1,T2>::Result, N> v2(v1);
    v2 /= a;
    return v2;
}
// scalar /
template<typename T, int_tvec N>
octree::TVector<T,N> operator/ (const T& a, octree::TVector<T,N> v1)
{
    for(auto i=0; i<N; ++i)
        v1(i) = a/v1(i);
    return v1;
}

// scalar / returning TVector
template<typename T1, typename T2, int_tvec N>
octree::TVector<typename PreferFP<T1,T2>::Result, N> operator/ (
    const T2& a, const octree::TVector<T1,N>& v1)
{
    return a / \
        octree::TVector<typename PreferFP<T1,T2>::Result, N>(v1);
}


// Some user-friendly (?) stuffs.
// summension
template<typename T, int_tvec N>
T sum(const octree::TVector<T,N>& v)
{
    T sum(0);
    for (auto i=0; i<N; ++i)
        sum += v(i);
    return sum;
}
// element product
template<typename T, int_tvec N>
T prod(const octree::TVector<T,N>& v)
{
    T prod(1);
    for (auto i=0; i<N; ++i)
        prod *= v(i);
    return prod;
}

// cumulative product
template<typename T, int_tvec N>
octree::TVector<T,N> cumprod(const octree::TVector<T,N>& v)
{
    octree::TVector<T,N> p;
    T prev(1);
    for (auto i=0; i<N; ++i)
        p(i) = prev = prev * v(i);

    return p;
}

// dot product --> They must match N anyway!!
template<typename T, int_tvec N>
T dotprod(const octree::TVector<T,N>& v1, const octree::TVector<T,N>& v2)
{
    T sum(0);
    for (auto i=0; i<N; ++i)
        sum += v1(i)*v2(i);

    return sum;
}

// Norm
template<typename T, int_tvec N>
T norm(const octree::TVector<T,N>& v)
{
    return std::sqrt( sum(v*v) );
}

/**
 * Constructors and Destructors
 *
**/

// Initialize an empty vector
template<typename T, int_tvec N>
octree::TVector<T,N>::TVector()
{
    _data = std::vector<T>();
}

// Initialize with a single value.
template<typename T, int_tvec N>
octree::TVector<T,N>::TVector(const T& value)
{
    for (auto i=0; i<N; ++i)
        _data[i] = value;
}

// Initialize with another TVector
template<typename T, int_tvec N>
template<typename T2>
octree::TVector<T,N>::TVector(const octree::TVector<T2,N>& v)
{
    *this = v;
}
