/**
 *
 * Octant.h
 *
 * Implementation of an Octal which will be contained into Octal Tree
 *
 *  Created on: Oct 6th, 2016
 *      Author: Taylor Shin
 *
**/

#ifndef __octal_h__
#define __octal_h__

#include <memory>
#include <iostream>
#include <sstream>
#include <string>

#include "fputils.h"
#include "physical_constants.h"
#include "carrier.h"
#include "typedefs.h"

/**
 *
 * the class octal.
 * A cubic section in a divided sensor material. Each Octal
 * region should be containing a carrier and its size.
 *
**/
class Octant
{
private:
    // The length of one side of the Octal
    Dim  length;

    // Coordinate of center of the Octal
    Loc  center;

public:
    // Assignment and return methods.
    //
    // Returns length of current octal
    Dim& GetLength()
    { return length; }
    const Dim& GetLength() const
    { return length; }
    fp_t GetLengthX()
    { return length.x; }
    // Returns center of current octal
    Loc& GetCenter()
    { return center; }
    const Loc& GetCenter() const
    { return center; }

    // Set up length
    void SetLength(const fp_t& new_length)
    { length = Dim{ new_length, new_length, new_length }; }
    // Assigns new center of octal.
    void SetCenter(const Loc& new_center_loc)
    { center = new_center_loc; }

    // Check if this quadrant contains a certain point.
    bool Contains(const Loc& some_coord) const;

    // Check if this quadrant contains a certain carrier
    bool Contains(const spCarrier& some_Carrier) const;

    // New Sub Octant generator for other carriers
    std::shared_ptr<Octant> NewOctant(const spCarrier& some_carrier);

    // Returns a proper octant location for given carrier
    int GetOctantPart(const spCarrier& carrier) const;

    // Generate octant by force.
    std::shared_ptr<Octant> uNE() const;
    std::shared_ptr<Octant> uNW() const;
    std::shared_ptr<Octant> uSE() const;
    std::shared_ptr<Octant> uSW() const;
    std::shared_ptr<Octant> lNE() const;
    std::shared_ptr<Octant> lNW() const;
    std::shared_ptr<Octant> lSE() const;
    std::shared_ptr<Octant> lSW() const;

    // Tells if a given carrier is within this Octant or not.
    // Wrapper for Contains
    bool in(const spCarrier& carrier) const
    { return Contains(carrier); }

    // Assignment operators
    Octant& operator= (const Octant& other);
    Octant& operator= (Octant&& other) noexcept;

    /**
     *
     * Constructors and Destructors
     *
    **/
    Octant();
    Octant(const Dim& dim, const Loc& center_loc);
    Octant(
        const std::shared_ptr<Dim> spdim,
        const std::shared_ptr<Loc> spcloc
    );
    Octant(const Octant& other);
    Octant(Octant&& other) noexcept;

    virtual ~Octant();

}; /* class Octant */


// Integrating with ostream
std::ostream& operator<< (std::ostream& os, const Octant& octant);


#endif /* Include guard */
