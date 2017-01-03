/**
 *
 * CTCForce.cc
 *
 * Implementation for Carrier to carrier interaction
 * force estimation.
 *
 * Written by Taylor Shin
 * Sep. 2nd, 2016
 *
**/

#include "CTCForce.h"

using namespace Physics;

// Returns thermal velocity
//
// Vth = sqrt(3*k_B*T/m*)
//
Vel CTCForce::thermal_vel(const spCarrier& carrier)
{
    // The thermal velocity comes from...
    //
    // E_therm = 3/2 * k_B * temperature
    // E_kinetic = 1/2 * mass * velocity^2
    //
    // Since E_therm == E_kinetic....
    //
    // vel_therm = sqrt(3.0 * k_B * temperature / mass) (m/s)
    //
    fp_t vel_therm = \
        v_therm(this->temperature, carrier->GetMass());

    // Implementing it into a random 3D vector.
    fp_t phi = fp_randn(2.0) * static_cast<fp_t>(M_PI);
    fp_t costheta = uniform_rand(-1.0, 1.0);
    fp_t theta = acos(costheta);
    Vel output_Vel = \
        Vel{
            sin(theta)*cos(phi),
            sin(theta)*sin(phi),
            cos(theta)
        } * vel_therm;
    // note that the return value is in the unit of (m/s)
    return output_Vel;
}

// Extract the Debye length of current material.
// TODO: Update this part if TCAD routine has implemented.
//
fp_t CTCForce::DebyeLength(const spCarrier& carrier)
{
    // Doping concentration (cm^(-3) = 1/10000^3 um^-3)
    //fp_t doping = 1.3e+12 / pow(1e4, 3);

    // Thermal voltage : 2D function from physical_constants.h
    fp_t Vt = volt_therm(this->temperature);

    // The Debye length
    //
    // debye_length = sqrt(eps_si*Vt / (q_h*doping));
    //
    // returns Debye length as mks unit. (m)
    //
    return static_cast<fp_t>(
        sqrt(this->DetMaterial.GetSemi("Silicon", "EPS")*eps_0*Vt / \
        fabs( carrier->GetCharge()* \
            ( this->doping * pow(100.0, 3.0) ) ) ) );
}

// Adjust position of a carrier from Brownian motion.
//
void CTCForce::MFPAdj(spCarrier& carrier)
{
    this->MFPAdj(carrier, this->delta_t);
}

// Adjust position of a carrier from Brownian motion.
// (with given time)
void CTCForce::MFPAdj(spCarrier& carrier, const fp_t& tau)
{
    // Obtain 3D thermal velocity.
    //
    fp_t mean_free_time = uniform_rand<fp_t>(1e-14, 1e-13);
    //
    // thermal velocity * meal_free_time --> random movement.
    //
    // Simulates brownian motion within this->delta_t period.
    //
    int brownian_motions = \
        static_cast<int>(
            round(tau / mean_free_time));
    if (brownian_motions > 0) {
        for (int i = 0; i<brownian_motions; ++i) {
            carrier->AdjPosDelta(
                this->thermal_vel(carrier)*this->len_scale_f*mean_free_time);
        }
    }

    return;
}

// Parallel plate electric field (Drift) (Returns N)
//
// TODO: Change it if TCAD has been implemented.
//
Force CTCForce::DriftForce(const spCarrier& carrier)
{
    fp_t z_bias = this->ExtBias->x - this->ExtBias->y;

    // Currently, we only consider a parallel plate through z-axis. (m)
    fp_t thickness = \
        (this->silicon_dimension->z_end - \
            this->silicon_dimension->z_start) / \
        this->len_scale_f;

    // Electric field in V/m
    /* auto carr_efield = EField{
        static_cast<fp_t>(0.0),
        static_cast<fp_t>(0.0), z_bias/thickness };
    */

    // F = qE (returns mks unit of N)
    // return carr_efield * carrier->GetCharge();
    return EField{
        static_cast<fp_t>(0.0),
        static_cast<fp_t>(0.0),
        static_cast<fp_t>(z_bias/thickness)
    } * carrier->GetCharge();
}

// Coulomb force calculation (returns MKS)
Force CTCForce::CoulombForce(spCarrier& carrier, spCarrier& other)
{
    // Safeguard... if carrier == other... then just return dummy.
    if (carrier->GetPos() == other->GetPos())
        return ZeroForce;

    // Distance between current carrier and other (as mks unit.)
    // Note that the carrier position is stored in (um) so we are 
    // converting it to MKS unit.
    fp_t distance = \
        carrier->GetPos().dist(other->GetPos()) / this->len_scale_f;

    // Handles debye length 
    // --> Coulomb force is not relevant if the distance between
    // carriers are closer than Debye Length. (MKS)
    fp_t debye_length = this->DebyeLength(carrier);
    if ( fp_lteq<fp_t>(distance, debye_length) )
        return ZeroForce;

    // Calculating force from the Carrier 'other'
    // Note that the distance is in um, so adding up 1e-6
    // multiplication factor to result MKS unit: (N).
    fp_t force = \
        k_e*carrier->GetCharge()*other->GetCharge()/(distance*distance);

    // Normalized direction vector.
    Force f_direction = \
        carrier->GetPos().direction(other->GetPos());

    return f_direction * static_cast<fp_t>(force);
}


