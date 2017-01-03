/**
 *
 * tarray2D.h
 *
 * Simple 2D array implementation with tarray.
 * --> Basically, a 2D array wrapper
 *
 * Written by Taylor Shin
 * Sep. 16th, 2016
 *
**/

#ifndef __tarray2D_h__
#define __tarray2D_h__

#include "tarray.h"

OPEN_NS_OCTREE

/**
 *
 * TArray2D class
 *
**/
template<typename T>
class TArray2D : public TArray<T, 2>
{
public:

    // Access methods
    uint64_t M() const;
    uint64_t N() const;

    const T& at(uint64_t i, uint64_t j) const;
    const T& operator() (uint64_t i, uint64_t j) const;
    T& operator() (uint64_t i, uint64_t j);

    // Subarray access
    TArray2D<T> subarray(
        uint64_t iBegin, uint64_t jBegin,
        uint64_t iEnd, uint64_t jEnd);


    /**
     *
     * Constructors and Destructors
     *
    **/
    TArray2D();
    TArray2D(uint64_t m, uint64_t n);
    TArray2D(const TArray<T, 2>& array);



}; /* class TArray2D */


// Constructors and Destructors
template<typename T>
TArray2D<T>::TArray2D() : TArray<T,2>()
{}

template<typename T>
TArray2D<T>::TArray2D(uint64_t m, uint64_t n) : \
    TArray<T, 2>()
{
    TVector<uint64_t, 2> sizes;
    sizes(1) = m;
    sizes(0) = n;

    TArray<T, 2>::operator= (TArray<T, 2>(sizes));
}

template<typename T>
TArray2D<T>::TArray2D(const TArray<T,2>& array) : \
    TArray<T, 2>(array)
{}

// Access methods
template<typename T>
uint64_t TArray2D<T>::M() const
{
    return this->sizes()(1);
}
template<typename T>
uint64_t TArray2D<T>::N() const
{
    return this->sizes()(0);
}

template<typename T>
const T& TArray2D<T>::at(uint64_t i, uint64_t j) const
{
    TVector<uint64_t, 2> indices;
    indices(1) = i;
    indices(0) = j;
    return TArray<T, 2>::at(indices);
}

template<typename T>
const T& TArray2D<T>::operator() (uint64_t i, uint64_t j) const
{
    return at(i, j);
}

template<typename T>
T& TArray2D<T>::operator() (uint64_t i, uint64_t j)
{
    TVector<uint64_t, 2> indices;
    indices(1) = i;
    indices(0) = j;
    return TArray<T, 2>::operator() (indices);
}

// Subarray access
template<typename T>
TArray2D<T> TArray2D<T>::subarray(
    uint64_t iBegin, uint64_t jBegin,
    uint64_t iEnd, uint64_t jEnd
)
{
    TVector<uint64_t, 2> begin, end;
    begin(1) = iBegin;
    begin(0) = jBegin;
    end(1) = iEnd;
    end(0) = jEnd;
    return TArray<T, 2>::subarray(begin, end);
}

CLOSE_NS_OCTREE


#endif /* Include guard */
