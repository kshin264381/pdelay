/**
 *
 * sim_space.h
 *
 * Defines methods for simulation space definition.
 *
 * Written by Taylor Shin
 * Sep. 6th, 2016
 *
**/

#ifndef __sim_space_h__
#define __sim_space_h__

#ifdef _OPENMP
#include <omp.h>
#endif

// Default process number.
#define PROCESS_NUM 4

// Boost filesystem settings
#define BOOST_FILESYSTEM_NO_DEPRECATED

#include "fputils.h"
#include "physical_constants.h"
#include "typedefs.h"
#include "materials.h"
#include "SimCondition.h"
#include "sim_progress.h"
#include "load_carr.h"
#include "readcsv.h"
#include "pbar.h"
#include "decor_output.h"

/**
 * Struct Bias
 *
 * contains bias information. anode and cathode
 *
**/
using Bias   =    __tuple__<fp_t>;

/**
 * Struct Dimension
 *
 * Contains a rectangular box dimension definition.
 *
**/
using Box    =    __tuple_3D_start_end__<fp_t>;
using XY     =    __tuple__<fp_t>;
using YZ     =    __tuple__<fp_t>;
using XZ     =    __tuple__<fp_t>;

class sim_space : \
    public virtual LoadCarrDB,
    public virtual Physics::SimCondition,
    public virtual SimProgress
{
public:
    // vector of Carriers. Populate it with
    // private method generate_carriers
    CarrierList Carriers;
    //CarrierList CarriersToRemove;
    CarrierSet CarriersToRemove;

    // Material dimension (silicon dimension... mostly)
    std::unique_ptr<Box> silicon_dimension;

    // External bias information
    std::unique_ptr<Bias> ExtBias;

    // Doping concentration
    fp_t doping;

    // number of electron/holes
    uint64_t num_elec;
    uint64_t num_hole;

    // length scale
    fp_t len_scale_f;

    // Normalization factor
    Loc NormFactor;

    // Material Info. (Fetch everything from here...)
    Materials::MatData DetMaterial;

    // Count collected n lost carriers
    fp_int_t collected_carriers;
    fp_int_t lost_carriers;

    // Raw data readin from the csv data reader.
    std::unique_ptr<ReadData> InitialData;

    // number of sim processes
    unsigned int processes;

    // Progress Bars
    ProgressBar CarrierReadInProgress;
    ProgressBar ForceCal;
    ProgressBar LocCal;

    // Setting up normalization factor.
    void SetNormFactor(
        const fp_t& nx, const fp_t& ny, const fp_t& nz);

    // populate carriers to remove
    int add_to_rem(const spCarrier& carrier);

    // Returns volume as of cm^3
    fp_t GetVolume();

    // Remove carrier from this->Carriers
    int remove_carr(spCarrier& carrier);
    // search carrier from this->Carriers
    uint64_t search_carr(const spCarrier& carrier, uint64_t& index);
    uint64_t search_rem_carr(const spCarrier& carrier);

    // Determines if a carrier is collectable or not.
    bool is_inside(const spCarrier& carrier);
    bool is_collectable(const spCarrier& carrier, const Loc& prev_pos);

    // Setting up input data (mainly length) unit system.
    void SetInputUnit(quantity<length, fp_t> dimension)
    {
        this->len_scale_f = (meter / dimension).value();
    }

    // delta_t control
    void SetVariableDeltaT();
    // Returns to variable delta_t algorithm
    void UnsetDeltaT()
    {
        this->SetVariableDeltaT();
        this->forced_delta_t = false;
    }

    // Simulation progress report tools (needs Carrier)
    void ShowSimStatus();
    void SimFinishMessage();

    /* Constructors and destructors */
    sim_space() : \
        InitialData(nullptr),
        collected_carriers(0),
        lost_carriers(0),
        processes(PROCESS_NUM),
        silicon_dimension(nullptr),
        ExtBias(nullptr),
        doping(static_cast<fp_t>(0.0)),
        num_elec(0), num_hole(0),
        len_scale_f(1.0),
        DetMaterial(Materials::MatData()),
        NormFactor({1.0, 1.0, 1.0}),
        CarrierReadInProgress(ProgressBar()),
        ForceCal(ProgressBar()),
        LocCal(ProgressBar())
    {;}

    virtual ~sim_space() {;}

};


#endif /* sim_space.h include shield */