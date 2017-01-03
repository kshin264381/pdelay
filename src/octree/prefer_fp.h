/**
 *
 * prefer_fp.h
 *
 * Forcefully selects operation output as double or long double.
 *
 * Written by Taylor Shin
 * Sep. 16th, 2016
 *
**/

#ifndef __prefer_fp_h__
#define __prefer_fp_h__

#ifdef __MULTIPRECISION__
    #include <boost/multiprecision/cpp_int.hpp>
    #include <boost/multiprecision/cpp_bin_float.hpp>
    using namespace boost::multiprecision;

    using fp_tvec = cpp_bin_float_100;

    using int_tvec = uint64_t; // Index type
#else

#ifndef _MSC_VER
    // using fp_tvec = double; // Uncomment it if you need double type.
    using fp_tvec = long double;
#else
    using fp_tvec = double;
#endif /* #ifndef _MSC_VER */

    using int_tvec = uint64_t; // Index type
#endif /* #ifdef __MULTIPRECISION__ */


/**
 * Main template
 * --> Just returns the first datatype
**/
template<typename T1, typename T2>
class PreferFP
{
public:
    using Result = T1;
};

// indexing integer, fp_tvec case
template<>
class PreferFP<int_tvec, fp_tvec>
{
public:
    using Result = fp_tvec;
};

template<>
class PreferFP<fp_tvec, int_tvec>
{
public:
    using Result = fp_tvec;
};

// regular integer, fp_tvec case
template<>
class PreferFP<int, fp_tvec>
{
public:
    using Result = fp_tvec;
};

template<>
class PreferFP<fp_tvec, int>
{
public:
    using Result = fp_tvec;
};

// regular short, fp_tvec case
template<>
class PreferFP<short, fp_tvec>
{
public:
    using Result = fp_tvec;
};

template<>
class PreferFP<fp_tvec, short>
{
public:
    using Result = fp_tvec;
};




#endif /* Include guard */
