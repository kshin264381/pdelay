/**
 * sim_space.cc
 *
 * Implementations for simulation space definition.
 *
 * Written by Taylor Shin
 * Sep. 6th, 2016
 *
**/

#include <memory>

#include "sim_space.h"

/**
 *
 * public stuff
 *
 *
**/

// Setting up normalization factor if needed
void sim_space::SetNormFactor(const fp_t& nx, const fp_t& ny, const fp_t& nz)
{
    // If all given factors are 1.0, why bother running this crap?
    if ( fp_equal(nx, FP_T(1.0)) && \
         fp_equal(ny, FP_T(1.0)) && \
         fp_equal(nz, FP_T(1.0)) )
    {
        return;
    }

    this->NormFactor.x = nx;
    this->NormFactor.y = ny;
    this->NormFactor.z = nz;

    // Updating dimension
    this->silicon_dimension->x_start /= this->NormFactor.x;
    this->silicon_dimension->x_end /= this->NormFactor.x;
    this->silicon_dimension->y_start /= this->NormFactor.y;
    this->silicon_dimension->y_end /= this->NormFactor.y;
    this->silicon_dimension->z_start /= this->NormFactor.z;
    this->silicon_dimension->z_end /= this->NormFactor.z;

    return;
}


// Get volume of current device as cm^3
fp_t sim_space::GetVolume()
{
    auto x_l = \
        this->silicon_dimension->x_end - \
        this->silicon_dimension->x_start;
    auto y_l = \
        this->silicon_dimension->y_end - \
        this->silicon_dimension->y_start;
    auto z_l = \
        this->silicon_dimension->z_end - \
        this->silicon_dimension->z_start;

    // Assuming um dimension
    return FP_T((x_l*1e-4)*(y_l*1e-4)*(z_l*1e-4));
}



// remove carrier from this->Carriers
//
int sim_space::remove_carr(spCarrier& carrier)
{
    auto Type = carrier->GetType();
    auto erase_size = this->Carriers.erase(carrier->GetIndex());

    if (!erase_size) {
        if (Type == std::string("Electron"))
            this->num_elec--;
        else if (Type == std::string("Hole"))
            this->num_hole--;
    }

    return 0;
}

// search carrier from this->Carriers
//
// returns 0 if success...
//
uint64_t sim_space::search_carr(const spCarrier& carrier, uint64_t& index)
{
    // DataType::Dict implementation (which is actually a std::map)
    auto it = this->Carriers.find(carrier->GetIndex());
    if (it != this->Carriers.end()) {
        index = carrier->GetIndex();
        return 0;
    }
    else {
        index = -1;
        return -1;
    }
}

// search carrier from this->CarriersToRemove
// returns index if success...
//
// --> Returning index is pointless with set or unordered_set datatype
// So, instead, returns 0 if given carrier exists and -1 not.
//
uint64_t sim_space::search_rem_carr(const spCarrier& carrier)
{
    if (this->CarriersToRemove.find(carrier)!=
        this->CarriersToRemove.end())
        return 0;
    else
        return -1;
}

// Is the carrier inside?
//
bool sim_space::is_inside(const spCarrier& carrier)
{
    auto carr_pos = carrier->GetPos();
    if ( (fp_mt<fp_t>(carr_pos.x, silicon_dimension->x_start) &&
        fp_lt<fp_t>(carr_pos.x, silicon_dimension->x_end) ) &&
        (fp_mt<fp_t>(carr_pos.y, silicon_dimension->y_start) &&
        fp_lt<fp_t>(carr_pos.y, silicon_dimension->y_end) ) &&
        (fp_mt<fp_t>(carr_pos.z, silicon_dimension->z_start) &&
         fp_lt<fp_t>(carr_pos.z, silicon_dimension->z_end)) )
        return true;
    else return false;
}

// Determines whether the carrier is collectable or not.
//
bool sim_space::is_collectable(const spCarrier& carrier, const Loc& prev_pos)
{
    //
    // In fact, just detecting whether the carrier is within the
    // scope of X - Y range is not enough to determine 
    // carrier collection at the electrodes.
    // Rather, we need to estimate whether the carrier actually 
    // traveled through the electrode or not.
    //

    // Cut out if prev_pos is out of scope.
    fp_t prev_x = prev_pos.x;
    fp_t prev_y = prev_pos.y;
    fp_t prev_z = prev_pos.z;

    if ((fp_mteq<fp_t>(prev_y, this->silicon_dimension->y_start) &&
        fp_lteq<fp_t>(prev_y, this->silicon_dimension->y_end)) &&
        (fp_mteq<fp_t>(prev_x, this->silicon_dimension->x_start) &&
        fp_lteq<fp_t>(prev_x, this->silicon_dimension->x_end))) {
        if ((fp_lt<fp_t>(prev_z, this->silicon_dimension->z_start) ||
            fp_mt<fp_t>(prev_z, this->silicon_dimension->z_end)) )
        {
            return true;
        }
    }
    else {
        std::cerr << "This carrier, " \
            << carrier->GetID() \
            << " has been found at outside of the simulation dimension." \
            << std::endl \
            << carrier->GetPosStr() \
            << std::endl;
        exit(-1);
    }

    auto curr_pos = carrier->GetPos();

    auto plane_start = \
        XY{ this->silicon_dimension->x_start,
        this->silicon_dimension->x_end };
    auto plane_end = \
        XY{ this->silicon_dimension->y_start,
        this->silicon_dimension->y_end };
    auto anode_loc = this->silicon_dimension->z_start;
    auto cathode_loc = this->silicon_dimension->z_end;
    std::string plane_axis("xy");

    // Determine if the carrier is close to anode or cathode.
    auto distA = fabs(curr_pos.z - anode_loc);
    auto distC = fabs(curr_pos.z - cathode_loc);

    auto pass_anode = fp_chk_cross_3D<fp_t>(
        prev_pos,
        curr_pos, plane_axis, plane_start,
        plane_end, anode_loc);
    auto pass_cathode = fp_chk_cross_3D<fp_t>(
        prev_pos,
        curr_pos, plane_axis, plane_start,
        plane_end, cathode_loc);

    if (pass_anode || pass_cathode) {
        return true;
    }
    else if (pass_anode && pass_cathode) {
        std::cerr \
            << "Passing both anode and cathode?" \
            << std::endl;
        return false;
    }
    // Still stays inside... except nan situation.
    else if (!(pass_anode && pass_cathode)) {
        return false;
    }
    else {
        return false;
    }
}






// Registers carriers to remove
// --> neglect input carrier if it is already registered and returns 0
// other than that, returns the index of carrier at CarriersToRemove
//
int sim_space::add_to_rem(const spCarrier& carrier)
{
    this->CarriersToRemove.insert(carrier);

    return 0;
}


// Some delta_t control
// Estimates delta_t for next step.
//
// tau < nu / sqrt(G*density)
// --> where G(Gravitational constant) 
//        was replaced by Coulomb's constant.
//
void sim_space::SetVariableDeltaT()
{
    // Density: /m^3
    auto Density = \
        this->Carriers.size() / \
        (this->GetVolume()*1e-2*1e-2*1e-2);

    // Stability & accuracy constant. Default is 0.03
    //
    fp_t nu = 0.03;
#ifndef __MULTIPRECISION__
    fp_t new_delta_t = \
        nu / std::sqrt(k_e*Density);
#else
    fp_t new_delta_t = \
        nu / boost::math::sqrt(k_e*Density);
#endif
    this->delta_t = new_delta_t;
}

// Prints out simulation status
void sim_space::ShowSimStatus()
{
    std::stringstream sst;

    // show current status
    //this->current_sim_time = std::clock();
    this->current_sim_time = CLOCK_NOW;
    sst.str(std::string());
    sst.clear(); // clearing out any eof or fail stuff..
    sst << "*** "<< sim_algorithm_str <<" Step [" \
        << (this->sim_step) << "] ***" \
        << std::endl \
        << "Elapsed Time: " \
        << this->print_elapsed_time() \
        << std::endl \
        << "Elapsed Simulation Time: " \
        << this->format_time(this->current_sim_time - this->start_time) \
        << std::endl \
        << "Delta T at this step: " \
        << this->delta_t << " seconds" \
        << std::endl \
        << "Total # of carriers: " \
        << this->Carriers.size() \
        << std::endl \
        << "# of Electrons: " \
        << this->num_elec \
        << std::endl \
        << "# of Holes: " \
        << this->num_hole \
        << std::endl \
        << "Collected carriers so far: " \
        << this->collected_carriers \
        << std::endl \
        << "Lost carriers so far: " \
        << this->lost_carriers \
        << std::endl;
    this->SimOutput.PutString(sst.str());
    this->SimOutput.Print();
    this->SimOutput.FlushText();
}


// Show finishing message
void sim_space::SimFinishMessage()
{
    // Mark the end time.
    auto end_time = CLOCK_NOW;
    auto sim_duration = \
        this->format_time(end_time - this->start_time);

    // Print out elapsed simulation time message.
    std::stringstream st_ss;
    st_ss \
        << "Simulation Finished!!!" \
        << std::endl \
        << "It took " \
        << sim_duration \
        << std::endl;
    SimOutput.PutString(st_ss.str());
    SimOutput.Print();

    return;
}




