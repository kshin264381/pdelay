/**
 *
 * tarray.h
 *
 * Simple array implementation for octree
 *
 * Written by Taylor Shin
 * Sep. 16th, 2016
 *
**/

#ifndef __tarray_h__
#define __tarray_h__

#include <cassert>
#include <memory>

#include "tvector.h"

OPEN_NS_OCTREE

/**
 *
 * The TArray class -> A wrapper for TVector
 *
**/
template<typename T, int_tvec N>
class TArray
{
private:
    // Data container
    std::shared_ptr< TVector<T,N> > data_;
    uint64_t offset_;
    TVector<uint64_t, N> strides_;
    TVector<uint64_t, N> sizes_;

    // Data indexing
    int dataIndex(const TVector<uint64_t, N>& indices) const;

public:
    // Size container
    const TVector<uint64_t, N>& sizes() const;

    // Access methods
    const T& at(const TVector<uint64_t, N>& indices) const;
    const T& operator() (const TVector<uint64_t, N>& indices) const;
    T& operator() (const TVector<uint64_t, N>& indices);

    // Subarray
    TArray<T, N> subarray(
        const TVector<uint64_t, N>& begin,
        const TVector<uint64_t, N>& end);


    /**
     *
     * Constructors and Destructors
     *
    **/
    TArray();
    TArray(const TVector<uint64_t, N>& sizes);
    virtual ~TArray();
};


CLOSE_NS_OCTREE

#include "tarray.tcc"

#endif /* Include guard */
