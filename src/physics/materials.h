#ifndef __materials_h__
#define __materials_h__

/**
 * materials.h
 *
 * Include material constant definitions.
 * --> Might change this part as a dedicated data reader.
 *
 * Written by Taylor Shin
 * May 12th 2016
 *
**/

#include <vector>
#include <string>
#include <sqlite3.h>
#include <cstdlib>
#include <cstdio>
#include <cmath>

#include "fputils.h"
#include "datatype_hash.h"

namespace Materials {

using Material = \
    DataType::Dict<std::string, fp_t>;
using MatType = \
    DataType::Dict<std::string, Material>;


// Sqlite3 callback functions --> Sqlite3 is written in C...
int Callback(void* param, int argc, char** argv, char** azColName);

// The Material Data handler class itself.
class MatData {
private:
    // Container for semiconductors...
    MatType Semiconductor;
    // Container for insulators
    MatType Insulator;
    // Container for contact metals
    MatType Metal;

    // Temporary material container
    MatType TempMatList;

    // Quick dirty method for Silicon Effective Mass
    fp_t SiEffMassN(); // electron
    fp_t SiEffMassP(); // hole

    // Clearing this->TempMatList
    int ClearTempMatList();

    // Sets up intrinsic carrier concentration
    void set_intrinsic();

    // Lattice Temperature
    fp_t lattice_temp;

public:
    /* Methods */
    // Initialize Silicon
    void InitSilicon();
    void InitSiO2();
    void InitSi3N4();
    void InitVacuum();
    void InitMetal();

    // Get Parameters from given material
    fp_t GetSemi(std::string MatName, std::string ParName)
    { return Semiconductor[MatName][ParName]; }
    fp_t GetInsul(std::string MatName, std::string ParName)
    { return Insulator[MatName][ParName]; }
    fp_t GetMetal(std::string MatName, std::string ParName)
    { return Metal[MatName][ParName]; }

    // Setup Temperature
    void SetTemp(fp_t new_temperature);

    // The actual callback function
    int Callback(int argc, char** argv, char** azColName);

    // Constructor and Destructor
    MatData();
    MatData(fp_t LatTemp);
    MatData(const char* sqlite3_filename);
    virtual ~MatData() {;}
};

} /* namespace Materials */



#endif /* Include Guard */