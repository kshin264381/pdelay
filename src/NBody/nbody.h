/**
 *
 * nbody.h
 *
 * header for N-body algorithm.
 *
 * Written by Taylor Shin
 * Feb. 25th, 2016
 *
**/

#ifndef __nbody_h__
#define __nbody_h__

#define _USE_MATH_DEFINES
#define BOOST_FILESYSTEM_NO_DEPRECATED

#include <cmath>
#include <climits>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <deque>
#include <vector>
#include <set>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/math/constants/constants.hpp>

#include "typedefs.h"
#include "unique_ptr.h"
#include "physical_constants.h"
#include "carrier.h"
#include "datatype_hash.h"
#include "visual.h"
#include "CTCForce.h"
#include "recombination_nbody.h"
#include "sim_file_io.h"

using namespace boost::math::constants;

// Sqlite callback function...
// static int c_callback(void* param, int argc, char **argv, char **azColName);


/**
 * Class NBody
 *
 * Runs NBody simulation.
 *
**/
class NBody : \
    public virtual Physics::CTCForce, \
    public virtual Physics::Recombination_NBody, \
    public virtual NBodyFileIO, \
    public virtual NBodyVisual
{

protected:
   
    // Update force on a carrier
    void update_force(spCarrier& carrier, const fp_t& tau);
    void update_all_force_sub(
        const uint64_t& istart,
        const uint64_t& ipoints,
        const fp_t& tau);
    void update_all_force(const fp_t& tau);

    // Update carrier position
    void update_carr_position(spCarrier& carrier);
    void update_carr_position(spCarrier& carrier, const fp_t& tau);
    // Update position of all carriers
    void update_all_carr_position_sub(
        uint64_t& istart, uint64_t& iprop, const fp_t& tau);
    void update_all_carr_position(const fp_t& tau);

    // Visualization class
    unsigned int log_carrier_data_format;

    // Continues from saved state?
    bool continued;

    // force_calculation_control
    bool pass_forcecal;

    // Initialize simulation depending on input methods.
    int InitSim();

    /**
     *
     * Drift and Kick algorithm
     *
     * Adopted from Time stepping N-body simulations.
     *
    **/
    // Select Algorithm --> basically, a fancy name for 
    // flexible delta_t find algorithm.
    void Select();
    void Select(const fp_t& tau);
    // Drift Algorithm
    int Drift(const fp_t& tau);
    // Kick Algorithm
    int Kick(const fp_t& tau);

public:
    // Run the actual simulation
    int Run(); // Very simple and stupid algorithm
    int RunSDKD(); // Select Drift Kick Drift algorithm
    int RunSKDK(); // Select Kick Drift Kick algorithm

    // Setting up num of processes
    int SetProcesses(const unsigned int& processes)
    {
        this->processes = processes;
        return static_cast<int>(processes);
    }

    // Set up carrier data visualization data format
    void SetCarrierDataFormat(unsigned int N)
    { this->log_carrier_data_format = N; }

    // Constructors
    // A dummy initializer
    NBody() : \
        log_carrier_data_format(0),
        continued(false),
        pass_forcecal(false)
    {
        this->forced_delta_t = false;
        this->delta_t = 0.0;
        this->len_scale_f = static_cast<fp_t>(1e6);
        this->sim_algorithm_str = "(One-To-One)";
    }

    // A correct constructor.
    NBody(
        const char* csv_file,
        Box dimension,
        Bias extBias,
        fp_t doping_conc) : \
        NBody()
    {
        this->container_file = csv_file;
        this->silicon_dimension = std::make_unique<Box>(dimension);
        this->InitialData = std::make_unique<ReadData>(csv_file);
        this->ExtBias = std::make_unique<Bias>(extBias);
        this->set_logfile_name();
        this->doping = doping_conc;
    }

    // With temperature
    NBody(
        const char* csv_file,
        Box dimension,
        Bias extBias,
        fp_t doping_conc,
        fp_t temperature) : \
        NBody(csv_file, dimension, extBias, doping_conc)
    {
        this->temperature = temperature;
        this->DetMaterial.SetTemp(temperature);
    }

    // Aaaand with process numbers
    NBody(
        const char* csv_file,
        Box dimension,
        Bias extBias,
        fp_t doping_conc,
        fp_t temperature,
        unsigned int cpu_num) : \
        NBody(csv_file, dimension, extBias, doping_conc, temperature)
    {
        this->DetMaterial.SetTemp(temperature);
        this->processes = cpu_num;
    }

    // Aaaaaaaaaaaand with material database file.
    NBody(
        const char* csv_file,
        Box dimension,
        Bias extBias,
        fp_t doping_conc,
        fp_t temperature,
        unsigned int cpu_num,
        const char* material_db_file) : \
        NBody(csv_file, dimension, extBias, doping_conc, temperature, cpu_num)
    {
        this->DetMaterial = Materials::MatData(material_db_file);
        this->DetMaterial.SetTemp(temperature);
        this->NBodyRecom  = Physics::Recombination(this->DetMaterial);
    }

    // Also, able to continue the simulation from saved sqlite3 db file.
    NBody(
        const char* db_file,
        Box dimension,
        Bias extBias,
        fp_t doping_conc,
        fp_t temperature,
        unsigned int cpu_num,
        const char* material_db_file,
        bool continue_sim) : NBody()
   {
        this->db_file = db_file;
        this->silicon_dimension = std::make_unique<Box>(dimension);
        this->ExtBias = std::make_unique<Bias>(extBias);
        this->set_logfile_name();
        this->DetMaterial = Materials::MatData(material_db_file);
        this->DetMaterial.SetTemp(temperature);
        this->NBodyRecom  = Physics::Recombination(this->DetMaterial);
        this->continued = continue_sim;
        this->doping = doping_conc;
    }

    // Destructor... does literally nothing since
    // every pointer type is handled by smart pointers...
    virtual ~NBody() 
    {
    }
};

#endif
