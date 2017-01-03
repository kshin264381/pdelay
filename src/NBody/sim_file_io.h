/**
 *
 * sim_file_io.h 
 *
 * File IO part for N-Body simulation
 *
 * Written by Taylor Shin
 * Sep. 7th, 2016
 *
**/

#ifndef __sim_file_io_h__
#define __sim_file_io_h__

#define BOOST_FILESYSTEM_NO_DEPRECATED

#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <boost/filesystem.hpp>

#include "sim_space.h"

class NBodyFileIO : public virtual sim_space
{
public:
    // private or protected members
    //
    // tar compressed csv file
    const char* container_file;
    // Database filename if continues from previous state.
    const char* db_file;

    // Write to file if a carrier escaped the Box!!
    std::string logfile_name;
    std::string time_string;
    std::ofstream simulation_logfile;

    // Methods
    int set_logfile_name();
    std::string carrier_info(spCarrier& carrier);
    int write_collected_carrier_info(spCarrier& carrier);
    int write_lost_carrier_info(spCarrier& carrier);
    int write_recombination_carrier_info(
        spCarrier& carrier, spCarrier& other);
    int write_mat_recomb_carrier_info(spCarrier& carrier);
    std::string GetBaseFilename();

    // Generate carriers from csv file
    int generate_carriers();
    // Generate carriers from database file
    int generate_carriers_db();


    // Constructors and Destructors
    NBodyFileIO() : \
        container_file(nullptr),
        db_file(nullptr),
        logfile_name(""),
        time_string(""),
        simulation_logfile()
    {;}
    virtual ~NBodyFileIO() {;}

};




#endif /* Include guard */