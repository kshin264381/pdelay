/**
 * physical_constants.cc
 *
 * Physical constant object files... oh well...
 *
 * Method implementations!!
 *
 * Written by Taylor Shin
 * May. 12th, 2016
 *
**/

#include "physical_constants.h"
#include "materials.h"

/**
 * EffMass class implementations
**/
fp_t EffMass::Electron()
{
    return this->MaterialDatabase.GetSemi("Silicon", "MVTHN")*m_elec;
}
fp_t EffMass::Hole()
{
    return this->MaterialDatabase.GetSemi("Silicon", "MVTHP")*m_elec;
}


/**
 * DiffCoeff class implementations
**/
fp_t DiffCoeff::Electron()
{
	return k_B * this->MaterialDatabase.GetTemp() * \
        this->MaterialDatabase.GetSemi("Silicon", "MUN") / q_h;
}
fp_t DiffCoeff::Hole()
{
	return k_B * this->MaterialDatabase.GetTemp() * \
        this->MaterialDatabase.GetSemi("Silicon", "MUP") / q_h;
}


