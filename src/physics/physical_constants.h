/**
 *
 * physical_constants.h
 *
 * Contains all the physical constants and
 * typedefs for nbody calculation.
 *
**/

#ifndef __physical_constants_h__
#define __physical_constants_h__

#include "fputils.h"
#include "materials.h"

/* handling missing M_PI */
#ifndef M_PI
#include <cmath>
#ifndef _MSC_VER
#define M_PI 4 * std::atan(1.0)
#else
// --> VS requires a strict constant.
#define M_PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062
#endif
#endif

// boost units library
#include <boost/units/systems/si/length.hpp>
#include <boost/units/systems/si/force.hpp>
#include <boost/units/systems/si/electric_potential.hpp>
#include <boost/units/systems/si/current.hpp>
#include <boost/units/systems/si/io.hpp>

using namespace boost::units;
using namespace boost::units::si;

// unit conversions
#define fm 1e-15*meter
#define pm 1e-12*meter
#define nm 1e-9*meter
#define um 1e-6*meter
/* VS Express has mm in wingdi.h to avoid the conflict, we changed mm to milm */
#define milm 1e-3*meter
#define cm 1e-2*meter
#define km 1e3*meter
#define Mm 1e6*meter
#define Gm 1e9*meter

// Some typedefs to shorten code...
using CoordSet = __tuple_3D__<fp_t>;
using Loc      = __tuple_3D__<fp_t>;
using Pos      = __tuple_3D__<fp_t>;
using Dim      = __tuple_3D__<fp_t>;
using Vel      = __tuple_3D__<fp_t>;
using Force    = __tuple_3D__<fp_t>;
using Pot      = __tuple_3D__<fp_t>;
using EField   = __tuple_3D__<fp_t>;

// Zero defults for those 3D coordinates.
#define Zero3D     CoordSet{ FP_T(0.0), FP_T(0.0), FP_T(0.0) }
#define ZeroLoc    Loc{ FP_T(0.0), FP_T(0.0), FP_T(0.0) }
#define ZeroPos    Pos{ FP_T(0.0), FP_T(0.0), FP_T(0.0) }
#define ZeroDim    Dim{ FP_T(0.0), FP_T(0.0), FP_T(0.0) }
#define ZeroVel    Vel{ FP_T(0.0), FP_T(0.0), FP_T(0.0) }
#define ZeroForce  Force{ FP_T(0.0), FP_T(0.0), FP_T(0.0) }
#define ZeroPot    Pot{ FP_T(0.0), FP_T(0.0), FP_T(0.0) }
#define ZeroEField EField{ FP_T(0.0), FP_T(0.0), FP_T(0.0) }

/**
 * Physical constants in MKS system.
 *
**/
// Speed of light (m/s)
constexpr fp_t c_ = 299792458.0;
// Speed of light (cm/s)
constexpr fp_t c_cgs = c_ * 100.0;
// charge of a hole (C)
constexpr fp_t q_h = 1.60217662e-19;
// Charge of an electron (C)
constexpr fp_t q_e = (-1.0)*q_h;
// Boltzmann's Constant (J/K)
constexpr fp_t k_B = 1.38064852e-23;
// Boltzmann's Constant (eV/K)
constexpr fp_t k_B_eV = 8.617332478e-5;
// Mass of an electron (kg)
constexpr fp_t m_elec = 9.10938356e-31;
// Mass of an electron (g)
constexpr fp_t m_elec_cgs = m_elec*1000.0;
// Permittivity of vacuum (F/m)
constexpr fp_t eps_0 = 8.8541878176e-12;
// Permittivity of vacuum (F/cm)
constexpr fp_t eps_0_cgs = eps_0/100.0;
// Permiability of vacuum (N/A2 = kg m /s /A^2)
constexpr fp_t mu_0 = 1.2566370614e-6;
// Permiability of vacuum (g cm /s /A^2)
constexpr fp_t mu_0_cgs = 4*M_PI/(c_cgs*c_cgs);
// Permittivity of Silicon (F/m with adjustment..)
constexpr fp_t eps_si = 11.9 * eps_0;
constexpr fp_t eps_si_cgs = 11.9 * eps_0_cgs;
// Coulomb's constant (N m^2 C^{-2})
constexpr fp_t k_e = c_ * c_ * mu_0 / (4.0*M_PI);
// Coulomb's constant (ergs cm^2 C^{-2})
constexpr fp_t k_e_cgs = c_cgs * c_cgs * mu_0_cgs / (4.0*M_PI);

// Thermal velocity (m/s)
//
template <typename T>
T v_therm(const T& temperature, const T& mass)
{
    return sqrt(3.0 * k_B * temperature / mass);
}

// Thermal voltage (V)
//
// V_T = k_B*T/q
//
template <typename T>
T volt_therm(const T& temperature)
{
    return k_B * temperature / q_h;
}

// Returns effective mass... for crystalline silicon only, at this moment.
//
class EffMass {
private:
    Materials::MatData MaterialDatabase;
    std::string MaterialName;

public:
    fp_t Electron();
    fp_t Hole();

    // Setting up material name
    void SetMaterial(std::string MatName)
    { this->MaterialName = MatName; }

    // Constructors and Destructors
    EffMass() : MaterialName("Silicon")
    { this->MaterialDatabase = Materials::MatData(); }
    EffMass(fp_t LatTemp) : MaterialName("Silicon")
    { this->MaterialDatabase = Materials::MatData(LatTemp); }
    EffMass(std::string MatName, fp_t LatTemp) : EffMass(LatTemp)
    { this->MaterialName = MatName; }
    EffMass(const char* mat_data_filename)
    {
        this->MaterialDatabase = \
            Materials::MatData(mat_data_filename);
        // Setting up default material to silicon anyway
        this->MaterialName = "Silicon";
    }
    virtual ~EffMass() {;}
};

#endif /* Include guard */
