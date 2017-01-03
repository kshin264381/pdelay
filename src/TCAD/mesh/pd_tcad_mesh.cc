/**
 * pd_tcad_mesh.cc
 *
 *  Implementation for pd_tcad_mesh class
 *  defined at pd_tcad_mesh.h...
 *
 *  Created on: May 4, 2016
 *      Author: Taylor Shin
**/

#include "pd_tcad_mesh.h"

using namespace pdTCAD;

/**
 * Private stuff
 *
**/
// Node list generator... (populates this->NodeList)
// --> Note generatiion routine. if the class was initialized with
//     box type device, we don't need to use it.
//
bool Mesh::__generate_node_list__()
{
    return false;
}

/**
 * Public stuff
 *
**/

/**
 * Constructors and destructors
 *
**/
// Dummy... you need to initialize everyting one by one.
Mesh::Mesh()
{

}

// Square devices.
Mesh::Mesh(
    DATA_TYPE xstart,
    DATA_TYPE xend,
    DATA_TYPE ystart,
    DATA_TYPE yend) : Mesh()
{

}

// Dict type initializer... --> custom shape devices
// TODO: Gotta implement initializer routine.
Mesh::Mesh(
    Dict<std::vector<DATA_TYPE>, DATA_TYPE>) : Mesh()
{

}

// Destructor
Mesh::~Mesh()
{
    
}
