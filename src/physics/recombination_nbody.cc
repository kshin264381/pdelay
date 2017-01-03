/**
 *
 * recombination_nbody.cc
 *
 * Recombination rate calculation methods for NBody.
 *
 * Written by Taylor Shin
 * Sep. 7th, 2016
 *
**/

#include "recombination_nbody.h"


using namespace Physics;


// Trap assisted recombination
//
void Recombination_NBody::TrapAssistedRecombination()
{
    // change here according to substrate type. 
    // it's p-type at this moment.
    //
    auto vol_mat = this->GetVolume();
    auto n_i = this->DetMaterial.GetSemi("Silicon", "N_I");
    fp_t n_conc, p_conc;
    n_conc = \
        static_cast<fp_t>(this->num_elec)/vol_mat \
        + (n_i*n_i)/this->doping;
    p_conc = \
        static_cast<fp_t>(this->num_hole)/vol_mat + \
        this->doping;
    this->recomb_trap = \
        this->NBodyRecom.TrapAssist(
            n_conc, p_conc, static_cast<fp_t>(0.0));

    return;
}

// Auger recombination
//
void Recombination_NBody::AugerRecombination()
{
    auto vol_mat = this->GetVolume();
    auto n_i = \
        this->DetMaterial.GetSemi("Silicon", "N_I");
    auto n_conc = \
        static_cast<fp_t>(this->num_elec)/vol_mat \
        + (n_i*n_i)/this->doping;
    auto p_conc = \
        static_cast<fp_t>(this->num_hole)/vol_mat \
        + this->doping;
    this->recomb_auger = \
        this->NBodyRecom.Auger(n_conc, p_conc);

    return;
}

// Substrate recombination
//
void Recombination_NBody::SubstrateRecombination()
{
    this->TrapAssistedRecombination();
    this->AugerRecombination();

    // std::cout << "Trap Recomb: " << this->recomb_trap << std::endl;
    // std::cout << "Auger Recomb: " << this->recomb_auger << std::endl;

    this->recomb_total = \
        this->recomb_trap+this->recomb_auger;

    return;
}

// Handles recombination
//
void Recombination_NBody::BulkRecombination()
{
    this->SubstrateRecombination();
    auto vol_mat = this->GetVolume();
    auto num_of_recombined_carr = \
        static_cast<uint64_t>(
            round(this->recomb_total*vol_mat*this->delta_t));

    auto carr_it = std::begin(this->Carriers);
    auto num_carrs = \
        static_cast<uint64_t>(this->Carriers.size());
    std::cout << "*** Recombination Rate: " \
        << num_of_recombined_carr \
        << " /current volume /" << this->delta_t \
        << " second(s) " \
        << " ***" << std::endl;
    if (num_of_recombined_carr >= num_carrs) {
        std::cerr << "Lost all carriers in the void!!" \
            << std::endl << std::endl;
        for (carr_it;
            carr_it!=std::end(this->Carriers); ++carr_it) {
            this->add_to_rem(carr_it->second);
            this->write_mat_recomb_carrier_info(carr_it->second);
        }
    }
    else {
        for (auto i=0; i<num_of_recombined_carr; ++i) {
            carr_it = std::begin(this->Carriers);
            std::advance(carr_it, rand() % num_of_recombined_carr);
            this->add_to_rem(carr_it->second);
            this->write_mat_recomb_carrier_info(carr_it->second);
        }
    }

    // Remove carriers as recombination
    for (auto ctr : this->CarriersToRemove) {
        this->remove_carr(ctr);
    }
    this->CarriersToRemove.clear();

    return;
}

// sub funciton of CarrToCarrRecombination (OpenMP)
void Recombination_NBody::c_to_c_sub(
    uint64_t& istart, uint64_t& ipoints)
{
    // auto it = std::begin(this->Carriers);
    // auto end = std::begin(this->Carriers);

    // // Advancing it and end to calculation range.
    // std::advance(it, istart);
    // std::advance(end, istart+ipoints);

    // // Carrier-index, distance pair.
    // std::deque<fp_t> dist_list;
    // std::deque<uint64_t> carr_index_list;

    // // Iteration!!!! --> delete a electron-hole pair which are too close.
    // for (it; it!=end; ++it) {
    //     auto current_carrier = it->second;
    //     // resetting distance lists...
    //     dist_list.clear();
    //     carr_index_list.clear();
    //     auto current_carrier_pos = current_carrier.GetPos();
    //     for (auto it_o=std::begin(this->Carriers);
    //         it_o!=std::end(this->Carriers); ++it_o) {
    //         auto other_carrier = it_o->second;

    //         // Ditch iteration for a few trivial situations
    //         if (current_carrier == other_carrier) continue;
    //         if (current_carrier.GetType() == other_carrier.GetType()) continue;

    //         auto other_carrier_pos = other_carrier.GetPos();
    //         auto distance = current_carrier_pos.dist(other_carrier_pos);

    //         if (distance < LDBL_MIN) {
    //             dist_list.append(distance);
    //             carr_index_list.append(other_carrier.GetIndex());
    //         }
    //     } /* for (auto it_o=std::begin(this->Carriers) */

    //     // Now selecte the closest one for elimination.


    // } /* for (it; it!=end; ++it)  */

    return;
}


// Estimates Carrier to Carrier Recombination
void Recombination_NBody::CarrToCarrRecombination()
{
    // With OpenMP
// #ifdef _OPENMP
//     uint64_t ith, nth, ipoints, istart, npoints;
//     #pragma omp parallel default(shared) private(ith, nth, ipoints, istart)
//     {
//     npoints = this->Carriers.size();
//     ith     = omp_get_thread_num();
//     nth     = omp_get_num_threads();
//     ipoints = npoints / nth;
//     istart = ith * ipoints;
//     if (ith == npoints-1) ipoints = npoints - istart;
//     this->c_to_c_sub(istart, ipoints);

//     } /* #pragma omp parallel default(shared) private(ith, nth, ipoints, istart) */
// #else
//     // Without OpenMP

// #endif /* #ifdef _OPENMP */

    return;
}