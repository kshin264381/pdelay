/**
 *
 * recombination_nbody.h
 *
 * Recombination rate calculation methods for NBody.
 *
 * Written by Taylor Shin
 * Sep. 7th, 2016
 *
**/

#ifndef __recombination_nbody_h__
#define __recombination_nbody_h__

#include "recombination.h"
#include "sim_space.h"
#include "sim_file_io.h"
#include "carrier.h"


namespace Physics {

class Recombination_NBody : \
    public virtual NBodyFileIO,
    public virtual Recombination
{

public:
    fp_t recomb_trap;
    fp_t recomb_auger;
    fp_t recomb_total;

    void TrapAssistedRecombination();
    void AugerRecombination();
    void SubstrateRecombination();
    void BulkRecombination();

    void c_to_c_sub(uint64_t& istart, uint64_t& ipoints);
    void CarrToCarrRecombination();

    // Recombination Object
    Recombination NBodyRecom;

    // Constructors and destructors
    Recombination_NBody() : \
        recomb_trap(static_cast<fp_t>(0.0)),
        recomb_auger(static_cast<fp_t>(0.0)),
        recomb_total(static_cast<fp_t>(0.0)),
        NBodyRecom(Recombination())
    {;}

    virtual ~Recombination_NBody() {;}

}; /* class Recombination_NBody */


}; /* namespace Physics */





#endif /* Include guard */