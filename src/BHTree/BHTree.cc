/**
 * BHTree.cc
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

#include "BHTree.h"

// Checking if it's external or not.
bool BHTree::isExternal(const std::shared_ptr<BHTree>& bht)
{
    if (!bht->uNE && !bht->uNW && 
        !bht->uSE && !bht->uSW && 
        !bht->lNE && !bht->lNW &&
        !bht->lSE && !bht->lSW) {
        return true;
    }
    else {
        return false;
    }
}

// Self checking version
bool BHTree::isExternal()
{
    if (!this->uNE && !this->uNW &&
        !this->uSE && !this->uSW &&
        !this->lNE && !this->lNW &&
        !this->lSE && !this->lSW) {
        return true;
    }
    else {
        return false;
    }
}

// Empty node?
bool BHTree::isEmpty()
{
    if (!this->tree_carrier) return true;
    else return false;
}


// Insert a carrier
//
// TODO: first try... make this thing non-recursive.
//
int BHTree::insert(const spCarrier& carrier)
{
    // Just put down a carrier if current tree is empty.
    if (this->tree_carrier == nullptr) {
        this->tree_carrier = carrier;
        return 0;
    }

    // make a sub_Octant
    auto carrier_loc = this->current_octant->GetOctantPart(carrier);

    // uNE, uNW, uSE, uSW, lNE, lNW, lSE, lSW
    // are mapped to...
    // 0, 1, 2, 3, 4, 5, 6, 7

    // At first, we need to find the last viable external node.
    auto node_to_insert = std::make_shared<BHTree>(*this);
    auto depth = 0;
    spOctant octant_for_new_node = nullptr;
    while(node_to_insert) {
        carrier_loc = node_to_insert->GetOctant()->GetOctantPart(carrier);
        switch (carrier_loc) {
            case 0:
                octant_for_new_node = node_to_insert->GetOctant()->uNE();
                node_to_insert = node_to_insert->GetuNE();
                break;
            case 1:
                octant_for_new_node = node_to_insert->GetOctant()->uNW();
                node_to_insert = node_to_insert->GetuNW();
                break;
            case 2:
                octant_for_new_node = node_to_insert->GetOctant()->uSE();
                node_to_insert = node_to_insert->GetuSE();
                break;
            case 3:
                octant_for_new_node = node_to_insert->GetOctant()->uSW();
                node_to_insert = node_to_insert->GetuSW();
                break;
            case 4:
                octant_for_new_node = node_to_insert->GetOctant()->lNE();
                node_to_insert = node_to_insert->GetlNE();
                break;
            case 5:
                octant_for_new_node = node_to_insert->GetOctant()->lNW();
                node_to_insert = node_to_insert->GetlNW();
                break;
            case 6:
                octant_for_new_node = node_to_insert->GetOctant()->lSE();
                node_to_insert = node_to_insert->GetlSE();
                break;
            case 7:
                octant_for_new_node = node_to_insert->GetOctant()->lSW();
                node_to_insert = node_to_insert->GetlSW();
                break;
        }
        depth++;
    }

    // Now, assign a new node...
    switch (carrier_loc) {
        case 0:
            node_to_insert = std::make_shared<BHTree>(
                octant_for_new_node,
                depth,
                carrier_loc);
            break;
        case 1:
            node_to_insert = std::make_shared<BHTree>(
                octant_for_new_node,
                depth,
                carrier_loc);
            break;
        case 2:
            node_to_insert = std::make_shared<BHTree>(
                octant_for_new_node,
                depth,
                carrier_loc);
            break;
        case 3:
            node_to_insert = std::make_shared<BHTree>(
                octant_for_new_node,
                depth,
                carrier_loc);
            break;
        case 4:
            node_to_insert = std::make_shared<BHTree>(
                octant_for_new_node,
                depth,
                carrier_loc);
            break;
        case 5:
            node_to_insert = std::make_shared<BHTree>(
                octant_for_new_node,
                depth,
                carrier_loc);
            break;
        case 6:
            node_to_insert = std::make_shared<BHTree>(
                octant_for_new_node,
                depth,
                carrier_loc);
            break;
        case 7:
            node_to_insert = std::make_shared<BHTree>(
                octant_for_new_node,
                depth,
                carrier_loc);
            break;
    }

    //
    // Old, stack overflow prone code
    //
    // Now, pass given carrier to subsequent tree.
    // switch (carrier_loc) {
    // case 0:
    //     if (this->uNE == nullptr) {
    //         this->uNE = std::make_shared<BHTree>(
    //             this->current_octant->uNE(), this->depth + 1, carrier_loc);
    //     }
    //     this->uNE->insert(carrier);
    //     break;
    // case 1:
    //     if (this->uNW == nullptr) {
    //         this->uNW = std::make_shared<BHTree>(
    //             this->current_octant->uNW(), this->depth + 1, carrier_loc);
    //     }
    //     this->uNW->insert(carrier);
    //     break;
    // case 2:
    //     if (this->uSE == nullptr) {
    //         this->uSE = std::make_shared<BHTree>(
    //             this->current_octant->uSE(), this->depth + 1, carrier_loc);
    //     }
    //     this->uSE->insert(carrier);
    //     break;
    // case 3:
    //     if (this->uSW == nullptr) {
    //         this->uSW = std::make_shared<BHTree>(
    //             this->current_octant->uSW(), this->depth + 1, carrier_loc);
    //     }
    //     this->uSW->insert(carrier);
    //     break;
    // case 4:
    //     if (this->lNE == nullptr) {
    //         this->lNE = std::make_shared<BHTree>(
    //             this->current_octant->lNE(), this->depth + 1, carrier_loc);
    //     }
    //     this->lNE->insert(carrier);
    //     break;
    // case 5:
    //     if (this->lNW == nullptr) {
    //         this->lNW = std::make_shared<BHTree>(
    //             this->current_octant->lNW(), this->depth + 1, carrier_loc);
    //     }
    //     this->lNW->insert(carrier);
    //     break;
    // case 6:
    //     if (this->lSE == nullptr) {
    //         this->lSE = std::make_shared<BHTree>(
    //             this->current_octant->lSE(), this->depth + 1, carrier_loc);
    //     }
    //     this->lSE->insert(carrier);
    //     break;
    // case 7:
    //     if (this->lSW == nullptr) {
    //         this->lSW = std::make_shared<BHTree>(
    //             this->current_octant->lSW(), this->depth + 1, carrier_loc);
    //     }
    //     this->lSW->insert(carrier);
    //     break;
    // } /* switch (carrier_loc) */

    return 0;
}
// direct insertion version
int BHTree::insert(Carrier carrier)
{
    return insert(std::make_shared<Carrier>(carrier));
}


// Setting up ID
void BHTree::SetID()
{
    // Cannot set ID without a carrier.
    if (!current_octant)
        return;

    std::stringstream ss;
    std::string branch_dir_str;

    // uNE, uNW, uSE, uSW, lNE, lNW, lSE, lSW
    // are mapped to...
    // 0, 1, 2, 3, 4, 5, 6, 7
    switch (this->branch_direction) {
    case 0:
        branch_dir_str = "uNE";
        break;
    case 1:
        branch_dir_str = "uNW";
        break;
    case 2:
        branch_dir_str = "uSE";
        break;
    case 3:
        branch_dir_str = "uSW";
        break;
    case 4:
        branch_dir_str = "lNE";
        break;
    case 5:
        branch_dir_str = "lNW";
        break;
    case 6:
        branch_dir_str = "lSE";
        break;
    case 7:
        branch_dir_str = "lSW";
        break;
    case -1:
        branch_dir_str = "Head";
        break;
    }

    ss << "D" << this->depth << "BD" << branch_dir_str;

    this->ID = ss.str();

    return;
}

// String output
std::string BHTree::to_string() const
{
    std::stringstream ss;

    if (!(uNE && uNW && uSE && uSW && lNE && lNW && lSE && lSW)) {
        ss << "External Case!!";
        if (tree_carrier) {
            ss << " Holding: " \
                << this->tree_carrier->GetID();
        }
        else {
            ss << " Holding No Carrier.";
        }
    }
    else {
        if (tree_carrier) {
            ss << " Holding: " \
                << this->tree_carrier->GetID();
        }
        else {
            ss << " Holding No Carrier.";
        }
        ss << std::endl;
        ss << "---------------------------" << std::endl;

        ss << "Upper North East: " \
            << uNE->GetID() << std::endl;
        ss << "Upper North West: " \
            << uNW->GetID() << std::endl;
        ss << "Upper South East: " \
            << uSE->GetID() << std::endl;
        ss << "Upper South West: " \
            << uSW->GetID() << std::endl;
        ss << "Upper North East: " \
            << lNE->GetID() << std::endl;
        ss << "Upper North West: " \
            << lNW->GetID() << std::endl;
        ss << "Upper South East: " \
            << lSE->GetID() << std::endl;
        ss << "Upper South West: " \
            << lSW->GetID() << std::endl;
    }

    ss << std::endl;

    return ss.str();
}

// Delete all branches
void BHTree::ResetAll()
{
    tree_carrier = nullptr;
    current_octant = nullptr;

    uNE = nullptr;
    uNW = nullptr;
    uSE = nullptr;
    uSW = nullptr;
    lNE = nullptr;
    lNW = nullptr;
    lSE = nullptr;
    lSW = nullptr;

    // this->current_octant.reset();

    // this->uNE.reset();
    // this->uNW.reset();
    // this->uSE.reset();
    // this->uSW.reset();
    // this->lNE.reset();
    // this->lNW.reset();
    // this->lSE.reset();
    // this->lSW.reset();
}

// Assignment operators
BHTree& BHTree::operator= (const BHTree& other)
{
    BHTree tmp(other);
    *this = std::move(tmp);
    return *this;
}

BHTree& BHTree::operator= (BHTree&& other) noexcept
{
    this->ResetAll();

    this->tree_carrier = other.GetCarrier();
    this->current_octant = other.GetOctant();
    this->depth = other.GetDepth();
    this->branch_direction = other.GetDirection();

    this->uNE = other.GetuNE();
    this->uNW = other.GetuNW();
    this->uSE = other.GetuSE();
    this->uSW = other.GetuSW();
    this->lNE = other.GetlNE();
    this->lNW = other.GetlNW();
    this->lSE = other.GetlSE();
    this->lSW = other.GetlSW();

    this->ID = other.GetID();

    other.ResetAll();

    return *this;
}



/**
 *
 * Constructors and Destructors
 *
**/
BHTree::BHTree() : \
    tree_carrier(nullptr),
    current_octant(nullptr),
    uNE(nullptr),
    uNW(nullptr),
    uSE(nullptr),
    uSW(nullptr),
    lNE(nullptr),
    lNW(nullptr),
    lSE(nullptr),
    lSW(nullptr),
    ID({}),
    depth(0)
{
}

BHTree::BHTree(Octant oct, uint64_t ndepth, short bdir) : BHTree::BHTree()
{
    current_octant = std::make_shared<Octant>(oct);
    depth = ndepth;
    branch_direction = bdir;
    SetID();
}

BHTree::BHTree(spOctant spoct, uint64_t ndepth, short bdir) : BHTree::BHTree()
{
    current_octant = spoct;
    depth = ndepth;
    branch_direction = bdir;
    SetID();
}

BHTree::BHTree(const BHTree& other) :
    tree_carrier(other.GetCarrier()),
    current_octant(other.GetOctant()),
    depth(other.GetDepth()),
    branch_direction(other.GetDirection()),
    uNE(other.GetuNE()),
    uNW(other.GetuNW()),
    uSE(other.GetuSE()),
    uSW(other.GetuSW()),
    lNE(other.GetlNE()),
    lNW(other.GetlNW()),
    lSE(other.GetlSE()),
    lSW(other.GetlSW()),
    ID(other.GetID())
{
}

BHTree::BHTree(BHTree&& other) noexcept :
    tree_carrier(other.GetCarrier()),
    current_octant(other.GetOctant()),
    depth(other.GetDepth()),
    branch_direction(other.GetDirection()),
    ID(other.GetID())
{
    this->uNE = other.GetuNE();
    this->uNW = other.GetuNW();
    this->uSE = other.GetuSE();
    this->uSW = other.GetuSW();
    this->lNE = other.GetlNE();
    this->lNW = other.GetlNW();
    this->lSE = other.GetlSE();
    this->lSW = other.GetlSW();

    other.ResetAll();
}

BHTree::~BHTree()
{
    ResetAll();
}


// Attaching to ostream to print out information
std::ostream& operator<< (std::ostream& os, const BHTree& bhtree)
{
    os << bhtree.to_string();
    return os;
}