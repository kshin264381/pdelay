/**
 *
 * Utils.h
 *
 * Some misc. utils that doesn't deserve its own class.
 *
 * Written by Taylor Shin
 * Oct. 18th, 2016
 *
**/

#ifndef __Utils_h__
#define __Utils_h__

#include <algorithm>
#include <string>
#include <sstream>

#include <fputils.h>

/**
 * Some basic string manipulation tools.
 *
 * --> They shouldn't be here but I'm not inclined to make
 * another header file for such small parts.
 *
**/
static std::string str_to_lower(const std::string& str)
{
    std::string STR = str;
    std::transform(STR.begin(), STR.end(), STR.begin(), ::tolower);
    return STR;
}
static std::string str_to_upper(const std::string& str)
{
    std::string STR = str;
    std::transform(STR.begin(), STR.end(), STR.begin(), ::toupper);
    return STR;
}

// Another small string manipulation utility to replace certain letters.
static std::string replace(
    const std::string& original_str,
    const std::string& c_tobe_r,
    const std::string& c_to_r)
{
    std::string new_string = original_str;
    char c_r = c_tobe_r[0];
    char c_o = c_to_r[0];
    std::replace(new_string.begin(), new_string.end(), c_r, c_o);
    return new_string;
}

// Quick string to fp_t conversion tool
//
template <typename T>
static T str_to_num(const std::string& input_str)
{
    T val;
    std::stringstream stream(input_str);
    stream >> val;
    if (stream.fail()) {
        std::runtime_error e(input_str);
        throw e;
    }
    return val;
}


/**
 *
 * Generates a random 2D and 3D vector
 *
 *
**/
template <typename T>
static __tuple__<T> UnitVec2D()
{
	T rad = uniform_rand<T>(T(-1.0), T(1.0));
	return __tuple__<T>{ sin(rad), cos(rad) };
}

template <typename T>
static __tuple_3D__<T> UnitVec3D()
{
	T costheta = uniform_rand<T>(T(-1.0), T(1.0));
	T phi = T(2.0) * static_cast<T>(M_PI);
	T theta = acos(costheta);

    return __tuple_3D__<T>( sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta) );
}

#endif /* Include guard */