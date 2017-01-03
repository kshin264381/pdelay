#!/usr/bin/env python

import math as m
import random as rnd

# Everything's in um scale
#
# Boltzmann's constant
k_B = 1.38064852e-23 * (1e6**2)
# Mass of electron
m_elec = 9.109e-31
# Permittivity of vacuum
eps_0 = 8.854e-12 * (1e-6**3)
# Permittivity of silicon
eps_si = 11.9 * eps_0
# charge of electron
q_e = -1.6e-19
# charge of hole
q_h = 1.6e-19
# Temperature
temperature = 300.0
# effective mass
m_eff_elec = 0.26*m_elec
m_eff_hole = 0.386*m_elec
# delta_t
delta_t = 1e-10
# mean free time
mft = rnd.random()*(1e-13 - 1e-14)

# thermal velocity
def thermal_vel(mass):
    return m.sqrt(3.0*k_B*temperature / mass)

# drift velocity
def drift_vel(bias, thickness, charge, mass):
    efield = -1.0*bias/thickness
    return efield*charge*delta_t / mass
    
# Debye Length
def debye_length(mass):
    doping = 1.3e12 / (1e4**3)
    Vt = thermal_vel(mass)
    return m.sqrt(eps_si*Vt/(q_h*doping))

# main function
if __name__ == "__main__":
    bias = -900 - (-1)
    thickness = 500
    
    print("Delta t:", delta_t, "s")
    print("Electron Drift velocity:",
          drift_vel(bias, thickness, q_e, m_eff_elec), "um/s")
    print("Electron mean free path:", 
        thermal_vel(m_eff_elec)*mft, "um")
    print("Electron Debye Length:", debye_length(m_eff_elec), "um")
    print("Hole Drift velocity:",
          drift_vel(bias, thickness, q_h, m_eff_hole), "um/s")
    print("Hole mean free path:", 
        thermal_vel(m_eff_hole)*mft, "um")
    print("Hole Debye Length:", debye_length(m_eff_hole), "um")

