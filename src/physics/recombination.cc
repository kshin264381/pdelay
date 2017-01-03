/**
 *
 * recombination.cc
 *
 * Includes some physical equations for recombination rate evaluation.
 *
 * Implementation part.
 *
 * Written by Taylor Shin
 * June 22th, 2016
 *
**/

#include <cmath>
#include <cstdlib>

#include "physical_constants.h"
#include "recombination.h"

using namespace Physics;

/**
 *
 * Private stuff
 *
**/

/**
 * Setting up intrinsic carrier concentration. (cm/3)
**/
void Recombination::set_intrinsic()
{
    this->n_i = this->MaterialData.GetSemi(this->material, "N_I");
}

/**
 *
 * Simple recombination model
 *
 * Un = n_p - n_p0 / tau_n (n-type)
 *    or p_n - p_n0 / tau_p (p-type)
 *
 * tau_n/tau_p : carrier lifetime of electron/hole
 *
**/
fp_t Recombination::simple(
    cfp_t& n_conc, cfp_t& p_conc,
    const char* type)
{
    /* Intrinsic case: simply no net recombination rate */
    cfp_t n_i = this->n_i;
    if ( fp_lteq(n_conc, n_i) && this->is_ntype(type) )
        return static_cast<fp_t>(0.0);
    if ( fp_lteq(p_conc, n_i) && this->is_ptype(type) )
        return static_cast<fp_t>(0.0);

    if ( this->is_ntype(type) ) {
        auto tau = \
            this->MaterialData.GetSemi(this->material, "TAUN0");
        return (n_conc - n_i)/tau;
    }
    else if ( this->is_ptype(type) ) {
        auto tau = \
            this->MaterialData.GetSemi(this->material, "TAUP0");
        return (p_conc - n_i)/tau;
    }

    return static_cast<fp_t>(0.0);
}

/**
 *
 * Trap assisted (Shochkley-Read-Hall)
 *
 * R_srh = (p*n-n_i^2)/(taup0[n+n_i*exp(ETRAP/kTL)+taun0[p+n_i*exp(ETRAP/kTL)])
 *
 * ETRAP = 0.0 by default. difference between trap energy and fermi level.
 *
**/
fp_t Recombination::trap(cfp_t& n_conc, cfp_t& p_conc, cfp_t& e_trap)
{
    auto denum = n_conc*p_conc - this->n_i*this->n_i;

    auto taup0 = this->MaterialData.GetSemi(this->material, "TAUP0");
    auto taun0 = this->MaterialData.GetSemi(this->material, "TAUN0");

    auto num = \
        taup0*(n_conc+this->n_i*exp(e_trap/(k_B_eV*this->temperature))) + \
        taun0*(p_conc+this->n_i*exp(-1.0*e_trap/(k_B_eV*this->temperature)));

    return denum/num;
}

/* Same as trap but includes doping concentration factor */
fp_t Recombination::ctrap(cfp_t& n_conc, cfp_t& p_conc, cfp_t& dop_conc, cfp_t& e_trap)
{
    fp_t denum = n_conc*p_conc - this->n_i*this->n_i;

    fp_t taup0 = this->MaterialData.GetSemi(this->material, "TAUP0");
    fp_t taun0 = this->MaterialData.GetSemi(this->material, "TAUN0");
    fp_t an    = this->MaterialData.GetSemi(this->material, "AN");
    fp_t bn    = this->MaterialData.GetSemi(this->material, "BN");
    fp_t cn    = this->MaterialData.GetSemi(this->material, "CN");
    fp_t en    = this->MaterialData.GetSemi(this->material, "EN");
    fp_t ap    = this->MaterialData.GetSemi(this->material, "AP");
    fp_t bp    = this->MaterialData.GetSemi(this->material, "BP");
    fp_t cp    = this->MaterialData.GetSemi(this->material, "CP");
    fp_t ep    = this->MaterialData.GetSemi(this->material, "EP");
    fp_t nsrhn = this->MaterialData.GetSemi(this->material, "NSRHN");
    fp_t nsrhp = this->MaterialData.GetSemi(this->material, "NSRHP");

    fp_t taun = taun0/(an+bn*(dop_conc/nsrhn) + cn*pow(dop_conc/nsrhn, en));
    fp_t taup = taup0/(ap+bp*(dop_conc/nsrhp) + cp*pow(dop_conc/nsrhp, ep));

    fp_t num = \
        taup*(n_conc+this->n_i*exp(e_trap/(k_B_eV*this->temperature))) + \
        taun*(p_conc+this->n_i*exp(-1.0*e_trap/(k_B_eV*this->temperature)));

    return denum/num;
}

/**
 *
 * Auger recombination
 *
 * auger_n*(p*n^2 - n*n_i^2) + auger_p*(n*p^2-p*n_i^2)
 *
**/
fp_t Recombination::auger(cfp_t& n_conc, cfp_t& p_conc)
{
    fp_t augn = this->MaterialData.GetSemi(this->material, "AUGN");
    fp_t augp = this->MaterialData.GetSemi(this->material, "AUGP");

    fp_t auger_recomb = \
        augn*(p_conc*n_conc*n_conc - n_conc*this->n_i*this->n_i) + \
        augp*(n_conc*p_conc*p_conc - p_conc*this->n_i*this->n_i);

    return auger_recomb;
}

/**
 *
 * Utilities
 *
**/
// checking type from const char
bool Recombination::is_ntype(const char* type_txt)
{
    char type[] = "n";
    char typeCap[] = "N";

    if ( (strcmp(type_txt, typeCap) == 0) || \
        (strcmp(type_txt, type) == 0) )
        return true;
    else
        return false;
}
bool Recombination::is_ptype(const char* type_txt)
{
    char type[] = "p";
    char typeCap[] = "P";

    if ( (strcmp(type_txt, typeCap) == 0) || \
        (strcmp(type_txt, type) == 0) )
        return true;
    else
        return false;
}



/**
 *
 * Public Stuff
 *
 *
**/


