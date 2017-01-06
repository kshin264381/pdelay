/**
 * BHTree.h
 *
 * Implementation of Barnes-Hut tree
 *
 * Original Code:
 *    http://physics.princeton.edu/~fpretori/Nbody/
 * Barnes-Hut:
 *    http://www.ifa.hawaii.edu/~barnes/treecode/treeguide.html
 *
 *  Created on: Oct 6th, 2016
 *      Author: Taylor Shin
**/

#ifndef __bhtree_h__
#define __bhtree_h__

#include <memory>
#include <iostream>
#include <string>
#include <sstream>

#include "Octant.h"
#include "carrier.h"
#include "typedefs.h"

// some typedefs
using spOctant = std::shared_ptr<Octant>;
//using spBHTree = std::shared_ptr<BHTree>;

/**
 *
 * The Barnes-Hut tree Implementation
 *
**/
class BHTree
{
private:
    spCarrier tree_carrier;
    spOctant  current_octant;
    uint64_t  depth;
    short branch_direction;

    // branches
    std::shared_ptr<BHTree> uNW;
    std::shared_ptr<BHTree> uNE;
    std::shared_ptr<BHTree> uSW;
    std::shared_ptr<BHTree> uSE;
    std::shared_ptr<BHTree> lNW;
    std::shared_ptr<BHTree> lNE;
    std::shared_ptr<BHTree> lSW;
    std::shared_ptr<BHTree> lSE;

    // ID string
    std::string ID;

public:
    // If other branch nodes are nulls, then the Octant represents
    // a single body and it is called "External."
    bool isExternal(const std::shared_ptr<BHTree>& bht);
    bool isExternal();

    // Empty node?
    bool isEmpty();

    // Inserting a carrier into the BHTree
    int insert(const spCarrier& carrier);
    int insert(Carrier carrier);

    // Emit current tree information as string.
    std::string to_string() const;

    // Setting up Tree ID
    void SetID();

    // Returns Tree ID (or info.)
    std::string GetID() const
    { return ID; }

    // Returns carrier pointer
    spCarrier& GetCarrier()
    { return tree_carrier; }
    const spCarrier& GetCarrier() const
    { return tree_carrier; }

    // Returns Octant pointer
    spOctant& GetOctant()
    { return current_octant; }
    const spOctant& GetOctant() const
    { return current_octant; }

    // Returns depth
    uint64_t GetDepth() const
    { return depth; }

    // Returns direction code of current node
    short GetDirection() const
    { return branch_direction; }

    // Accessing branches...
    std::shared_ptr<BHTree> GetuNW() const { return uNW; }
    std::shared_ptr<BHTree> GetuNE() const { return uNE; }
    std::shared_ptr<BHTree> GetuSW() const { return uSW; }
    std::shared_ptr<BHTree> GetuSE() const { return uSE; }
    std::shared_ptr<BHTree> GetlNW() const { return lNW; }
    std::shared_ptr<BHTree> GetlNE() const { return lNE; }
    std::shared_ptr<BHTree> GetlSW() const { return lSW; }
    std::shared_ptr<BHTree> GetlSE() const { return lSE; }

    // Delete all branches and carriers
    void ResetAll();

    // Assignment operators
    BHTree& operator= (const BHTree& other);
    BHTree& operator= (BHTree&& other) noexcept;

    /**
     *
     * Constructors and Destructors
     *
    **/
    BHTree();
    BHTree(Octant oct, uint64_t ndepth=0, short bdir=-1);
    BHTree(spOctant spoct, uint64_t ndepth=0, short bdir=-1);

    BHTree(const BHTree& other);
    BHTree(BHTree&& other) noexcept;

    virtual ~BHTree();
};

// Integrating with ostream
std::ostream& operator<< (std::ostream& os, const BHTree& bhtree);


#endif /* Include guard */
