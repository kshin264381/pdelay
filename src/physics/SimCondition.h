/**
 *
 * SimCondition.h
 *
 * Contains constants on simulation condition.
 *
 * Written by Taylor Shin
 * Sep. 7th, 2016
 *
**/

#ifndef __SimCondition_h__
#define __SimCondition_h__



#include "fputils.h"
#include "physical_constants.h"

namespace Physics {


/* The Simulation condition class */
class SimCondition {

public:
	// Temperature in (K)
	fp_t temperature;

    // Some physical constants...

    // Constructors and destructors
    SimCondition() : \
    	temperature(static_cast<fp_t>(300.0))
    {;}

    virtual ~SimCondition() {;}


}; /* class SimCondition */



}; /* using namespace Physics */

#endif