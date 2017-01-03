/**
 *
 * tvector.h
 *
 * Simple vector implementation for octree
 *
 * Written by Taylor Shin
 * Sep. 16th, 2016
 *
**/

#ifndef __tvector_h__
#define __tvector_h__

#include <cstdint>
#include <vector>

#include "prefer_fp.h"

#define OPEN_NS_OCTREE namespace octree {
#define CLOSE_NS_OCTREE }

OPEN_NS_OCTREE

/**
 *
 * The simple vector class
 *
 *
**/
template<typename T, int_tvec N>
class TVector
{
private:
    std::vector<T> _data;

public:
    /**
     * Operator Overloading
     *
    **/
    template<typename T2>
    TVector<T, N>& operator= (const TVector<T2, N>& v);

    // Access operator: (x, y) style.
    const T& operator() (const int_tvec& i) const { return at(i); }
    T& operator() (const int_tvec& i) { return _data[i]; }

    // Uniary operators.
    void operator+= (const TVector<T, N>& v)
    {
        for (auto i = 0; i < N; ++i)
            _data[i] += v._data[i];
    }
    void operator-= (const TVector<T, N>& v)
    {
        for (auto i = 0; i < N; ++i)
            _data[i] -= v._data[i];
    }
    void operator*= (const TVector<T, N>& v)
    {
        for (auto i = 0; i < N; ++i)
            _data[i] *= v._data[i];
    }
    void operator/= (const TVector<T, N>& v)
    {
        for (auto i = 0; i < N; ++i)
            _data[i] /= v._data[i];
    }

    /**
     * Some access functions
     *
    **/
    const T& at(const int_tvec& i) const { return _data[i]; }
    const std::vector<T> data() const { return _data; }
    std::vector<T> data() { return _data; }

    /**
     * Constructors and Destructors
     *
    **/
    TVector();
    TVector(const T& some_value);
    template<typename Tv>
    TVector(const TVector<Tv, N>& v);
    virtual ~TVector();
}; /* class TVector */

/**
 * Operator overloading and some useful stuffs
 * --> definition: Check *.cc file for their implementations.
 *
**/
// +
template<typename T1, typename T2, int_tvec N>
TVector<typename PreferFP<T1, T2>::Result, N> operator+ (
    const TVector<T1, N>& v1, const TVector<T2, N>& v2);
// -
template<typename T1, typename T2, int_tvec N>
TVector<typename PreferFP<T1, T2>::Result, N> operator- (
    const TVector<T1, N>& v1, const TVector<T2, N>& v2);
// *
template<typename T1, typename T2, int_tvec N>
TVector<typename PreferFP<T1, T2>::Result, N> operator* (
    const TVector<T1, N>& v1, const TVector<T2, N>& v2);

template<typename T1, typename T2, int_tvec N>
TVector<typename PreferFP<T1, T2>::Result, N> operator* (
    const TVector<T1, N>& v1, const T2& a);

template<typename T, int_tvec N>
TVector<T, N> operator* (const T& a, TVector<T, N> v1);
// /
template<typename T1, typename T2, int_tvec N>
TVector<typename PreferFP<T1, T2>::Result, N> operator/ (
    const T2& a, const TVector<T1, N>& v1);

template<typename T1, typename T2, int_tvec N>
TVector<typename PreferFP<T1, T2>::Result, N> operator/ (
    const TVector<T1, N>& v1, const TVector<T2, N>& v2);

template<typename T1, typename T2, int_tvec N>
TVector<typename PreferFP<T1, T2>::Result, N> operator/ (
    const TVector<T1, N>& v1, const T2& a);

template<typename T, int_tvec N>
TVector<T, N> operator/ (const T& a, TVector<T, N> v1);

template<typename T1, typename T2, int_tvec N>
TVector<typename PreferFP<T1, T2>::Result, N> operator/ (
    const T2& a, const TVector<T1, N>& v1);

// Some user-friendly (?) stuffs.
// summension
template<typename T, int_tvec N>
T sum(const TVector<T, N>& v);

// element product
template<typename T, int_tvec N>
T prod(const TVector<T, N>& v);

// cumulative product
template<typename T, int_tvec N>
TVector<T, N> cumprod(const TVector<T, N>& v);

// dot product --> They must match N anyway!!
template<typename T, int_tvec N>
T dotprod(const TVector<T, N>& v1, const TVector<T, N>& v2);

// Norm
template<typename T, int_tvec N>
T norm(const TVector<T, N>& v);


CLOSE_NS_OCTREE

#include "tvector.tcc"

#endif /* Inlclude guard */
