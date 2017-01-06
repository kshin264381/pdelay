/**
 * Octant.h
 *
 * Implementation of an Octal which will be contained into Octal Tree
 *
 * Method parts has been moved to here for concurrency and arrrrrgggggg!!
 * I hate super long header file anyway...
 *
 *  Created on: Oct 18th, 2016
 *      Author: Taylor Shin
**/

#include "unique_ptr.h"

#include "Octant.h"

// Check if this quadrant contains a certain carrier
bool Octant::Contains(const Loc& some_coord) const
{
    // x
    if (fp_lt<fp_t>(some_coord.x, center.x - length.x / 2.0) ||
        fp_mt<fp_t>(some_coord.x, center.x + length.x / 2.0))
        return false;
    // y
    if (fp_lt<fp_t>(some_coord.y, center.y - length.y / 2.0) ||
        fp_mt<fp_t>(some_coord.y, center.y + length.y / 2.0))
        return false;
    // z
    if (fp_lt<fp_t>(some_coord.z, center.z - length.z / 2.0) ||
        fp_mt<fp_t>(some_coord.z, center.z + length.z / 2.0))
        return false;

    return true;
}
// Wrapper for shared_ptr carrier case.
bool Octant::Contains(const spCarrier& some_Carrier) const
{
    return Contains( some_Carrier->GetPos() );
}

// New Sub Octant generator for other carriers
std::shared_ptr<Octant> Octant::NewOctant(const spCarrier& some_carrier)
{
    // Determine some_carrier's location from
    // center of this Octant.
    auto rel_loc = \
        std::make_shared<Loc>(some_carrier->GetPos() - this->center);

    // determining relative pos
    // --> True: positive side,
    // --> False: negative side,
    //
    bool x_rel, y_rel, z_rel;
    x_rel = fp_lt<fp_t>(rel_loc->x, FP_T(0.0));
    y_rel = fp_lt<fp_t>(rel_loc->y, FP_T(0.0));
    z_rel = fp_lt<fp_t>(rel_loc->z, FP_T(0.0));

    auto new_len = std::make_shared<Dim>(this->length / 2.0);
    auto new_len_adj = std::make_shared<Dim>(*new_len / 4.0);
    std::shared_ptr<Loc> new_center = nullptr;

    // Now, deal with 8 cases.
    //
    // x_rel > 0, y_rel > 0, z_rel > 0 111
    if (x_rel && y_rel && z_rel) {
        new_center = \
            std::make_shared<Loc>(
            center.x + new_len_adj->x,
                center.y + new_len_adj->y,
                center.z + new_len_adj->z);
    }
    // x_rel > 0, y_rel > 0, z_rel < 0 110
    if (x_rel && y_rel && !z_rel) {
        new_center = \
            std::make_shared<Loc>(
            center.x + new_len_adj->x,
                center.y + new_len_adj->y,
                center.z - new_len_adj->z );
    }
    // x_rel > 0, y_rel < 0, z_rel > 0 101
    if (x_rel && !y_rel && z_rel) {
        new_center = \
            std::make_shared<Loc>(
            center.x + new_len_adj->x,
                center.y - new_len_adj->y,
                center.z + new_len_adj->z);
    }
    // x_rel > 0, y_rel < 0, z_rel < 0 100
    if (x_rel && !y_rel && !z_rel) {
        new_center = \
            std::make_shared<Loc>(
            center.x + new_len_adj->x,
                center.y - new_len_adj->y,
                center.z - new_len_adj->z);
    }
    // x_rel < 0, y_rel > 0, z_rel > 0 011
    if (!x_rel && y_rel && z_rel) {
        new_center = \
            std::make_shared<Loc>(
            center.x - new_len_adj->x,
                center.y + new_len_adj->y,
                center.z + new_len_adj->z);
    }
    // x_rel < 0, y_rel < 0, z_rel > 0 001
    if (!x_rel && !y_rel && z_rel) {
        new_center = \
            std::make_shared<Loc>(
            center.x - new_len_adj->x,
                center.y - new_len_adj->y,
                center.z + new_len_adj->z);
    }
    // x_rel < 0, y_rel > 0, z_rel < 0 010
    if (!x_rel && y_rel && !z_rel) {
        new_center = \
            std::make_shared<Loc>(
            center.x - new_len_adj->x,
                center.y + new_len_adj->y,
                center.z - new_len_adj->z);
    }
    // x_rel < 0, y_rel < 0, z_rel < 0 000
    if (!x_rel && !y_rel && !z_rel) {
        new_center = \
            std::make_shared<Loc>(
            center.x - new_len_adj->x,
                center.y - new_len_adj->y,
                center.z - new_len_adj->z);
    }

    return std::make_shared<Octant>(new_len, new_center);
}

// Returns a proper octant location for given carrier
int Octant::GetOctantPart(const spCarrier& carrier) const
{
    // Determine some_carrier's location from
    // center of this Octant.
    auto rel_loc = \
        std::make_unique<Loc>(carrier->GetPos() - this->center);

    // determining relative pos
    // --> True: positive side,
    // --> False: negative side,
    //
    bool x_rel, y_rel, z_rel;
    x_rel = fp_lt<fp_t>(rel_loc->x, FP_T(0.0));
    y_rel = fp_lt<fp_t>(rel_loc->y, FP_T(0.0));
    z_rel = fp_lt<fp_t>(rel_loc->z, FP_T(0.0));

    // uNE, uNW, uSE, uSW, lNE, lNW, lSE, lSW
    // are mapped to...
    // 0, 1, 2, 3, 4, 5, 6, 7
    //
    if (x_rel && y_rel && z_rel)
    {
        return 0;
    }
    else if (x_rel && y_rel && !z_rel)
    {
        return 1;
    }
    else if (x_rel && !y_rel && z_rel)
    {
        return 2;
    }
    else if (!x_rel && y_rel && z_rel)
    {
        return 3;
    }
    else if (!x_rel && y_rel && z_rel)
    {
        return 4;
    }
    else if (!x_rel && y_rel && !z_rel)
    {
        return 5;
    }
    else if (!x_rel && !y_rel && z_rel)
    {
        return 6;
    }
    else if (!x_rel && !y_rel && !z_rel)
    {
        return 7;
    }
}

// Generate octant by force.
std::shared_ptr<Octant> Octant::uNE() const // 111
{
    auto new_len = std::make_shared<Dim>(this->length / 2.0);
    auto new_len_adj = std::make_shared<Dim>(*new_len / 4.0);
    auto new_center = std::make_shared<Loc>(
        center.x + new_len_adj->x,
        center.y + new_len_adj->y,
        center.z + new_len_adj->z);
    return std::make_shared<Octant>(new_len, new_center);
}
std::shared_ptr<Octant> Octant::uNW() const // 011
{
    auto new_len = std::make_shared<Dim>(this->length / 2.0);
    auto new_len_adj = std::make_shared<Dim>(*new_len / 4.0);
    auto new_center = std::make_shared<Loc>(
        center.x - new_len_adj->x,
        center.y + new_len_adj->y,
        center.z + new_len_adj->z);
    return std::make_shared<Octant>(new_len, new_center);
}
std::shared_ptr<Octant> Octant::uSE() const // 101
{
    auto new_len = std::make_shared<Dim>(this->length / 2.0);
    auto new_len_adj = std::make_shared<Dim>(*new_len / 4.0);
    auto new_center = std::make_shared<Loc>(
        center.x + new_len_adj->x,
        center.y - new_len_adj->y,
        center.z + new_len_adj->z);
    return std::make_shared<Octant>(new_len, new_center);
}
std::shared_ptr<Octant> Octant::uSW() const // 001
{
    auto new_len = std::make_shared<Dim>(this->length / 2.0);
    auto new_len_adj = std::make_shared<Dim>(*new_len / 4.0);
    auto new_center = std::make_shared<Loc>(
        center.x - new_len_adj->x,
        center.y - new_len_adj->y,
        center.z + new_len_adj->z);
    return std::make_shared<Octant>(new_len, new_center);
}
std::shared_ptr<Octant> Octant::lNE() const // 110
{
    auto new_len = std::make_shared<Dim>(this->length / 2.0);
    auto new_len_adj = std::make_shared<Dim>(*new_len / 4.0);
    auto new_center = std::make_shared<Loc>(
        center.x + new_len_adj->x,
        center.y + new_len_adj->y,
        center.z - new_len_adj->z);
    return std::make_shared<Octant>(new_len, new_center);
}
std::shared_ptr<Octant> Octant::lNW() const // 010
{
    auto new_len = std::make_shared<Dim>(this->length / 2.0);
    auto new_len_adj = std::make_shared<Dim>(*new_len / 4.0);
    auto new_center = std::make_shared<Loc>(
        center.x - new_len_adj->x,
        center.y + new_len_adj->y,
        center.z - new_len_adj->z);
    return std::make_shared<Octant>(new_len, new_center);
}
std::shared_ptr<Octant> Octant::lSE() const // 100
{
    auto new_len = std::make_shared<Dim>(this->length / 2.0);
    auto new_len_adj = std::make_shared<Dim>(*new_len / 4.0);
    auto new_center = std::make_shared<Loc>(
        center.x + new_len_adj->x,
        center.y - new_len_adj->y,
        center.z - new_len_adj->z);
    return std::make_shared<Octant>(new_len, new_center);
}
std::shared_ptr<Octant> Octant::lSW() const // 000
{
    auto new_len = std::make_shared<Dim>(this->length / 2.0);
    auto new_len_adj = std::make_shared<Dim>(*new_len / 4.0);
    auto new_center = std::make_shared<Loc>(
        center.x - new_len_adj->x,
        center.y - new_len_adj->y,
        center.z - new_len_adj->z);
    return std::make_shared<Octant>(new_len, new_center);
}

/**
 *
 * Assignment operators
 *
**/
// Normal one
Octant& Octant::operator= (const Octant& other)
{
    Octant tmp(other);
    *this = std::move(tmp);
    return *this;
}
// Move operator
Octant& Octant::operator= (Octant&& other) noexcept
{
    this->length = other.GetLength();
    this->center = other.GetCenter();
    return *this;
}

/**
 *
 * Constructors and Destructors
 *
**/
Octant::Octant() : \
    length(ZeroDim),
    center(ZeroLoc)
{
}

Octant::Octant(const Dim& dim, const Loc& center_loc) :
    length(dim),
    center(center_loc)
{
}

Octant::Octant(
    const std::shared_ptr<Dim> spdim,
    const std::shared_ptr<Loc> spcloc
) : length(*spdim), center(*spcloc)
{
}

Octant::Octant(const Octant& other) :
    length(other.GetLength()),
    center(other.GetCenter())
{
}

Octant::Octant(Octant&& other) noexcept :
    length(other.GetLength()),
    center(other.GetCenter())
{
}

Octant::~Octant()
{
}



// Integrating with ostream
std::ostream& operator<< (std::ostream& os, const Octant& octant)
{
    std::stringstream ss;

    // Get Length
    ss << "Octant Dimension:" << std::endl;
    ss << octant.GetLength().to_str() << std::endl;

    // Get Center Position
    ss << "Octant Center:" << std::endl;
    ss << octant.GetCenter().to_str() << std::endl;

    os << ss.str();

    return os;
}
