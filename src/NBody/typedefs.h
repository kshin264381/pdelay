/**
 *
 * typedef.h
 *
 * Some shortcut types for carriers.
 *
 * Written by Taylor Shin
 * Somewhere in early 2016
 *
**/

#ifndef __typedefs_h__
#define __typedefs_h__

#include "carrier.h"
#include "datatype_hash.h"

#include <deque>
#include <vector>
#include <set>

/**
 * Some typedefs
 *
**/
using spCarrier        = std::shared_ptr<Carrier>;

// Some types of carrier containers.
using CarrierVector    = std::vector<spCarrier>;
using CarrierDeque     = std::deque<spCarrier>;
using CarrierMap       = DataType::Map<uint64_t, spCarrier>;

// Defining the 'CarrierList' type for NBody class
using CarrierList      = CarrierMap;

using CarrierList_iter = CarrierList::iterator;
using CarrierSet       = std::set<spCarrier>;

#endif /* Include guard */