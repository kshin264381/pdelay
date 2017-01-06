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

#endif /* Include guard */