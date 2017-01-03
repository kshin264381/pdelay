/**
 *
 * recombination.h
 *
 * Includes some physical equations for recombination rate evaluation.
 *
 * Written by Taylor Shin
 * June 22th, 2016
 *
**/

#ifndef __RECOMBINATION_H__
#define __RECOMBINATION_H__

#include <cmath>
#include <string>
#include <vector>
#include <sstream>

#include "fputils.h"
#include "materials.h"
#include "SimCondition.h"

namespace Physics {

// Note: recombination rate is (cm/3 /s)
class Recombination : \
    public virtual SimCondition
{
private:
    // Material info storage
    Materials::MatData MaterialData;
    std::string material;

    // Some physical constants
    //
    // intrinsic carrier concentration
    fp_t n_i;
    void set_intrinsic();

    // Simple
    fp_t simple(
        cfp_t& n_conc, cfp_t& p_conc,
        const char* type);

    // Trap assisted (Shochkley-Read-Hall)
    fp_t trap(cfp_t& n_conc, cfp_t& p_conc, cfp_t& e_trap);
    fp_t ctrap(cfp_t& n_conc, cfp_t& p_conc,
        cfp_t& dop_conc, cfp_t& e_trap);

    // Auger
    fp_t auger(cfp_t& n_conc, cfp_t& p_conc);

    // Utility
    // check type
    bool is_ntype(const char* type_txt);
    bool is_ptype(const char* type_txt);

public:
    // Interface functions
    fp_t Simple(
        cfp_t& n_conc, cfp_t& p_conc,
        const char* type)
    { return this->simple(n_conc, p_conc, type); }
    fp_t TrapAssist(cfp_t& n_conc, cfp_t& p_conc, cfp_t& e_trap)
    { return this->trap(n_conc, p_conc, e_trap); }
    fp_t CTrapAssist(cfp_t& n_conc, cfp_t& p_conc, cfp_t& dop_conc, cfp_t& e_trap)
    { return this->ctrap(n_conc, p_conc, dop_conc, e_trap); }
    fp_t Auger(cfp_t& n_conc, cfp_t& p_conc)
    { return auger(n_conc, p_conc); }

    // Constructors and destructors
    // Default constructor (silicon)
    Recombination()
    {
        this->material      = std::string("Silicon");
        this->MaterialData  = Materials::MatData();
        this->temperature   = 300.0;
        this->set_intrinsic();
    }
    Recombination(Materials::MatData& MatInfo)
    {
        this->material      = std::string("Silicon");
        this->MaterialData  = MatInfo;
        this->temperature   = 300.0;
        this->set_intrinsic();
    }
    Recombination(
        Materials::MatData& MatInfo,
        std::string str_material,
        fp_t temp)
    {
        this->MaterialData  = MatInfo;
        this->material      = str_material;
        this->temperature   = temp;
        this->set_intrinsic();
    }
    ~Recombination() {;}

}; /* class Recombination */

}



















#endif /* Include guard */