/**
 *
 * pd_tcad_mesh.h
 *
 *  Contains semiconductor meshing algorithm for pd_tcad routine.
 *  It generates 10 nodes per um without *.str input.
 *
 *  The input *.str can be Silvaco Devedit generated str file or
 *  Suprem4.GS generated str file.
 *
 *  Created on: May 4, 2016
 *      Author: Taylor Shin
 *
**/

#ifndef __pd_tcad_mesh_h__
#define __pd_tcad_mesh_h__

#include <iostream>
#include <vector>
#include <deque>

#include "unique_ptr.h"
#include "fputils.h"
#include "pd_tcad_node.h"
#include "datatype_hash.h" // Implements has datatype.

using namespace DataType;

// namespace definition
namespace pdTCAD
{

// Defining default datatypes
using DATA_TYPE = fp_t;
using INDEX_TYPE = uint64_t;
using pdNode = std::unique_ptr<Node<DATA_TYPE, INDEX_TYPE>>;
using Index = std::vector<INDEX_TYPE>;
using vecData = std::vector<DATA_TYPE>;

/*
 * The Mesh class
 *
 * Generates mesh data structure and supports solver operation.
 *
 */
class Mesh {
private:
    // Node container --> must be dynamically allocated into heap!!
    std::shared_ptr<std::deque<pdNode>> NodeList;

    // We can keep mesh generation instuction in stack.
    Dict<std::vector<DATA_TYPE>, DATA_TYPE> MeshGenInstr;

    // Mesh generation
    bool __generate_node_list__();


public:



    // Constructors and Destructors
    // Dummy initializer
    Mesh();
    // Initialize with box shape dimension (automatic meshing)
    Mesh(
        DATA_TYPE xstart,
        DATA_TYPE xend,
        DATA_TYPE ystart,
        DATA_TYPE yend);
    // Initialize with Meshing assignments
    // --> Dict type data looks like:
    //     MeshInstruction = {
    //          std::vector{1.0,2.0}: 0.1,
    //          std::vector{20.0,40.0}: 1.0
    //          std::vector{100.0, 40.0}: 0.023
    //     }
    // of course, units are in um
    // --> Detects boundary automatically.
    Mesh(Dict<std::vector<DATA_TYPE>, DATA_TYPE>);
    // Destroy this crap!!
    virtual ~Mesh();
};



} // Namespace pdTCAD
#endif /* Include guard */