/**
 *
 * sim_dimension.h
 *
 * Defines simulation dimension...
 *
 * TODO: Implement a better interfacing for official relase.
 *
**/

#ifndef __sim_dimension_h__
#define __sim_dimension_h__

#include "sim_space.h"

/****************************************************************************/
/************** Fix this area if you wish to change any stuff ***************/
/****************************************************************************/

// Sensor dimension (in um)
const fp_t X_START = -10000;
const fp_t X_END = 10000;
const fp_t Y_START = -10000;
const fp_t Y_END = 10000;
const fp_t Z_START = 0.0;
const fp_t Z_END = 500;

// Bias (in V)
const fp_t BIAS_ANODE = -200;     // Bulk electrode side: photons impinge here...
const fp_t BIAS_CATHODE = -1;     // Pixel electrode side

// Doping concentration
const fp_t DOPING_CONC = 3.59e+11;

// Sensor Material
const std::string MATERIAL = "Silicon";

// Default Database File
const std::string MAT_DB_FILE = "src/database/Materials.db";

/****************************************************************************/
/********************* OK to change part ends here!! ************************/
/****************************************************************************/

// The input stuff 
const Box DIM_BOX = Box{ X_START, X_END, Y_START, Y_END, Z_START, Z_END };
const Bias BIAS_DEF = Bias{ BIAS_ANODE, BIAS_CATHODE };

#endif /* include guard */