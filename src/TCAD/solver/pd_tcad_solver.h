/**
 *
 * pd_tcad_solver.h
 *
 *  Contains solver algorithms for Poisson equation (potential solver)
 *  and continuous equation (current and carrier concentration.)
 *
 *  At this moment, we don't need to consider carrier movement but
 *  potentials.
 *
 *  Created on: May 4, 2016
 *      Author: Taylor Shin
 *
**/

#ifndef __pd_tcad_solver_h__
#define __pd_tcad_solver_h__

#include "unique_ptr.h"
#include "fputils.h"
#include "t_matrix.h"
#include "pd_tcad_mesh.h"


namespace pdTCAD
{

/*
 * The pd_tcad_solver class
 *
 * Solver for Poisson and Continuous equation.
 *
 */
template <class T>
class Solver {
private:

public:

};


} // namespace pdTCAD

#endif /* Include guard */