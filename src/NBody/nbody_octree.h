/**
 *
 * nbody_octree.h
 *
 * Header for octal-tree N-Body algorithm...
 * adopted from Barnes-Hut
 * http://www.ifa.hawaii.edu/~barnes/treecode/treeguide.html
 *
 * Written by Taylor Shin
 * Sep. 20th, 2016
 *
**/

#ifndef __nbody_octree_h__
#define __nbody_octree_h__

#define _USE_MATH_DEFINES

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
#include "pbar.h"
#include "decor_output.h"
#include "datatype_hash.h"
#include "visual.h"
#include "CTCForce.h"
#include "recombination_nbody.h"
#include "sim_file_io.h"

#include "Octant.h"
#include "BHTree.h"

using namespace boost::math::constants;

// Some OcTree typedefs.
using spOctant = std::shared_ptr<Octant>;
using Octree = BHTree;
using spOctree = std::shared_ptr<Octree>;

/**
 *
 * The NBody class for octal tree division algorithm.
 *
**/
class NBody_Octree : \
    public virtual Physics::CTCForce, \
    public virtual Physics::Recombination_NBody, \
    public virtual NBodyFileIO, \
    public virtual NBodyVisual
{

private:
    // The octal tree
    spOctree Tree;

    //
    // Generate Tree from CarrierList
    //
    // Runs Barnes-Hut Tree generation algorithm... or
    // something similar...
    //
    int MakeTree();
    int InsertToTree(const spCarrier& carrier);
    spOctant FirstOctant;
    void InitFirstQctant();

    // Processes for OpenMP
    int processes;

    // alpha threashold
    // --> ratio of Octant length / distance to other body.
    //
    fp_t alpha;

protected:
    // Initialize simulation
    int SimInit();

    // Simulation control parameter
    // true -> reads in db format
    // false -> reads in csv format
    //
    bool continued;

    // Kick: calculates force and velocity to all
    // carriers.
    int kick_sub(const uint64_t& istart, const uint64_t& ipoints, const fp_t& delta_t);
    int Kick(const fp_t& delta_t);
    int Kick();

    // Drift: calculates displacements to all
    // carriers.
    int Drift(const fp_t& delta_t);
    int Drift();

    // Select: calculates delta t
    void Select();
    void Select(const fp_t& tau);

    // input tar filename
    std::string input_data_filename;

    // Output data file format
    unsigned int log_carrier_data_format;

    // Simulation status output
    DecorOutput SimOutput;
    void SimFinishMessage();
    void ShowSimStatus();

    // Simulation time monitors.
    std::clock_t current_sim_time;
    std::clock_t start_time;

    // Pass force calculation for the very first loop if read from
    // Carrier database. (Continuation case)
    bool pass_forcecal;

    // Methods for Tree Force calculation
    void TreeUpdateCForce(spOctree tree, spCarrier carrier);
    void TreeUpdateDForce(spCarrier carrier);

    // Methods for Drift.
    void update_carr_position(spCarrier& carrier);
    void update_carr_position(spCarrier& carrier, const fp_t& tau);
    void update_all_carr_position_sub(
        uint64_t& istart, uint64_t& ipoints, const fp_t& tau);
    void update_all_carr_position(const fp_t& tau);

public:
    // N-Body calculation algorithms
    int Run();
    int RunSingleShot();
    int RunSKDK();
    int RunSDKD();

    template<typename T>
    std::string to_str(const T& some_stuff)
    {
        std::stringstream ss;
        ss << some_stuff;
        return ss.str();
    }

    // Delta_t evaluator.
    void SetVariableDeltaT();
    // Returns variable delta_t algorithm
    void UnsetDeltaT()
    {
        this->SetVariableDeltaT();
        this->forced_delta_t = false;
    }

    // Set up carrier data visualization data format
    void SetCarrierDataFormat(unsigned int N)
    {
        this->log_carrier_data_format = N;
    }

    // Constructors and Destructors
    NBody_Octree() : \
        continued(false),
        Tree(nullptr),
        input_data_filename({}),
        pass_forcecal(false),
        alpha(FP_T(0.5))
    {
        spOctant spCV = std::make_shared<Octant>(Octant({}));
        //Octree OcTreeInit = Octree(0, spCV, true);
        //Tree = std::make_shared<Octree>(OcTreeInit);
    }
    
    
    NBody_Octree(const char* filename, bool continued = false) : \
        NBody_Octree()
    {
        this->input_data_filename = std::string(filename);
        this->continued = continued;
    }

    // A correct constructor.
    NBody_Octree(
        const char* csv_file,
        Box dimension,
        Bias extBias,
        fp_t doping_conc) : \
        NBody_Octree()
    {
        this->container_file = csv_file;
        this->silicon_dimension = std::make_unique<Box>(dimension);
        this->InitialData = std::make_unique<ReadData>(csv_file);
        this->ExtBias = std::make_unique<Bias>(extBias);
        this->set_logfile_name();
        this->doping = doping_conc;
    }

    // With temperature
    NBody_Octree(
        const char* csv_file,
        Box dimension,
        Bias extBias,
        fp_t doping_conc,
        fp_t temperature) : \
        NBody_Octree(csv_file, dimension, extBias, doping_conc)
    {
        this->temperature = temperature;
        this->DetMaterial.SetTemp(temperature);
    }

    // Aaaand with process numbers
    NBody_Octree(
        const char* csv_file,
        Box dimension,
        Bias extBias,
        fp_t doping_conc,
        fp_t temperature,
        unsigned int cpu_num) : \
        NBody_Octree(csv_file, dimension, extBias, doping_conc, temperature)
    {
        this->DetMaterial.SetTemp(temperature);
        this->processes = cpu_num;
    }

    // Aaaaaaaaaaaand with material database file.
    NBody_Octree(
        const char* csv_file,
        Box dimension,
        Bias extBias,
        fp_t doping_conc,
        fp_t temperature,
        unsigned int cpu_num,
        const char* material_db_file) : \
        NBody_Octree(
            csv_file, dimension, extBias, doping_conc, temperature, cpu_num)
    {
        this->DetMaterial = Materials::MatData(material_db_file);
        this->DetMaterial.SetTemp(temperature);
        this->NBodyRecom = Physics::Recombination(this->DetMaterial);
    }

    // Also, able to continue the simulation from saved sqlite3 db file.
    NBody_Octree(
        const char* db_file,
        Box dimension,
        Bias extBias,
        fp_t doping_conc,
        fp_t temperature,
        unsigned int cpu_num,
        const char* material_db_file,
        bool continue_sim) : NBody_Octree()
    {
        this->db_file = db_file;
        this->silicon_dimension = std::make_unique<Box>(dimension);
        this->ExtBias = std::make_unique<Bias>(extBias);
        this->set_logfile_name();
        this->DetMaterial = Materials::MatData(material_db_file);
        this->DetMaterial.SetTemp(temperature);
        this->NBodyRecom = Physics::Recombination(this->DetMaterial);
        this->continued = continue_sim;
        this->doping = doping_conc;
    }

    virtual ~NBody_Octree()
    {;}

}; /* class NBody_Octree */





#endif /* Include guard */
