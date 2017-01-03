/**
 *
 * tarray.tcc
 *
 * Simple array implementation for octree
 * (implementation)
 *
 * Written by Taylor Shin
 * Sep. 16th, 2016
 *
**/

//#include "tarray.h"

OPEN_NS_OCTREE

/**
 *
 * Private Stuff
 *
**/



/**
 *
 * Public Stuff
 *
**/
template<typename T, int_tvec N>
const TVector<uint64_t, N>& TArray<T, N>::sizes() const
{
    return sizes_;
}

template<typename T, int_tvec N>
const T& TArray<T, N>::at(const TVector<uint64_t, N>& indices) const
{
    return data_[dataIndex(indices)];
}

template<typename T, int_tvec N>
const T& TArray<T, N>::operator() (const TVector<uint64_t, N>& indices) const
{
    return at(indices);
}

template<typename T, int_tvec N>
T& TArray<T, N>::operator() (const TVector<uint64_t, N>& indices)
{
    return data_[dataIndex(indices)];
}

template<typename T, int_tvec N>
int TArray<T, N>::dataIndex(const TVector<uint64_t, N>& indices) const
{
    for (auto i = 0; i < N; ++i) {
        assert(indices(i) >= 0 && indices(i) < sizes_(i));
    }
    return offset_ + sum(indices*strides_);
}

template<typename T, int_tvec N>
TArray<T, N> TArray<T, N>::subarray(
    const TVector<uint64_t, N>& begin,
    const TVector<uint64_t, N>& end)
{
    TArray<T, N> sub;
    sub.data_ = data_;
    sub.offset_ = dataIndex(begin);
    sub.strides_ = strides_;
    sub.sizes_ = end - begin;
    return sub;
}

/**
 *
 * Constructors and Destructors
 *
**/
template<typename T, int_tvec N>
TArray<T,N>::TArray() : \
    data_(0), sizes_(0)
{}

template<typename T, int_tvec N>
TArray<T,N>::TArray(const TVector<uint64_t,N>& sizes) : \
    data_(std::make_shared< TVector<T,N> >(TVector<T,N>(prod(sizes)))),
    offset_(0),
    strides_(comprod(sizes)/sizes(0)),
    sizes_(sizes)
{}

template<typename T, int_tvec N>
TArray<T, N>::~TArray()
{}


/**
 *
 * Some other stuffs...
 *
**/


CLOSE_NS_OCTREE