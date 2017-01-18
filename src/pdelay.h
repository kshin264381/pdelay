/**
 *
 * pdelay.h
 *
 * Main function for the N-body simulation.
 *
 * Written by Taylor Shin
 * Feb. 20th, 2016
 *
**/

#ifndef __pdelay_h__
#define __pdelay_h__

#include <iostream>
#include <string>
#include <ctime>
#include <algorithm>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include "nbody.h"
#include "nbody_octree.h"
#include "unique_ptr.h"
#include "cxxopts.hpp" // https://github.com/jarro2783/cxxopts
#include "visual.h"
#include "Utils.h"

#if defined(_WIN32) || defined(_MSC_VER)
#include <windows.h>
#elif defined(MACOS)
#include <sys/param.h>
#include <sys/sysctl.h>
#else
#include <sys/sysinfo.h>
#include <sys/resource.h>
#endif

using namespace boost::units;
using namespace boost::units::si;

namespace fs = boost::filesystem;

// TODO: remove this crap after TCAD interface implementation...
#include "sim_dimension.h"

// Setting up stack size (in MiB)
#define STACK_SIZE 1000


/**
 *
 * the main program .... will be running on this class.
 *
**/

class PDelay
{
private:
    std::string sim_mode;      // Decides which N-Body algorithm (One to One or Octree) to run
    std::string algorithm;     // Decides which way N-Body steps will be handled
    std::string self_path;     // Path of executable (relative path works too)
    std::string input_file;    // Input filename
    std::string cr_file;       // Continuation filename (same as input file if continue is true)
    std::string database_file; // Material database filename
    bool continued;            // Continued or not
    bool force_delta_t;        // force fixed delta_t or not.
    unsigned int vis_mode;     // Carrier log file format... don't change initial value unless needed.
    quantity<length> def_unit; // Default unit of input dimension.
    std::string c_log_str;     // Carrier log generation setting receiver.
    bool c_log;                // Generate carrier log.
    std::string bias_str;      // Bias input string
    std::string dimension_str; // Dimension string

    enum sim_modes { onetoone, octree }; // Supported simulation modes.
    enum algorithm_modes { oneshot, skdk, sdkd }; // Supported algorithms.
    sim_modes sim_mode_i;       // Current sim mode as enum
    algorithm_modes sim_algorithm_i; // Current algorithm as enum

    // Runners for each simulation modes.
    std::unique_ptr<NBody> NBodyRunner;
    std::unique_ptr<NBody_Octree> NBodyOctreeRunner;

    // Some more input parameters.
    fp_t doping_concentration;

    // Number of processes
    unsigned int num_of_procs;

    // Sensor chunk as Box datatype (um, um, um)
    Box SensorChunk;

    // Bias as Bias datatype (V)
    Bias DetBias;

    // Detector materials
    std::string DetMaterial;
    std::string InsulatorMaterial;

    // Basic simulation parameters
    fp_t delta_t;
    fp_t temperature;

    // Get Path to given file
    std::string GetPath(std::string some_file_path);
    std::string GetPath(const char* some_file_path);
    std::string GetFullPath(std::string some_file_path);
    std::string GetFullPath(const char* some_file_path);
    
    // Parse options
    //
    // The cxxopts::Options was Adopted from...
    // --> https://github.com/jarro2783/cxxopts
    //
    cxxopts::Options options;
    int ParseOptions(int argc, char* argv[]); // Parses input from main()

public:
    // Utility methods
    //

    // CPU number extraction utility
    int cpuNUM();

#if defined(__posix) || defined(__linux) || defined(__unix)
    // Set stack size (generally enlarges it)
    int set_stack_size(const rlim_t& stack_size);
#endif /* #if defined(__posix) || defined(__linux) || defined(__unix) */

    // Usage display
    void usage(const char* exec_name);

    // Simulation runners
    int Run();
    int RunOneToOne();
    int InitOneToOneS();
    int InitOneToOneC();
    int RunOctree();
    int InitOctreeS();
    int InitOctreeC();

    /**
     *
     * Set and get methods.
     *
    **/
    void SetAlgorithm(const std::string& new_calc_algorithm);
    void SetContinued(bool i_continued);
    void SetProcesses(const unsigned int& new_procs);
    void SetDimension(const Box& new_dimension);
    void SetDimension(
        const fp_t& x_start, const fp_t& x_end,
        const fp_t& y_start, const fp_t& y_end,
        const fp_t& z_start, const fp_t& z_end);
    void SetBias(const Bias& new_bias);
    void SetBias(const fp_t& top_bias, const fp_t& bottom_bias);
    void SetDetMaterial(const std::string& new_det_material);
    void SetDetMaterial(const char* new_det_material);
    void SetInsulatorMaterial(const std::string& new_insulator_material);
    void SetInsulatorMaterial(const char* new_insulator_material);
    void SetTemperature(const fp_t& new_temp);
    void SetDeltaT(const fp_t& new_delta_t);
    bool SetSelfPath(const char* argv_zero);
    void SetMatDBFilePath();
    bool SetMatDBFile(const std::string& new_DB_file);
    bool SetMatDBFile(const char* new_DB_file);
    bool SetInputFile(const std::string& new_input_file);
    bool SetInputFile(const char* new_input_file);
    int SetSimMode(std::string mode);
    int SetSimMode(const char* new_sim_mode);

    /**
     *
     * Report Status
     *
    **/
    void ReportStatus();
    void ReportBias();
    void ReportDimension();
    void ReportDeltaT();
    void ReportSimMode();
    void ReportAlgorithm();

    /**
     *
     * Contructors and Destructors
     *
    **/
    // the basic constructor --> Don't use this
    PDelay() : \
        sim_mode("OneToOne"),
        algorithm("OneShot"),
        sim_mode_i(onetoone),
        sim_algorithm_i(oneshot),
        self_path("."),
        input_file({}),
        cr_file({}),
        c_log(true),
        bias_str({}),
        dimension_str({}),
        database_file(MAT_DB_FILE),
        vis_mode(NBV_OMODE_SQLITE3),
        force_delta_t(false),
        doping_concentration(DOPING_CONC),
        delta_t(FP_T(0.0)),
        temperature(FP_T(300.0)),
        def_unit(um),
        SensorChunk(DIM_BOX),
        DetBias(BIAS_DEF),
        DetMaterial(MATERIAL),
        InsulatorMaterial({}),
        continued(false),
        num_of_procs(cpuNUM()),
        options({}),
        NBodyRunner(nullptr),
        NBodyOctreeRunner(nullptr)
    {
    }

    // A proper initializer.
    PDelay(int argc, char* argv[]) : \
        PDelay()
    {
        // Preparing options parser.
        options = cxxopts::Options(argv[0], " - pdelay command line options");

        // And parse given options.
        ParseOptions(argc, argv);
    }

    ~PDelay()
    {}

}; /* class PDelay */



#endif /* Include guard */
