/**
 *
 * visual.h
 *
 * Visual library for nbody algorithm
 *
 * Written by Taylor Shin
 * May. 27th, 2016
 *
 * --> Maybe implement a GUI based on Qt library later?
 *
**/

#ifndef __visual_h__
#define __visual_h__

#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <boost/filesystem.hpp>

#include "typedefs.h"
#include "fputils.h"
#include "pbar.h"

#include "sim_space.h"
#include "sim_file_io.h"
#include "sim_progress.h"

// Some SQLITE check stuffs
#define SQLITE_CHECK_EXISTS 1
#define SQLITE_CHECK_INEXISTS 0

// Output Modes
static const unsigned int NBV_OMODE_LOG = 0;
static const unsigned int NBV_OMODE_CSV = 1;
static const unsigned int NBV_OMODE_SQLITE3 = 2;
static const unsigned int NBV_OMODE_MAX = 2;

// Some typedef(s)
using IntMap = std::map<unsigned int, std::string>;

// callback function wrapper for sqlite3
static int __callback__(void* param, int argc, char** argv, char** azColName);

class NBodyVisual : \
    public virtual sim_space, \
    public virtual NBodyFileIO, \
    public virtual SimProgress
{
private:

    // base filename
    std::string output_filename;

    // file output stream
    std::ofstream output_file;

    // Extension dictionary
    IntMap ExtensionMap;
    IntMap Delimiters;

    // Temporary data container
    uint64_t carr_index;             // index
    fp_t carr_mass;                  // mass
    fp_t x_coord, y_coord, z_coord;  // Location
    fp_t x_vel, y_vel, z_vel;        // Velocity
    fp_t x_for, y_for, z_for;        // Force
    std::string carr_type;           // Carrier Type (electron? hole?)
    std::string timestamp_str;       // for sql use.

    // File output mode
    //
    // 0: NBody log format (default)
    // 1: csv format
    // 2: sqlite3 db file (not implemented yet)
    //
    unsigned int output_mode;
    // Write carriers as csv
    int write_carrier_csv();
    // Write carriers as log(NBody log format)
    int write_carrier_log();
    // Write carriers as sqlite3 database
    int write_carrier_sqlite3();
    int write_carrier_sqlite3_omp(
        const std::string& timestamp_str,
        const uint64_t& carr_index,
        const fp_t& carr_mass,
        const fp_t& x_coord,
        const fp_t& y_coord,
        const fp_t& z_coord,
        const fp_t& x_vel,
        const fp_t& y_vel,
        const fp_t& z_vel,
        const fp_t& x_for,
        const fp_t& y_for,
        const fp_t& z_for,
        const std::string& carr_type);
    int init_db();
    int make_table();
    int make_table(std::string table_name);
    int check_table(const char* tab_name);
    // and so on...

    // Write carrier
    template<typename TS, typename I, typename FT>
    int write_carr(
        TS ts, I ind, std::string carr_type,
        FT x, FT y, FT z,
        FT vx, FT vy, FT vz,
        FT fx, FT fy, FT fz);

    // write line
    int write_line(std::string in_text);

    // Make legend
    std::string make_legend();

    // retrieve filename to output_filename
    void retrieve_filename();

    // convert everything to string
    template <typename T>
    std::string to_str(T input);

    // Make end delimiter.
    std::string end_delim;
    void set_end_delim();

    // Progress Bar
    ProgressBar WriteCarrBar;

public:
    // Setup output mode
    void SetOutputMode(unsigned int N);

    // Get entries
    fp_uint_t GetEntries();

    // Get Carrier info filename
    std::string GetFileName() { return this->output_filename; }

    // Write current carrier location
    int WriteCarriers_sub(
        const uint64_t& istart,
        const uint64_t& ipoints,
        const std::string& ts_str);
    int WriteCarriers();
    int WriteCarriers(const std::string& table_name);
    int WriteCarriersInit();

    // Sqlite3 callback function
    int sql_callback(int argc, char** argv, char** azColName);

    /**
     * Constructors and Destructors
     *
    **/
    // Dummy one
    NBodyVisual();
    virtual ~NBodyVisual();
};





// turn everything to string
template <typename T>
std::string NBodyVisual::to_str(T input)
{
    std::stringstream ss;
    ss << std::scientific << input;
    return ss.str();
}


// Write it line by line
template<typename TS, typename I, typename FT>
int NBodyVisual::write_carr(
    TS ts, I ind, std::string carr_type,
    FT x, FT y, FT z,
    FT vx, FT vy, FT vz,
    FT fx, FT fy, FT fz)
{
    std::stringstream ss;
    ss << ts \
        << this->Delimiters[this->output_mode] \
        << ind \
        << this->Delimiters[this->output_mode] \
        << this->carr_type \
        << this->Delimiters[this->output_mode] \
        << x \
        << this->Delimiters[this->output_mode] \
        << y \
        << this->Delimiters[this->output_mode] \
        << z \
        << this->Delimiters[this->output_mode] \
        << vx \
        << this->Delimiters[this->output_mode] \
        << vy \
        << this->Delimiters[this->output_mode] \
        << vz \
        << this->Delimiters[this->output_mode] \
        << fx \
        << this->Delimiters[this->output_mode] \
        << fy \
        << this->Delimiters[this->output_mode] \
        << fz \
        << this->end_delim \
        << "\n";

    this->output_file.open(
        this->output_filename, ios::out | ios::app);

    if (this->output_file.is_open()) {
        this->output_file << ss.str();
    }
    else return -1;

    this->output_file.close();

    return 0;
}

#endif /* Include guard */