/**
 * fputils.cc
 *
 * Floating point datatype utilities.
 *
 * Don't use those utility functions with integers, strings 
 * pointers and many magical stuffs other than:
 * float, double, long double. 
 *
 * Implementations!!
 *
 * Written by Taylor Shin
 * Feb. 25th, 2016
 *
**/

#include "fputils.h"

/**
 *
 * Some RNG tools
 *
**/
f_t fp_rand()
{
    return F_T(rand()) / F_T(RAND_MAX);
}

fp_t fpt_rand()
{
    return FP_T(rand()) / FP_T(RAND_MAX);
}