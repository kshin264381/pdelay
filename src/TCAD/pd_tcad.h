/**
 *
 * pd_tcad.h
 *
 * A tcad electric field calculation tool for pdelay.
 * Only supports 2D model at this moment.
 *
 *  Created on March 22th 2016
 *       by Taylor Shin
 *
**/

#ifndef __pd_tcad_h__
#define __pd_tcad_h__

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>

#include "unique_ptr.h"
#include "fputils.h"
#include "t_matrix.h"
#include "pd_tcad_mesh.h"
#include "pd_tcad_solver.h"

/**
 * TCAD simulation interface control class
 *
 * Oh well, let's do it anyway...
 *
**/
namespace pdTCAD
{

class TCAD {
private:
    const char* filename;
    short model;
    std::vector<fp_t> solver_range;

    // Input deck parsor (reads in this->filename)
    void Parsor();

    // Meshing object.
    Mesh MeshData;
    Solver<fp_t> mSolver;

public:
    // Methods

    // Constructors and Destructors
    TCAD(); // Dummy constructors
    TCAD(const char* deck_filename); // Initialize the simulation with file...
    virtual ~TCAD();

};

} // namespace pdTCAD
#endif /* __pd_tcad_h__ include guard */
