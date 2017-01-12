/**
 * carrier.cc
 *
 * Carrier (electron/holes) description in C++ for 
 * N-body simulation.
 *
 * Method implementations!!
 *
 * Written by Taylor Shin
 * Feb. 24th, 2016
 *
**/

#include "carrier.h"

/**
 * Carrier
 *
 * Private part
 *
**/
// Probably nothing to be placed here...

/**
 * Carrier
 *
 * Public part
 *
**/

void Carrier::setType()
{
    if (fp_lt<fp_t>(this->charge, FP_T(0.0))) {
        this->CarrType = "Electron";
    }
    else {
        this->CarrType = "Hole";
    }

    return;
}

void Carrier::setType(const std::string& new_type)
{
    if (str_to_lower(new_type) != "electron" || str_to_lower(new_type) != "hole") {
        std::cerr << "Carrier::setType: Can't determine type!! must be electron or hole!!" << std::endl;
        exit(-1);
    }

    this->CarrType = new_type;
}

void Carrier::setID()
{
    this->setType();
    this->CarrID = std::string(
        std::string("[") + fp_t_to_string(this->index) +
        std::string("]") + this->CarrType);
}

void Carrier::setMass()
{
    // A super crude way to set mass. Don't use it if 
    // a proper way to estimate electric field.
    if (this->CarrType == "Electron") {
        this->mass = \
            m_elec*(1.045 + 4.5 * 1e-4)*300.0;
    }
    else if (this->CarrType == "Hole") {
        this->mass = \
            m_elec*(0.523 + 1.4e-3 * 300.0 - \
                1.48e-6 * 300.0 * 300.0);
    }
}

void Carrier::SetType(std::string typestr)
{
    if (typestr == "Electron")
        this->SetCharge(q_e);
    else if (typestr == "Hole")
        this->SetCharge(q_h);
    else {
        std::cerr << \
            "Carrier: Invalid carrier type." << \
            " Either provide Electron or Hole!!!" << \
            std::endl;
        this->SetCharge(FP_T(0.0));
    }
    this->setType();
}

// Returns Pos/Vel/Force as string.
std::string Carrier::GetPosStr() const
{
    std::stringstream ss;
    ss << "x: " << this->position(0) << " " \
        << "y: " << this->position(1) << " " \
        << "z: " << this->position(2);

    return ss.str();
}

std::string Carrier::GetVelStr() const
{
    std::stringstream ss;
    ss << "vx: " << this->velocity(0) << " " \
        << "vy: " << this->velocity(1) << " " \
        << "vz: " << this->velocity(2);

    return ss.str();
}

std::string Carrier::GetForceStr() const
{
    std::stringstream ss;
    ss << "Fx: " << this->force(0) << " " \
        << "Fy: " << this->force(1) << " " \
        << "Fz: " << this->force(2);

    return ss.str();

}

short Carrier::GetTypeI()
{
    if (fp_lt(this->charge, FP_T(0.0))) return CARR_T_ELECTRON;
    else if (fp_mt(this->charge, FP_T(0.0))) return CARR_T_HOLE;
    else return 100;
}

//
// Force calculation methods
//

/**
 *
 * Carrier - Constructors and Destructors
 *
**/
Carrier::Carrier() : \
    charge(FP_T(0.0)),
    position(ZeroLoc),
    velocity(ZeroVel),
    force(ZeroForce),
    index(0),
    mass(m_elec),
    CarrID({}),
    CarrType({})
{
    this->setID();
}

Carrier::Carrier(
    std::string carrier_type,
    Loc position,
    Vel velocity,
    fp_t mass,
    uint64_t carr_num) : Carrier()
{
    this->SetPos(position);
    this->SetVel(velocity);
    this->SetMass(mass);
    this->SetIndex(carr_num);

    if (carrier_type == "hole")
        this->SetCharge(q_h);
    else if (carrier_type == "electron")
        this->SetCharge(q_e);
    else
        this->SetCharge(q_e);

    this->setID();
}

Carrier::Carrier(
    fp_t charge,
    Loc position,
    Vel velocity,
    fp_t mass,
    uint64_t carr_num) : Carrier()
{
    this->Set(charge, position, velocity, mass, carr_num);
    this->setID();
}

Carrier::Carrier(const Carrier& other) : \
    charge(other.GetCharge()),
    position(other.GetPos()),
    velocity(other.GetVel()),
    force(other.GetForce()),
    mass(other.GetMass()),
    index(other.GetIndex()),
    CarrID(other.GetID()),
    CarrType(other.GetType())
{
}

Carrier::Carrier(Carrier&& other) noexcept: \
    charge(other.GetCharge()),
    position(other.GetPos()),
    velocity(other.GetVel()),
    force(other.GetForce()),
    mass(other.GetMass()),
    index(other.GetIndex()),
    CarrID(other.GetID()),
    CarrType(other.GetType())
{
}

Carrier::~Carrier()
{
    // Nothing to do now.
}


// Integrating with ostream
std::ostream& operator<< (std::ostream& os, const Carrier& carrier)
{
    std::stringstream ss;

    // Carrier ID
    ss << carrier.GetID() << std::endl;

    // Position
    ss << "Position: " << carrier.GetPosStr() << std::endl;

    // Velocity
    ss << "Velocity: " << carrier.GetVelStr() << std::endl;

    // Force
    ss << "Force:" << carrier.GetForceStr() << std::endl;

    ss << std::endl;

    os << ss.str();

    return os;
}

