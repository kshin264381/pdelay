#ifndef __carrier_h__
#define __carrier_h__

#include <cmath>
#include <limits>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>

#include "fputils.h"
#include "Utils.h"
#include "physical_constants.h"

/**
 * carrier.h
 *
 * Carrier (electron/holes) description in C++ for 
 * N-body simulation.
 *
 * Written by Taylor Shin
 * Feb. 24th, 2016
 *
**/

// misc stuff
cfp_t rand_delta_min = 1e-6;
cfp_t rand_delta_max = 1e-5;

// Some definitions
constexpr short CARR_T_HOLE = 1;
constexpr short CARR_T_ELECTRON = 0;

/**
 * Carrier (class)
 * 
 * Contains information of each carrier.
 * Location (X,Y,Z), velocity(VX,VY,VZ), and
 * calculates force from other carriers.
 *
**/
class Carrier
{
private:
    fp_t charge;          // Charge of a carrier (C)
    Loc position;         // Position in Cartesian coordinate set (um)
    Vel velocity;         // Velocity in Cartesian coordinate set (um/s)
    Force force;          // Force in Cartesian coordinate set (m/s^2)
    fp_t mass;            // Effective mass (kg)
    uint64_t index;       // Index... just for bureaucracy stuff...
    std::string CarrID;   // ID... another bureaucracy stuff...
    std::string CarrType; // carrier type in string

public:
    void setType();
    void setType(const std::string& new_type);
    void setID();
    void setMass();

    // Setting up parameters manually
    void SetCharge(fp_t new_charge) { this->charge = new_charge; }
    void SetCharge(); // Setting charge by carrier type.
    void SetType(std::string typestr);
    void SetPos(Loc new_position) { this->position = new_position; }
    void SetVel(Vel new_velocity) { this->velocity = new_velocity; }
    void SetForce(Force new_force) { this->force = new_force; }
    void SetMass(fp_t new_mass) { this->mass = new_mass; }
    void SetIndex(uint64_t new_index) 
    {
        this->index = new_index;
        this->setType();
        this->setID();
    }
    void Set(
        fp_t new_charge,
        Loc new_position,
        Vel new_velocity,
        fp_t new_mass,
        uint64_t new_index)
    {
        this->SetCharge(new_charge);
        this->SetPos(new_position);
        this->SetVel(new_velocity);
        this->SetMass(new_mass);
        this->SetIndex(new_index);
    }
    void Set(
        std::string carr_type,
        Loc new_position,
        Vel new_velocity,
        fp_t new_mass,
        uint64_t new_index)
    {
        this->SetType(carr_type);
        this->position = new_position;
        this->velocity = new_velocity;
        this->mass = new_mass;
        this->index = new_index;

        this->setType();
        this->setID();
    }

    void ResetVelnForce()
    {
        this->velocity = ZeroVel;
        this->force = ZeroForce;
    }

    // Retrieve properties
    fp_t GetCharge() const { return this->charge; }
    Loc GetPos() const { return this->position; }
    std::string GetPosStr() const;
    Vel GetVel() const { return this->velocity; }
    std::string GetVelStr() const;
    Force GetForce() const { return this->force; }
    std::string GetForceStr() const;
    fp_t GetMass() const { return this->mass; }
    uint64_t GetIndex() const { return this->index; }
    std::string GetID() const { return this->CarrID; }
    std::string GetType() const { return this->CarrType; }
    short GetTypeI();

    // Add Force
    void AddForce(const Force& ext_force)
    { this->force += ext_force; }

    // Update velocity
    void UpdateVel(const fp_t& time_delta)
    { this->velocity += (this->force/this->mass)*time_delta; }
    // with modifier: Remember that Force is stored as MKS all the time.
    void UpdateVel(const fp_t& time_delta, const fp_t& modifier)
    { this->UpdateVel(time_delta*modifier); }

    // Update location with time (must be sec. unit...)
    void UpdatePos(const fp_t& time_delta)
    { this->position += (this->velocity * time_delta); }

    // Position adjustment
    void AdjPosDelta(Loc DeltaPos)
    { this->position += DeltaPos; }

    // Some operator overloading stuff
    bool operator==(const Carrier& other_carrier) const
    { return (this->index == other_carrier.GetIndex()); }
    bool operator!=(const Carrier& other_carrier) const
    { return (this->index != other_carrier.GetIndex()); }

    // Copy assignment operators
    Carrier& operator= (const Carrier& other)
    {
        Carrier tmp(other);
        *this = std::move(tmp);
        return *this;
    }
    Carrier& operator= (Carrier&& other) noexcept
    {
        this->Set(
            other.GetCharge(),
            other.GetPos(),
            other.GetVel(),
            other.GetMass(),
            other.GetIndex());
        this->setID();

        return *this;
    }

    // Some operators for BHTree insertion
    Carrier& operator+ (Carrier Other)
    {
        if (Other != *this) {
            this->charge += Other.GetCharge();
            this->position = (this->position + Other.GetPos())/2.0;
            this->velocity += Other.GetVel();
            this->force += Other.GetForce();
            this->mass += Other.GetMass();
            // Just leave index system for now...
        }
        return *this;
    }

    // Constructors and Destructors
    Carrier();
    Carrier(
        std::string carrier_type,
        Loc position,
        Vel velocity,
        fp_t mass,
        uint64_t carr_num);
    Carrier(
        fp_t charge,
        Loc position,
        Vel velocity,
        fp_t mass,
        uint64_t carr_num);
    Carrier(const Carrier& other);
    Carrier(Carrier&& other) noexcept;
    virtual ~Carrier();
};


// Integrating with ostream
std::ostream& operator<< (std::ostream& os, const Carrier& carrier);

#endif /* include guard */
