/**
 *
 * sim_progress.h 
 *
 * Progress monitoring part for N-Body simulation
 *
 * Written by Taylor Shin
 * Sep. 7th, 2016
 *
**/

#ifndef __sim_progress_h__
#define __sim_progress_h__

#include <ctime>
#include <string>
#include <sstream>

#include "fputils.h"


class SimProgress
{
public:
    fp_t delta_t; // simulation step time in seconds.
    fp_t elapsed_time; // elapsed time in seconds.
    fp_int_t sim_step; // Current simulation step

    bool gen_carr_log; // Generate carrier log or not.

    bool forced_delta_t; // forced delta_t or not

    // methods
    std::string print_elapsed_time();
    std::string format_time(std::clock_t elapsed_sim_time);

    //
    // delta_t control...
    //
    // Implement maximum delta_t possible based on Select algoritm in...
    //
    // Thomas Quinn, Neal Katz, Joachim Stadel, and George Lake,
    // "Time stepping N-body simulations"
    //
    // 
    fp_t nu;

    // Overriding SetDeltaT in SimProgress...
    void SetDeltaT(const fp_t& new_delta_t)
    {
        this->delta_t = new_delta_t;
        this->forced_delta_t = true;
    }

    // Set stability & accuracy constant
    void SetSnAConst(const fp_t& new_nu)
    { this->nu = new_nu; }

    // Set carrier log generation on or off
    void SetCarrLogGen(bool scl)
    { this->gen_carr_log = scl; }

    // Constructors and destructors
    SimProgress() : \
        delta_t(static_cast<fp_t>(0.0)),
    	elapsed_time(static_cast<fp_t>(0.0)),
        gen_carr_log(true),
        forced_delta_t(false),
        sim_step(0),
        nu(0.03)
    {;}
    virtual ~SimProgress()
    {;}
};

#endif