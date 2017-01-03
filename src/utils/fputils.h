/**
 *
 * fputils.h
 *
 * Floating point datatype utilities.
 *
 * Don't use those utility functions with integers, strings
 * pointers and many magical stuffs other than:
 * float, double, long double.
 *
 * Written by Taylor Shin
 * Feb. 25th, 2016
 *
**/
#ifndef __fputils_h__
#define __fputils_h__

#include <algorithm>
#include <cmath>
#include <limits>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>

/* Uncomment below to enable boost::multiprecision */
//#define __MULTIPRECISION__

#ifdef __MULTIPRECISION__
    #include <boost/multiprecision/cpp_int.hpp>
    #include <boost/multiprecision/cpp_bin_float.hpp>
    using namespace boost::multiprecision;
#endif

// float typedefs
using f_t = float;
using cf_t = const float;
using d_t = double;
using cd_t = const double;
using ld_t = double;
using cld_t = const double;

#ifdef __MULTIPRECISION__
    // boost::multiprecision float typedefs
    using fp_t = cpp_bin_float_100;
    using cfp_t = const cpp_bin_float_100;
    // int typedefs
    using fp_int_t = cpp_int;
    using cfp_int_t = const cpp_int;
    using fp_uint_t = cpp_int;
    using cfp_uint_t = const cpp_int;
#else
    // Default floating point typedefs...
    using fp_t = ld_t;
    using cfp_t = cld_t;
    // Default integer typedefs...
    using fp_int_t = int64_t;
    using cfp_int_t = const int64_t;
    using fp_uint_t = uint64_t;
    using cfp_uint_t = const uint64_t;
#endif

// Some fp_t static cast conversion shortcuts.
#define F_T(num)         static_cast<f_t>(num)
#define FP_T(num)        static_cast<fp_t>(num)
#define CFP_T(num)       static_cast<cfp_t>(num)
#define FP_INT_T(num)    static_cast<fp_int_t>(num)
#define CFP_INT_T(num)   static_cast<cfp_int_t>(num)
#define FP_UINT_T(num)   static_cast<fp_uint_t>(num)
#define CFP_UINT_T(num)  static_cast<cfp_uint_t>(num)

// std::min or std::max override for MSVC
#ifdef _MSC_VER
template<typename T>
const T& std_min(const T& a, const T& b)
{
    return (std::min)(a, b);
}
template<typename T>
const T& std_max(const T& a, const T& b)
{
    return (std::max)(a, b);
}
#else
// template<typename T>
// constexpr const T& std_min(const T& a, const T& b)
// {
//     return std::min(a, b);
// }
// template<typename T>
// constexpr const T& std_max(const T& a, const T& b)
// {
//     return std::max(a, b);
// }
#define std_min(A, B) std::min(A, B)
#define std_max(A, B) std::max(A, B)
#endif

/**
 *
 * Comparison methods
 *
**/
// A == B
template <typename T>
bool fp_equal(const T& A, const T& B)
{
    T abs_A = std::fabs(A);
    T abs_B = std::fabs(B);
    T _diff = std::fabs(A - B);
    T epsilon = std_min(abs_A, abs_B) * \
        std::numeric_limits<T>::epsilon();
    T MIN = (std::numeric_limits<T>::min)();
    T MAX = (std::numeric_limits<T>::max)();
    T ZERO = static_cast<T>(0.0);

    if (A == B) {
        // Immediate case.
        return true;
    }
    else if (A == ZERO || B == ZERO || _diff < epsilon) {
        // If any of A or B is 0.0, return a comparison with
        // a minimum number.
        return _diff < epsilon*MIN;
    }
    else {
        // Regular routine.
        return _diff / std_min((abs_A + abs_B), MAX) < epsilon;
    }
}

// A > B
template <typename T>
bool fp_mt(const T& A, const T& B)
{
    if (!fp_equal<T>(A, B))
        return (A - B) > static_cast<T>(0.0);
    else
        return false;
}

// A >= B
template <typename T>
bool fp_mteq(const T& A, const T& B)
{
    if (fp_equal<T>(A, B))
        return true;
    else
        return (A - B) > static_cast<T>(0.0);
}

// A < B
template <typename T>
bool fp_lt(const T& A, const T& B)
{
    if (!fp_equal<T>(A, B))
        return (B - A) > static_cast<T>(0.0);
    else
        return false;
}

// A <= B
template <typename T>
bool fp_lteq(const T& A, const T& B)
{
    if (fp_equal<T>(A, B))
        return true;
    else
        return (B - A) > static_cast<T>(0.0);
}

// simple min max
template <typename T>
T fp_min(const T& A, const T& B)
{
    if (fp_lteq<T>(A, B))
        return A;
    else
        return B;
}
template <typename T>
T fp_max(const T& A, const T& B)
{
    if (fp_lteq<T>(A, B))
        return B;
    else
        return A;
}

// Returns max from multiple inputs
template <typename T, typename... Ts>
T fp_max(const T& A, const T& B, Ts&&... vs)
{
    if (fp_lteq<T>(A, B))
        return fp_max<T>(B, std::forward<Ts>(vs)...);
    else
        return fp_max<T>(A, std::forward<Ts>(vs)...);
}
// Returns min from multiple inputs
template <typename T, typename... Ts>
T fp_min(const T& A, const T& B, Ts&&... vs)
{
    if (fp_lteq<T>(A, B))
        return fp_min<T>(A, std::forward<Ts>(vs)...);
    else
        return fp_min<T>(B, std::forward<Ts>(vs)...);
}

/**
 *
 * Cartesian 2D struct
 *
**/
template <typename T>
struct __tuple__{
    T x; T y;

    // Constructors
    __tuple__()
    { this->x = static_cast<T>(0.0); this->y = static_cast<T>(0.0); }
    __tuple__(const T& x_in, const T& y_in)
    { this->x = x_in; this->y = y_in; }
    __tuple__(const __tuple__& other)
    { this->x = other.x; this->y = other.y; }

    // Operator overloading
    // access operator ()
    T operator() (const short& index) const
    {
        if (index > 1 || index < 0) {
            std::cerr << "__tuple__: given index exceeds tuple elements!!" << std::endl;
            exit(-1);
        }

        switch (index) {
        case 0:
            return this->x;
        case 1:
            return this->y;
        }
    }

    // +
    __tuple__ operator+(const __tuple__& other)
    {
        x = x+other.x; y = y+other.y;
        return *this;
    }
    // +=
    __tuple__& operator+=(const __tuple__& other)
    {
        x = x+other.x; y = y+other.y;
        return *this;
    }
    // + with scalar
    __tuple__ operator+(const T& other_scalar) const
    {
        return __tuple__{x+other_scalar, y+other_scalar};
    }
    // += with scalar
    __tuple__& operator+=(const T& other_scalar) const
    {
        return __tuple__{x+other_scalar, y+other_scalar};
    }

    // -
    __tuple__ operator-(const __tuple__& other)
    {
        x = x-other.x; y = y-other.y;
        return *this;
    }
    // -=
    __tuple__& operator-=(const __tuple__& other)
    {
        x = x-other.x; y = y-other.y;
        return *this;
    }
    // - with scalar
    __tuple__ operator-(const T& other_scalar) const
    {
        return __tuple__{x-other_scalar, y-other_scalar};
    }
    // -= with scalar
    __tuple__& operator-=(const T& other_scalar) const
    {
        return __tuple__{x-other_scalar, y-other_scalar};
    }

    // *
    __tuple__ operator*(const __tuple__& other)
    {
        x = x*other.x; y = y*other.y;
        return *this;
    }
    // *=
    __tuple__& operator*=(const __tuple__& other)
    {
        x = x*other.x; y = y*other.y;
        return *this;
    }
    // * with scalar
    __tuple__ operator*(const T& other_scalar) const
    {
        return __tuple__{x*other_scalar, y*other_scalar};
    }
    // *= with scalar
    __tuple__& operator*=(const T& other_scalar) const
    {
        return __tuple__{x*other_scalar, y*other_scalar};
    }

    // /
    __tuple__ operator/(const __tuple__& other)
    {
        x = x/other.x; y = y/other.y;
        return *this;
    }
    // /=
    __tuple__& operator/=(const __tuple__& other)
    {
        x = x/other.x; y = y/other.y;
        return *this;
    }

    // / with scalar
    __tuple__ operator/(const T& other_scalar) const
    {
        return __tuple__{x/other_scalar, y/other_scalar};
    }
    // /= with scalar
    __tuple__& operator/=(const T& other_scalar) const
    {
        return __tuple__{x/other_scalar, y/other_scalar};
    }

    // ==
    bool operator==(const __tuple__& other) const
    {
        if (x == other.x && y == other.y) return true;
        else return false;
    }
    // !=
    bool operator!=(const __tuple__& other) const
    {
        if (x == other.x && y == other.y) return false;
        else return true;
    }

    // distance
    T dist(const __tuple__& other) const
    {
        T x_dist = other.x - x;
        T y_dist = other.y - y;
        return sqrt( x_dist*x_dist + y_dist*y_dist );
    }

    // Absolute value
    T _abs() const
    { return sqrt(x*x+y*y); }

    // norm (unit vector with the same direction)
    __tuple__ _norm() const
    { return __tuple__{ x / this->_abs(), y / this->_abs() }; }

    // direction to other vector (unit vector)
    __tuple__ direction(const __tuple__& other) const
    { return __tuple__{other.x-x, other.y-y}._norm(); }

    // Text output
    std::string to_str() const
    {
        std::stringstream ss;
        ss << "x: " << x << ", y: " << y;
        return ss.str();
    }

    // Checking nan
    short _isnan() const
    {
#ifdef __MULTIPRECISION__
        if ( (boost::math::isnan)(x)) return 1;
        if ( (boost::math::isnan)(y)) return 2;
#else
        if (std::isnan(x)) return 1;
        if (std::isnan(y)) return 2;
#endif
        return 0;
    }
};

/**
 *
 * Cartesian coordinate structs
 *
**/
template <typename T>
struct __tuple_3D__ {
    T x; T y; T z;

    // Constructors
    __tuple_3D__()
    {
        this->x = static_cast<T>(0.0);
        this->y = static_cast<T>(0.0);
        this->z = static_cast<T>(0.0); }
    __tuple_3D__(const T& x_in, const T& y_in, const T& z_in)
    {
        this->x = x_in;
        this->y = y_in;
        this->z = z_in;
    }
    __tuple_3D__(const __tuple_3D__& other)
    {
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;
    }

    // Some operator overloads for cartessian tuple struct

    // access operator (0)
    T operator() (const short& index) const
    {
        if (index > 1 || index < 0) {
            std::cerr << "__tuple_3D__: given index exceeds tuple elements!!" << std::endl;
            exit(-1);
        }

        switch (index) {
        case 0:
            return this->x;
        case 1:
            return this->y;
        case 2:
            return this->z;
        }
    }

    // +
    __tuple_3D__ operator+(const __tuple_3D__& other)
    {
        x = x+other.x; y = y+other.y; z = z+other.z;
        return *this;
    }
    // +=
    __tuple_3D__& operator+=(const __tuple_3D__& other)
    {
        x = x+other.x; y = y+other.y; z = z+other.z;
        return *this;
    }
    // + with scalar
    __tuple_3D__ operator+(const T& other_scalar) const
    {
        return __tuple_3D__{x+other_scalar, y+other_scalar, z+other_scalar};
    }
    // += with scalar
    __tuple_3D__& operator+=(const T& other_scalar) const
    {
        return __tuple_3D__{x+other_scalar, y+other_scalar, z+other_scalar};
    }
    // -
    __tuple_3D__ operator-(const __tuple_3D__& other)
    {
        x = x-other.x; y = y-other.y; z = z-other.z;
        return *this;
    }
    // -=
    __tuple_3D__& operator-=(const __tuple_3D__& other)
    {
        x = x-other.x; y = y-other.y; z = z-other.z;
        return *this;
    }
    // - with scalar
    __tuple_3D__ operator-(const T& other_scalar) const
    {
        return __tuple_3D__{x-other_scalar, y-other_scalar, z-other_scalar};
    }
    // -= with scalar
    __tuple_3D__& operator-=(const T& other_scalar) const
    {
        return __tuple_3D__{x-other_scalar, y-other_scalar, z-other_scalar};
    }
    // *
    __tuple_3D__ operator*(const __tuple_3D__& other)
    {
        x = x*other.x; y = y*other.y; z = z*other.z;
        return *this;
    }
    // *=
    __tuple_3D__& operator*=(const __tuple_3D__& other)
    {
        x = x*other.x; y = y*other.y; z = z*other.z;
        return *this;
    }
    // * with scalar
    __tuple_3D__ operator*(const T& other_scalar) const
    {
        return __tuple_3D__{x*other_scalar, y*other_scalar, z*other_scalar};
    }
    // *= with scalar
    __tuple_3D__& operator*=(const T& other_scalar) const
    {
        return __tuple_3D__{x*other_scalar, y*other_scalar, z*other_scalar};
    }
    // /
    __tuple_3D__ operator/(const __tuple_3D__& other)
    {
        x = x/other.x; y = y/other.y; z = z/other.z;
        return *this;
    }
    // /=
    __tuple_3D__& operator/=(const __tuple_3D__& other)
    {
        x = x/other.x; y = y/other.y; z = z/other.z;
        return *this;
    }
    // / with scalar
    __tuple_3D__ operator/(const T& other_scalar) const
    {
        return __tuple_3D__{x/other_scalar, y/other_scalar, z/other_scalar};
    }
    //const __tuple_3D__& operator/(const T& other_scalar) const
    //{ return (*this)/other_scalar; }
    // /= with scalar
    __tuple_3D__& operator/=(const T& other_scalar) const
    {
        return __tuple_3D__{x/other_scalar, y/other_scalar, z/other_scalar};
    }
    // ==
    bool operator==(const __tuple_3D__& other) const
    {
        if (x == other.x && y == other.y && z == other.z)
            return true;
        else return false;
    }
    // !=
    bool operator!=(const __tuple_3D__& other) const
    {
        if (x == other.x && y == other.y && z == other.z)
            return false;
        else return true;
    }

    // distance calculator
    T dist(const __tuple_3D__& other) const
    {
        T x_dist = other.x - x;
        T y_dist = other.y - y;
        T z_dist = other.z - z;
        return sqrt( x_dist*x_dist + y_dist*y_dist + z_dist*z_dist );
    }

    // direction to other __tuple_3D__
    __tuple_3D__ direction(const __tuple_3D__& other) const
    {
        return __tuple_3D__{
            other.x - x,
            other.y - y,
            other.z - z }._norm();
    }

    // take acos to self
    //
    __tuple_3D__ _acos() const
    {
        return __tuple_3D__{acos(x), acos(y), acos(z)};
    }

    // Return 2D part
    __tuple__<T> getXY() const
    {
        return __tuple__<T>{this->x, this->y};
    }
    __tuple__<T> getYX() const
    {
        return __tuple__<T>{this->y, this->x};
    }
    __tuple__<T> getXZ() const
    {
        return __tuple__<T>{this->x, this->z};
    }
    __tuple__<T> getZX() const
    {
        return __tuple__<T>{this->z, this->x};
    }
    __tuple__<T> getYZ() const
    {
        return __tuple__<T>{this->y, this->z};
    }
    __tuple__<T> getZY() const
    {
        return __tuple__<T>{this->z, this->y};
    }

    // () versions
    __tuple__<T> operator() (const short& i, const short& j) const
    {
        if ((i < 0 || i > 2) || (j < 0 || j > 2)) {
            std::cerr \
                << "__tuple_3D__: Out of index range!!" \
                << "i: " << i << " j: " << j \
                << std::endl;
            exit(-1);
        }

        __tuple__<T> ret_tuple( *this(i), *this(j) );
        return ret_tuple;
    }

    // abs!
    T _abs() const
    { return sqrt(x*x + y*y + z*z); }

    // norm (unit vector)
    __tuple_3D__ _norm() const
    {
        T absolute_val = this->_abs();
        __tuple_3D__ ret_tuple{ x / absolute_val, y / absolute_val, z / absolute_val };
        return ret_tuple;
    }

    // Text output
    std::string to_str() const
    {
        //auto str_x = std::to_string(x);
        //auto str_y = std::to_string(y);
        //auto str_z = std::to_string(z);
        //std::string ret_str = "x: " + str_x + ", y: " + str_y + ", z: " + str_z;
        //return ret_str;

        std::stringstream ss;
        ss.str("");
        ss << std::scientific << std::setprecision(6);
        ss << "x: " << x << ", y: " << y << ", z: " << z;
        return ss.str();
    }

    // checking NaNs..
    short _isnan() const
    {
#ifdef __MULTIPRECISION__
        if ( (boost::math::isnan)(x) )  return 1;
        if ( (boost::math::isnan)(y) )  return 2;
        if ( (boost::math::isnan)(z) )  return 3;
#else
        if (std::isnan(x)) return 1;
        if (std::isnan(y)) return 2;
        if (std::isnan(z)) return 3;
#endif
        return 0;
    }
};

/**
 *
 * Determine crossing between two points.
 *
 * 3D implementation uses two 2D line crossing
 * determination functions.
 *
**/
// 2D crossing determination
template <typename T>
bool fp_chk_cross_2D(
    const __tuple__<T>& A,
    const __tuple__<T>& B,
    const __tuple__<T>& line_rng,
    const T& line_loc)
{
    if ( (fp_lteq<T>(A.x, line_loc) && fp_lteq<T>(line_loc, B.x)) || \
        (fp_lteq<T>(B.x, line_loc) && fp_lteq<T>(line_loc, A.x)) ) {

        // Slope
        T slope = (B.y - A.y)/(B.x - A.x);
        // f(y') = a(x' - x0) + y0
        if (fp_equal<T>(slope, static_cast<T>(0.0))) return true;
        else {
            T cross_pt = slope*(line_loc - A.x) + A.y;
            return \
                ( fp_lteq<T>(line_rng.x, cross_pt) && \
                    fp_lteq<T>(cross_pt, line_rng.y) );
        }
    }
    else {
        // std::cerr \
        //     << "fp_chk_cross_2D: given line_loc: " \
        //     << line_loc \
        //     << " is not between " \
        //     << A.x \
        //     << " and " \
        //     << B.x \
        //     << "." \
        //     << std::endl << "Returning false!!" << std::endl;
        return false;
    }
}

// 3D plane crossing determination
// plane_axis only accepts "xy", "xz", "yz"
template <typename T>
bool fp_chk_cross_3D(
    const __tuple_3D__<T>& start,
    const __tuple_3D__<T>& end,
    const std::string& plane_axis,
    const __tuple__<T>& plane_rng_A,
    const __tuple__<T>& plane_rng_B,
    const T& plane_loc)
{
    __tuple__<T> start_A{ static_cast<T>(0.0), static_cast<T>(0.0) };
    __tuple__<T> end_A{ static_cast<T>(0.0), static_cast<T>(0.0) };
    __tuple__<T> start_B{ static_cast<T>(0.0), static_cast<T>(0.0) };
    __tuple__<T> end_B{ static_cast<T>(0.0), static_cast<T>(0.0) };

    // Initializing start and end coordinates.
    //
    //----------------------------------------------------------
    //| Plane | Estimation axis set A | Estimation axis set B  |
    //----------------------------------------------------------
    //|  x,y  | x,z                   | y,z                    |
    //|  x,z  | x,y                   | y,z                    |
    //|  y,z  | x,y                   | x,z                    |
    //----------------------------------------------------------
    //

    if (plane_axis == "xy") {
        // main axis: z
        start_A = start.getZX();
        end_A   = end.getZX();
        start_B = start.getZY();
        end_B   = end.getZY();
        return (fp_chk_cross_2D(start_A, end_A, plane_rng_A, plane_loc) &&
            fp_chk_cross_2D(start_B, end_B, plane_rng_B, plane_loc));
    }
    else if (plane_axis == "xz") {
        // main axis: y
        start_A = start.getYX();
        end_A   = end.getYX();
        start_B = start.getYZ();
        end_B   = end.getYZ();
        return (fp_chk_cross_2D(start_A, end_A, plane_rng_A, plane_loc) &&
            fp_chk_cross_2D(start_B, end_B, plane_rng_B, plane_loc));
    }
    else if (plane_axis == "yz") {
        // main axis: x
        start_A = start.getXY();
        end_A   = end.getXY();
        start_B = start.getXZ();
        end_B   = end.getXZ();
        return (fp_chk_cross_2D(start_A, end_A, plane_rng_A, plane_loc) &&
            fp_chk_cross_2D(start_B, end_B, plane_rng_B, plane_loc));
    }
    else return false;
}

/**
 *
 * Cartesian coordinate start-end struct
 *
**/
template <typename T>
struct __tuple_3D_start_end__{
    T x_start;
    T x_end;
    T y_start;
    T y_end;
    T z_start;
    T z_end;

    // Range access functions
    __tuple__<T> XRng()
    {
        return __tuple__<T>{x_start, x_end};
    }
    __tuple__<T> YRng()
    {
        return __tuple__<T>{y_start, y_end};
    }
    __tuple__<T> ZRng()
    {
        return __tuple__<T>{z_start, z_end};
    }
    const __tuple__<T> XRng() const
    {
        return __tuple__<T>{x_start, x_end};
    }
    const __tuple__<T> YRng() const
    {
        return __tuple__<T>{y_start, y_end};
    }
    const __tuple__<T> ZRng() const
    {
        return __tuple__<T>{z_start, z_end};
    }

    // general access operator
    T operator() (const short& i)
    {
        if (i < 0 || i > 5) {
            std::cerr \
                << "__tuple_3D_start_end__: Index out of range!!" \
                << std::endl;
            exit(-1);
        }
        switch (i) {
        case 0:
            return x_start;
        case 1:
            return x_end;
        case 2:
            return y_start;
        case 3:
            return y_end;
        case 4:
            return z_start;
        case 5:
            return z_end;
        }
    }
    // const version
    const T operator() (const short& i) const
    {
        return (*this)(i);
    }

};

/**
 *
 * Custom fp_t or fp_int_t to string conversions.
 *
**/
template <typename T>
std::string fp_t_to_string(const T& input_fp_t)
{
    std::stringstream ss;
    ss << input_fp_t;

    return ss.str();
}

/**
 *
 * Random number generator for distance zero case.
 *
 * --> be sure to initialize random seed before using them.
 *
**/
f_t fp_rand();
fp_t fpt_rand();

// random number min to max
template <typename T>
T uniform_rand(const T& min, const T& max)
{
    if (fp_mt<T>(min, max)) \
        return \
            static_cast<T>(
                fp_rand())*(min-max)-(min-max)/static_cast<T>(2.0);
    else
        return \
            static_cast<T>(
                fp_rand())*(max-min)-(max-min)/static_cast<T>(2.0);
}

// basically the same thing.
template <typename T>
T fp_randr(const T& min, const T& max)
{
    return uniform_rand<T>(min, max);
}

// random number 0 to max
template <typename T>
T fp_randn(const T& max)
{
    return static_cast<T>(fp_rand()) * max;
}

#endif /* Include guard */
