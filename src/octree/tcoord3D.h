/**
 *
 * tcoord3D.h
 *
 * Simple 3D coordinate set.
 * --> Wrapper for tarray.h
 *
 * Written by Taylor Shin
 * Sep. 16th, 2016
 *
**/

#ifndef __tcoord3D_h__
#define __tcoord3D_h__

#include "tvector.h"

OPEN_NS_OCTREE

template<typename T>
class Coord3D : public TVector<T, 3>
{
public:
    // Access methods
    const T& x() const { return this->at(0); }
    const T& y() const { return this->at(1); }
    const T& z() const { return this->at(2); }
    T& x() { return (*this)(0); }
    T& y() { return (*this)(1); }
    T& z() { return (*this)(2); }

    /**
     *
     * Consructors and Destructors
     *
    **/
    Coord3D() {}
    Coord3D(T x, T y, T z)
    {
        (*this)(0) = x;
        (*this)(1) = y;
        (*this)(2) = z;
    }

    Coord3D(const TVector<T,3>& v) : TVector<T,3>(v)
    {}

}; /* class Coord3D */

CLOSE_NS_OCTREE

#endif /* Include guard */
