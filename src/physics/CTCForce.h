/**
 *
 * CTCForce.h
 *
 * Defines methods for Carrier to carrier interaction
 * force estimation.
 *
 * Written by Taylor Shin
 * Sep. 2nd, 2016
 *
**/

#ifndef __CTCForce_h__
#define __CTCForce_h__

#define _USE_MATH_DEFINES

#include "fputils.h"
#include "physical_constants.h"
#include "typedefs.h"
#include "materials.h"
#include "sim_space.h"

namespace Physics {

class CTCForce : \
    public virtual sim_space
{
public:
    // Returns effective mass of electron/holes of silicon.
    // fp_t eff_mass_si(spCarrier carrier);
    // Returns thermal velocity with effective mass
    Vel thermal_vel(const spCarrier& carrier);

    // Calculate Debye length for a carrier
    fp_t DebyeLength(const spCarrier& carrier);

    // Diffuse a Carrier by given tau
    void Diffusion(const spCarrier& carrier);
    void Diffusion(const spCarrier& carrier, const fp_t& tau);

    // Calculate Brownian scattering for a carrier
    void MFPAdj(spCarrier& carrier);
    void MFPAdj(spCarrier& carrier, const fp_t& tau);

    // Drift force (Electric field from external bias)
    Force DriftForce(const spCarrier& carrier);

    // Carrier to Carrier interaction.
    Force CoulombForce(spCarrier& carrier, spCarrier& other);

	// Constructors and Destructors
	CTCForce() {;}
	virtual ~CTCForce() {;}

}; /* class CTCForce */

}; /* namespace Physics */


#endif /* Include guard */