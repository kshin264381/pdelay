/**
 *
 * nbody_octree.cc
 *
 * Implementation for octal-tree N-Body algorithm...
 * adopted from Barnes-Hut
 * http://www.ifa.hawaii.edu/~barnes/treecode/treeguide.html
 *
 * Written by Taylor Shin
 * Sep. 20th, 2016
 *
**/

#include "fputils.h"
#include "nbody_octree.h"

/**
 *
 * Private methods...
 *
**/
//
// The octal tree generator from CarrierList
//
int NBody_Octree::MakeTree()
{
    // Stop simulation if nothing has been read out.
    if (this->Carriers.empty()) {
        std::cerr << "Cannot find any carriers!!" << std::endl;
        exit(0);
    }

    // Initialize Tree
    //this->Tree.reset();
    this->Tree = nullptr;
    this->Tree = std::make_shared<BHTree>(FirstOctant);

    // Looks stupid but std::map doesn't have random access
    // iterators. So, we can't rely on them.
    //
    // Initializing progress bar
    uint64_t total_carriers = this->Carriers.size();
    std::string TreeTitle = std::string("Tree@")+this->to_str(this->elapsed_time);
    ProgressBar TreeGen(TreeTitle, total_carriers);

    auto it = std::begin(this->Carriers);
    auto end = std::end(this->Carriers);

    for (it; it != end; ++it) {
        if (!this->InsertToTree(it->second))
            TreeGen.Update();
    }

    return 0;
}
//
// Allocate a Carrier into Octree
// --> Interface for MakeTree()
int NBody_Octree::InsertToTree(const spCarrier& carrier)
{
    return this->Tree->insert(carrier);
}

//
// Initialize First Octant
//
void NBody_Octree::InitFirstQctant()
{
    auto x_dim = this->silicon_dimension->XRng().y - this->silicon_dimension->XRng().x;
    auto y_dim = this->silicon_dimension->YRng().y - this->silicon_dimension->YRng().x;
    auto z_dim = this->silicon_dimension->ZRng().y - this->silicon_dimension->ZRng().x;

    //auto max_dim = fp_max<fp_t>(x_dim, y_dim, z_dim);
    auto dim = Dim{ x_dim, y_dim, z_dim };
    Loc center_pt = Loc{
        (this->silicon_dimension->XRng().y + this->silicon_dimension->XRng().x) / 2.0,
        (this->silicon_dimension->YRng().y + this->silicon_dimension->YRng().x) / 2.0,
        (this->silicon_dimension->ZRng().y + this->silicon_dimension->ZRng().x) / 2.0
    };

    this->FirstOctant = std::make_shared<Octant>(dim, center_pt);
}



// Initialize simulation
//
int NBody_Octree::SimInit()
{
    // Initialize monitor variables.
    this->sim_step = 0;
    this->collected_carriers = 0;
    this->SimOutput = DecorOutput(65);
    this->SimOutput.SetType(2);
    this->current_sim_time = std::clock();

    // Initialize first octant
    this->InitFirstQctant();

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

    // Preparing visual report tool.
    this->SetOutputMode(this->log_carrier_data_format);

    // Setting up timer... and marking the simulation start time.
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
        this->WriteCarriersInit();
    }

    return 0;
}



/**
 *
 * Kick and Drift methods + Select
 *
**/
// Sub method for Kick OpenMP implementation.
int NBody_Octree::kick_sub(const uint64_t& istart, const uint64_t& ipoints, const fp_t& delta_t)
{
    auto it = std::begin(this->Carriers);
    auto end = std::begin(this->Carriers);
    std::advance(it, istart);
    std::advance(end, istart + ipoints);

    for (it; it != end; ++it) {
        it->second->ResetVelnForce();
        this->TreeUpdateCForce(this->Tree, it->second);
        this->TreeUpdateDForce(it->second);
        it->second->UpdateVel(delta_t*this->len_scale_f);
    }

    return 0;
}

// Kick
int NBody_Octree::Kick(const fp_t& delta_t)
{
    // Make tree... hoping it generates without error...
    if (this->MakeTree())
        return -1;

    // Initialize Force calculation status bar.
    this->ForceCal = ProgressBar("Force Est.", this->Carriers.size());
    
#ifdef _OPENMP
    uint64_t ithread, nthreads, ipoints, istart, npoints;
    fp_t omp_dt = delta_t;
#pragma omp parallel private(ithread, nthreads, ipoints, istart, npoints)
    {
        npoints = this->Carriers.size();
        ithread = omp_get_thread_num();
        nthreads = omp_get_num_threads();
        ipoints = npoints / nthreads;
        istart = ithread * ipoints;
        if (ithread == nthreads - 1)
            ipoints = npoints - istart;
        this->kick_sub(istart, ipoints, omp_dt);
#pragma omp critical
        {
            this->ForceCal.Update(ipoints);
        }
    }

#else
    // Single thread method... (Recursive)
    auto carr_it = std::begin(this->Carriers);
    auto carr_it_end = std::end(this->Carriers);
    for (carr_it; carr_it != carr_it_end; carr_it++) {
        carr_it->second->ResetVelnForce();
        this->TreeUpdateCForce(this->Tree, carr_it->second);
        this->TreeUpdateDForce(carr_it->second);
        carr_it->second->UpdateVel(delta_t*this->len_scale_f);
        this->ForceCal.Update();
    }
#endif /* #ifdef _OPENMP */

    return 0;
}
int NBody_Octree::Kick()
{
    return this->Kick(this->delta_t);
}

// Drift
int NBody_Octree::Drift(const fp_t& delta_t)
{
    this->update_all_carr_position(delta_t);
    return 0;
}
int NBody_Octree::Drift()
{
    this->update_all_carr_position(this->delta_t);
    return 0;
}

// Select --> consider just using friend class' Select.
void NBody_Octree::Select()
{
    this->SetVariableDeltaT();
    this->forced_delta_t = false;
}
void NBody_Octree::Select(const fp_t& tau)
{
    this->delta_t = tau;
    this->forced_delta_t = true;
}


/**********************************************************/
// Update force in Tree
void NBody_Octree::TreeUpdateCForce(spOctree tree, spCarrier carrier)
{
    auto dist = tree->GetCarrier()->GetPos().dist(carrier->GetPos());
    auto oct_len = tree->GetOctant()->GetLength();
    auto oct_len_max = fp_max<fp_t>(oct_len.x, oct_len.y, oct_len.z);

    if ( tree->GetCarrier() != carrier && (dist / oct_len_max) < this->alpha ) {
        // Update Coulomb force withing alpha...
        carrier->AddForce(this->CoulombForce(carrier, tree->GetCarrier()));
    }

    // Recursively... run this crap to the end...
    if (tree->GetuNW()) { this->TreeUpdateCForce(tree->GetuNW(), carrier); }
    if (tree->GetuNE()) { this->TreeUpdateCForce(tree->GetuNE(), carrier); }
    if (tree->GetuSW()) { this->TreeUpdateCForce(tree->GetuSW(), carrier); }
    if (tree->GetuSE()) { this->TreeUpdateCForce(tree->GetuSE(), carrier); }
    if (tree->GetlNW()) { this->TreeUpdateCForce(tree->GetlNW(), carrier); }
    if (tree->GetlNE()) { this->TreeUpdateCForce(tree->GetlNE(), carrier); }
    if (tree->GetlSW()) { this->TreeUpdateCForce(tree->GetlSW(), carrier); }
    if (tree->GetlSE()) { this->TreeUpdateCForce(tree->GetlSE(), carrier); }
}

// Updating Drift force.
void NBody_Octree::TreeUpdateDForce(spCarrier carrier)
{
    carrier->AddForce(this->DriftForce(carrier));
}



/**********************************************************/



/**********************************************************/
// Update carrier position
//
void NBody_Octree::update_carr_position(spCarrier& carrier)
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

// Same with update_carr_position but with given time.
void NBody_Octree::update_carr_position(spCarrier& carrier, const fp_t& tau)
{
    // Implemented Debye length to solve the "too close carriers"
    // problem.

    // Back up previous position before update.
    Loc prev_pos = carrier->GetPos();
    carrier->UpdatePos(tau);
    // Update positions with mean free path estimation.
    this->MFPAdj(carrier, tau);
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
void NBody_Octree::update_all_carr_position_sub(
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
void NBody_Octree::update_all_carr_position(const fp_t& tau)
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





/**
 *
 * Public methods...
 *
**/

// Finishing methods...
void NBody_Octree::SimFinishMessage()
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

// Prints out simulation status
void NBody_Octree::ShowSimStatus()
{
    std::stringstream sst;

    // show current status
    this->current_sim_time = std::clock();
    sst.str(std::string());
    sst.clear(); // clearing out any eof or fail stuff..
    sst << "*** (Barnes-Hut) Step [" \
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
        << "Alpha: " \
        << this->alpha \
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




/**
 *
 * N-Body calculation algorithms
 *
**/

// Wrapper for single shot.
int NBody_Octree::Run()
{
    return RunSingleShot();
}

// Single shot algorithm
int NBody_Octree::RunSingleShot()
{
    // Announcing model
    std::cout << "Using Single Shot N-Body simulation model!!" << std::endl;

    // Initialize simulation
    this->SimInit();

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


// SKDK
// Select, Kick, Drift, Kick
int NBody_Octree::RunSKDK()
{
    // Select Kick Drift Kick mode.
    //
    //

    // Announcing model
    std::cout << "Using SKDK N-Body simulation model!!" << std::endl;

    // Initialize
    this->SimInit();

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


// SDKD
// Select, Drift, Kick, Drift
int NBody_Octree::RunSDKD()
{
    // Select Drift Kick Drift mode.
    //
    //

    // Announcing model
    std::cout << "Using SDKD N-Body simulation model!!" << std::endl;

    // Initialize
    this->SimInit();

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





// Some delta_t control
// Estimates delta_t for next step.
//
// tau < nu / sqrt(G*density)
// --> where G(Gravitational constant) 
//        was replaced by Coulomb's constant.
//
void NBody_Octree::SetVariableDeltaT()
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



/**
 *
 * Constructors and destructors
 *
**/

// --> See header file.