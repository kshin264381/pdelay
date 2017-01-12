/**
 * nbody.cc
 *
 * N-body algorithm implementation.
 *
 * Written by Taylor Shin
 * Feb. 25th, 2016
 *
**/

#include "fputils.h"
#include "nbody.h"

/**
 * NBody
 *
 * Protected part
 *
**/

/**********************************************************/
/**********************************************************/
//
// Update force on a given carrier and defines its new velocity.
// --> Timed version
//
void NBody::update_force(spCarrier& carrier, const fp_t& tau)
{
    auto totalForce = ZeroForce;

    // Estimate Coulombic force.
    //
    //fp_uint_t carr_index = this->Carriers.size();
    auto seg_coulomb_force = ZeroForce;

    auto it = std::begin(this->Carriers);
    auto end = std::end(this->Carriers);

    for (it; it != end; ++it) {
        if (it->second != carrier)
            seg_coulomb_force += this->CoulombForce(carrier, it->second);
    }

    // Update total Force..
    totalForce += seg_coulomb_force;

    // Now update the drift force as well.
    totalForce += this->DriftForce(carrier);

    // Ok, assign force to current carrier.
    //carrier->SetForce(totalForce*this->len_scale_f);
    carrier->SetForce(totalForce);
    //carrier->UpdateVel(this->delta_t);

    // Gotta match the unit when calculating the velocity.
    // --> (m/s to um/s)
    Vel carr_temp_velocity = \
        ((totalForce / carrier->GetMass()) * tau) * \
        this->len_scale_f;
    carrier->SetVel(carr_temp_velocity);

    return;
}


// sub routine to implement OpenMP for update_all_force
void NBody::update_all_force_sub(
    const uint64_t& istart,
    const uint64_t& ipoints,
    const fp_t& tau)
{
    auto it = std::begin(this->Carriers);
    auto end = std::begin(this->Carriers);
    std::advance(it, istart);
    std::advance(end, istart + ipoints);

    for (it; it != end; ++it) {
        this->update_force(it->second, tau);

#pragma omp critical
    {
        this->ForceCal.Update();
    }
    } /* for (it; it!=end; it++) */

    return;
}

// Force calculation
// --> Timed edition.
void NBody::update_all_force(const fp_t& tau)
{
    // update force on carriers
    this->ForceCal = ProgressBar("Force Est.", this->Carriers.size());
#ifdef _OPENMP
    uint64_t ithread, nthreads, ipoints, istart, npoints;
    fp_t openmp_tau = tau;
#pragma omp parallel private(ithread, nthreads, ipoints, istart, npoints)
    {
        npoints = this->Carriers.size();
        ithread = omp_get_thread_num();
        nthreads = omp_get_num_threads();
        ipoints = npoints / nthreads;
        istart = ithread * ipoints;
        if (ithread == nthreads - 1)
            ipoints = npoints - istart;
        this->update_all_force_sub(istart, ipoints, openmp_tau);
    } /* #pragma omp parallel */
#pragma omp barrier
#else /* #ifdef _OPENMP */
    for (auto carr : this->Carriers) {
        this->update_force(carr.second, tau);
        this->ForceCal.Update();
    } /* for (auto carr : this->Carriers) */
      //std::cout << std::endl;
#endif /* #ifdef _OPENMP */

    return;
}
/**********************************************************/



/**********************************************************/
// Update carrier position
//
void NBody::update_carr_position(spCarrier& carrier)
{
    // Implemented Debye length to solve the "too close carriers"
    // problem.

    // Back up previous position before update.
    Loc prev_pos = carrier->GetPos();
    carrier->UpdatePos(this->delta_t);
    // Update positions with mean free path estimation.
    this->MFPAdj(carrier);
    // New position...
    Loc new_pos = carrier->GetPos();

    // Checking the new position for NaN...
    if ( new_pos._isnan() ) {
        if ( !prev_pos._isnan() ) {
            std::cerr << carrier->GetID() \
                << " became nan..." \
                << std::endl;
        }

        std::cerr \
            << "nan detected!! Printing out Carrier Info." \
            << std::endl \
            << "Carrier Index: " << carrier->GetID() \
            << std::endl \
            << "Force (N): " << std::endl \
            << "(" << carrier->GetForce().x \
            << ", " << carrier->GetForce().y \
            << ", " << carrier->GetForce().z \
            << ")" << std::endl \
            << "Velocity (um/s):" << std::endl \
            << "(" << carrier->GetVel().x \
            << ", " << carrier->GetVel().y \
            << ", " << carrier->GetVel().z \
            << ")" << std::endl \
            << "Position (um):" << std::endl \
            << "(" << carrier->GetPos().x \
            << ", " << carrier->GetPos().y \
            << ", " << carrier->GetPos().z \
            << ")" << std::endl;
        this->write_lost_carrier_info(carrier);
        this->lost_carriers++;
        this->add_to_rem(carrier);

        return;
    }

    // Determine if the carrier is within the device or not.
    if (!is_inside(carrier)) {
        // If the carrier ended up out of device but seems to be
        // collected at the electrodes at z axis.
        //
        if ( this->is_collectable(carrier, prev_pos) ) {
            this->write_collected_carrier_info(carrier);
            this->collected_carriers++;
            this->add_to_rem(carrier);
        }

        return;
    }

}

// Same with update_carr_position but with given time.
void NBody::update_carr_position(spCarrier& carrier, const fp_t& tau)
{
    // Implemented Debye length to solve the "too close carriers"
    // problem.

    // Back up previous position before update.
    Loc prev_pos = carrier->GetPos();
    carrier->UpdatePos(tau);
    // Update positions with mean free path estimation.
    this->MFPAdj(carrier, tau);
    // Apply diffusion
    this->Diffusion(carrier, tau);
    // New position...
    Loc new_pos = carrier->GetPos();

    // Checking the new position for NaN...
    if (new_pos._isnan()) {
        if (!prev_pos._isnan()) {
            std::cerr << carrier->GetID() \
                << " became nan..." \
                << std::endl;
        }

        std::cerr \
            << "nan detected!! Printing out Carrier Info." \
            << std::endl \
            << "Carrier Index: " << carrier->GetID() \
            << std::endl \
            << "Force (N): " << std::endl \
            << "(" << carrier->GetForce().x \
            << ", " << carrier->GetForce().y \
            << ", " << carrier->GetForce().z \
            << ")" << std::endl \
            << "Velocity (um/s):" << std::endl \
            << "(" << carrier->GetVel().x \
            << ", " << carrier->GetVel().y \
            << ", " << carrier->GetVel().z \
            << ")" << std::endl \
            << "Position (um):" << std::endl \
            << "(" << carrier->GetPos().x \
            << ", " << carrier->GetPos().y \
            << ", " << carrier->GetPos().z \
            << ")" << std::endl;
        this->write_lost_carrier_info(carrier);
        this->lost_carriers++;
        this->add_to_rem(carrier);

        return;
    }

    // Determine if the carrier is within the device or not.
    if (!is_inside(carrier)) {
        // If the carrier ended up out of device but seems to be
        // collected at the electrodes at z axis.
        //
        if (this->is_collectable(carrier, prev_pos)) {
            this->write_collected_carrier_info(carrier);
            this->collected_carriers++;
            this->add_to_rem(carrier);
        }
        return;
    }
}

// Segmentation of update_all_carr_position (OpenMP)
// --> timed version
//
void NBody::update_all_carr_position_sub(
    uint64_t& istart, uint64_t& ipoints, const fp_t& tau)
{
    auto it = std::begin(this->Carriers);
    std::advance(it, istart);
    auto end = std::begin(this->Carriers);
    std::advance(end, istart + ipoints);

    for (it; it != end; ++it) {
        this->update_carr_position(it->second, tau);

#pragma omp critical
        {
            this->LocCal.Update();
        }

    } /* for (it; it != end; ++it) */

    return;
}

// Batch process to update positions of all carriers within scope.
// --> timed version
//
void NBody::update_all_carr_position(const fp_t& tau)
{
    this->LocCal = ProgressBar(
        "Loc Update.", this->Carriers.size());

    //this->CarriersToRemove = CarrierList();
    if (this->CarriersToRemove.size()) {
        this->CarriersToRemove.clear();
    }

#ifdef _OPENMP
    uint64_t ith, nth, ipoints, istart, npoints;

#pragma omp parallel private(ith, nth, ipoints, istart, npoints)
    {
        npoints = this->Carriers.size();
        ith = omp_get_thread_num();
        nth = omp_get_num_threads();
        ipoints = npoints / nth;
        istart = ith * ipoints;
        if (ith == nth - 1) ipoints = npoints - istart;
        this->update_all_carr_position_sub(istart, ipoints, tau);
    }  /* #pragma omp parallel */

#else

    auto it = std::begin(this->Carriers);
    while (true) {
        this->update_carr_position(it->second, tau);
        this->LocCal.Update();
        if (it != std::end(this->Carriers)) ++it;
        else break;
    }

#endif

    // Remove marked carriers.
    for (auto ctr : this->CarriersToRemove) {
        this->remove_carr(ctr);
    }
    return;
}

/**********************************************************/


// Initialize simulation.
int NBody::InitSim()
{
    // Initialize monitor variables.
    this->sim_step = 0;
    this->collected_carriers = 0;
    this->SimOutput = DecorOutput(65);
    this->SimOutput.SetType(2);
    this->current_sim_time = std::clock();

    // Prepare logfile and write preamble.
    this->simulation_logfile.open(this->logfile_name, ios::out | ios::app);
    std::stringstream ss_preamble;

    if (this->container_file) {
        ss_preamble \
            << "***** pdelay simulation result from " \
            << this->container_file << " *****" \
            << std::endl << std::endl;
    }
    else if (this->db_file) {
        ss_preamble \
            << "***** pdelay continuation simulation result from " \
            << this->db_file << " *****" \
            << std::endl << std::endl;
        this->pass_forcecal = true;
    } /* if (this->container_file) */

    if (this->simulation_logfile.is_open()) {
        this->simulation_logfile << ss_preamble.str();
        this->simulation_logfile.close();
    }

    // Preparing visual report tool.
    this->SetOutputMode(this->log_carrier_data_format);

    // Setting up timer... and marking the star time.
    this->start_time = current_sim_time;

    // Setting up maximum number of threads that the entire algorithm
    // will use.
#ifdef _OPENMP
    omp_set_num_threads(this->processes);
#endif

    // Read in data and generate carrier list.
    // tarball input case
    if (!this->continued) {
        this->generate_carriers();
    }
    // Database input case from previous calculation.
    if (this->continued && this->db_file) {
        this->generate_carriers_db();
    }

    // Write carrier locations
    if (this->gen_carr_log) {
        std::cout << "Writing initial carrier locations to: " \
            << this->GetFileName() \
            << std::endl;
        // Write initial carrier location.
        // (Doesn't contain any velocity and force.)
        this->WriteCarriersInit();
    }

    return 0;
}


// Prints out simulation status
void NBody::ShowSimStatus()
{
    std::stringstream sst;

    // show current status
    this->current_sim_time = std::clock();
    sst.str(std::string());
    sst.clear(); // clearing out any eof or fail stuff..
    sst << "*** (One-To-One) Step [" \
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



void NBody::SimFinishMessage()
{
    // Mark the end time.
    auto end_time = std::clock();
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


/**
 *
 * Drift and Kick algorithm
 *
 * Adopted from Time stepping N-body simulations.
 *
 **/
// Select Algorithm
void NBody::Select()
{
    this->SetVariableDeltaT();
    this->forced_delta_t = false;
}

void NBody::Select(const fp_t& tau)
{
    this->delta_t = tau;
    this->forced_delta_t = true;
}

// Drift Algorithm
//
// --> Simply put, updates carrier location by its speed for
//     the given time (tau.)
//
int NBody::Drift(const fp_t& tau)
{
    this->update_all_carr_position(tau);

    return 0;
}


// Kick Algorithm
//
// --> Obtains speed from accleration (which should be given
//     from force estimation) for the given time (tau.)
//
int NBody::Kick(const fp_t& tau)
{
    this->update_all_force(tau);

    return 0;
}



/**
 *
 * NBody
 *
 * Public part
 *
**/

// Runs the actual simulation.
//
int NBody::Run()
{
    // Announcing model
    std::cout << "Using Single Shot N-Body simulation model!!" << std::endl;

    // Initialize
    this->InitSim();

    // Now actually runs the simulation
    while (this->Carriers.size()) {
        // Setting up delta_t
        if (!this->forced_delta_t) this->Select();

        // Bulk Recombination (with substrate traps and acceptor/doners)
        this->BulkRecombination();
        if (!this->Carriers.size()) break;

        // Prints out current simulation status to stdout.
        this->ShowSimStatus();

        if (!this->pass_forcecal) {
            this->Kick(this->delta_t);
        } /* if (!pass_forcecal) */
        else {
            this->pass_forcecal = false;
        }

        // Update location of carriers (Implementing with OpenMP)
        this->Drift(this->delta_t);

        // Write carrier location to log file.
        this->WriteCarriers();

        // Increase step # by 1
        this->sim_step++;
        this->elapsed_time += this->delta_t;
    }

    this->SimOutput.FlushText();

    // Wrapping up.
    this->SimFinishMessage();
    return 0;
}

// SDKD
int NBody::RunSDKD()
{
    // Select Drift Kick Drift mode.
    //
    //

    // Announcing model
    std::cout << "Using SDKD N-Body simulation model!!" << std::endl;

    // Initialize
    this->InitSim();

    while (this->Carriers.size()) {
        // Select
        if (!this->forced_delta_t) this->Select();
        // else this->Select(this->delta_t);

        // Bulk Recombination (with substrate traps and acceptor/doners)
        this->BulkRecombination();
        if (!this->Carriers.size()) break;

        // Prints out current simulation status to stdout.
        this->ShowSimStatus();

        // 1st half drift
        if (this->sim_step)
            this->Drift(this->delta_t / 2.0);

        // Kick
        this->Kick(this->delta_t);

        // 2nd half drift
        this->Drift(this->delta_t / 2.0);

        // Write carrier location to log file.
        this->WriteCarriers();

        // Increase step # by 1
        this->sim_step++;
        this->elapsed_time += this->delta_t;
    }

    this->SimOutput.FlushText();
    // Wrapping up.
    this->SimFinishMessage();
    return 0;
}


// SKDK
int NBody::RunSKDK()
{
    // Select Kick Drift Kick mode.
    //
    //

    // Announcing model
    std::cout << "Using SKDK N-Body simulation model!!" << std::endl;

    // Initialize
    this->InitSim();

    while (this->Carriers.size()) {
        // Select
        if (!this->forced_delta_t) this->Select();
        // else this->Select(this->delta_t);

        // Bulk Recombination (with substrate traps and acceptor/doners)
        this->BulkRecombination();
        if (!this->Carriers.size()) break;

        // Prints out current simulation status to stdout.
        this->ShowSimStatus();

        // 1st half kick
        this->Kick(this->delta_t / 2.0);

        // Drift
        if (this->sim_step != 0)
            this->Drift(this->delta_t);
        else
            this->Drift(this->delta_t / 2.0);

        // 2nd half kick
        this->Kick(this->delta_t / 2.0);

        // Write carrier location to log file.
        this->WriteCarriers();

        // Increase step # by 1
        this->sim_step++;
        this->elapsed_time += this->delta_t;
    }

    this->SimOutput.FlushText();
    // Wrapping up.
    this->SimFinishMessage();
    return 0;
}



// Estimates delta_t for next step.
//
// tau < nu / sqrt(G*density)
// --> where G(Gravitational constant)
//        was replaced by Coulomb's constant.
//
void NBody::SetVariableDeltaT()
{
    // Density: /m^3
    auto Density = \
        this->Carriers.size()/ \
        (this->GetVolume()*1e-2*1e-2*1e-2);

    // Stability & accuracy constant. Default is 0.03
    //
#ifndef __MULTIPRECISION__
    fp_t new_delta_t = \
        nu / std::sqrt(k_e*Density);
#else
    fp_t new_delta_t = \
        nu / boost::math::sqrt(k_e*Density);
#endif
    this->delta_t = new_delta_t;
}


/**
 *
 * NBody
 *
 * Constructors and Destructors
 *
**/

//--> See header file.
