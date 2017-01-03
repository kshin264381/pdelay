/**
 *
 * sim_file_io.cc
 *
 * File IO part for N-Body simulation (implementation)
 *
 * Written by Taylor Shin
 * Sep. 7th, 2016
 *
**/


#include "sim_file_io.h"

// random number generator...
static fp_t rand_delta();

// Set logfile name
//
int NBodyFileIO::set_logfile_name()
{
    // current date/time
    std::time_t now = std::time(NULL);

    // setting up basename
    std::string base_filename;
    if (this->container_file)
        base_filename = this->container_file;
    else if (this->db_file)
        base_filename = this->db_file;

    boost::filesystem::path p(base_filename);
    std::string ext(".log");

    std::stringstream s_lf_name;
    std::stringstream s_time_string;

#ifndef _MSC_VER
    s_time_string << \
        std::put_time(std::localtime(&now),
            "%a_%b_%d_%T_%Y");
#else
    struct tm _tm;
    localtime_s(&_tm, &now);
    s_time_string << \
        std::put_time(&_tm, "%a_%b_%d_%T_%Y");
#endif

    this->time_string = s_time_string.str();

    // remove ':'s from time_string
    std::replace(
        this->time_string.begin(),
        this->time_string.end(), ':', '_');

    s_lf_name \
        << p.filename().string() \
        << "_" << this->time_string << ext;

    this->logfile_name = s_lf_name.str();

    return 0;
}

// Return carrier location info as string
//
std::string NBodyFileIO::carrier_info(spCarrier& carrier)
{
    std::string timestamp(this->print_elapsed_time());
    std::string carrier_ID(carrier->GetID());
    auto carrierPos = carrier->GetPos();

    std::stringstream last_known_location;
    last_known_location \
        << "(" \
        << carrierPos.x << ", " \
        << carrierPos.y << ", " \
        << carrierPos.z << ")";

    std::stringstream carrier_info_string;
    carrier_info_string \
        << "["+timestamp+"] " \
        << carrier_ID << " " \
        << last_known_location.str();

    return carrier_info_string.str();
}


// Write collected carrier info. to file.
int NBodyFileIO::write_collected_carrier_info(spCarrier& carrier)
{

#pragma omp critical
{
    this->simulation_logfile.open(
        this->logfile_name, std::ios::out | std::ios::app);

    if (this->simulation_logfile.is_open()) {
        this->simulation_logfile \
            << this->carrier_info(carrier) \
            << std::endl;
    }

    std::cout << "Collected carrier: " \
        << carrier->GetID() \
        << " at (" << carrier->GetPos().x \
        << ", " << carrier->GetPos().y \
        << ", " << carrier->GetPos().z \
        << ")" << std::endl;

    //this->collected_carriers++;

    this->simulation_logfile.close();

} /* #pragma omp critical */

    return 0;
}

// Write lost carrier info. to file.
int NBodyFileIO::write_lost_carrier_info(spCarrier& carrier)
{

#pragma omp critical
{

    this->simulation_logfile.open(
        this->logfile_name, std::ios::out | std::ios::app);

    if (this->simulation_logfile.is_open()) {
        this->simulation_logfile \
            << "** LOST ** " \
            << this->carrier_info(carrier) \
            << std::endl;
    }

    std::cout << "Detected out of reach carrier: " \
        << carrier->GetID() \
        << " at (" << carrier->GetPos().x << ", " \
        << carrier->GetPos().y << ", " \
        << carrier->GetPos().z << ")" << std::endl;

    //this->lost_carriers++;

    this->simulation_logfile.close();

} /* #pragma omp critical */

    return 0;
}

// Write recombinated carrier couple(?) info. to logfile.
int NBodyFileIO::write_recombination_carrier_info(
    spCarrier& carrier, spCarrier& other)
{
#pragma omp critical
{

    this->simulation_logfile.open(
        this->logfile_name, std::ios::out | std::ios::app);

    if (this->simulation_logfile.is_open()) {
        this->simulation_logfile \
            << "** RECOMBINATION ** " \
            << this->carrier_info(carrier)  \
            << std::endl \
            << "** RECOMBINATION ** " \
            << this->carrier_info(other)  \
            << std::endl;
    }

    std::cout << "Detected recombination between: " \
        << carrier->GetID() \
        << " and " \
        << other->GetID() \
        << std::endl;

    //this->lost_carriers += 2;

    this->simulation_logfile.close();

} /* #pragma omp critical */

    return 0;
}

// Write lost carrier info. to file.
int NBodyFileIO::write_mat_recomb_carrier_info(spCarrier& carrier)
{
#pragma omp critical
{

    this->simulation_logfile.open(
        this->logfile_name, std::ios::out | std::ios::app);

    if (this->simulation_logfile.is_open()) {
        this->simulation_logfile \
            << "** Recombination ** " \
            << this->carrier_info(carrier) \
            << std::endl;
    }

    std::cout << "Detected recombinated carrier: " \
        << carrier->GetID() \
        << " at (" << carrier->GetPos().x << ", " \
        << carrier->GetPos().y << ", " \
        << carrier->GetPos().z << ")" << std::endl;

    //this->lost_carriers++;

    this->simulation_logfile.close();

} /* #pragma omp critical */

    return 0;
}

// Returns base filename
std::string NBodyFileIO::GetBaseFilename()
{
    // setting up basename
    boost::filesystem::path p;
    if (this->container_file) {
        p = this->container_file;
    }
    else if (this->db_file) {
        p = this->db_file;
    }
    else {
        std::cerr << "Input filename is not valid!!" << std::endl;
        exit(-1);
    }

    std::stringstream s_lf_name;
    s_lf_name << p.filename().string();

    return s_lf_name.str();
}


// Reading out carriers from given file.
int NBodyFileIO::generate_carriers()
{
    if (this->InitialData) {
        uint64_t    entries = this->InitialData->Len();
        CarrierReadInProgress = \
            ProgressBar("Generating carriers", entries * 2);

        uint64_t  carr_index = 0;
        uint64_t  cnt_elec = 0;
        uint64_t  cnt_hole = 0;

        auto electron_mass = \
            this->DetMaterial.GetSemi("Silicon", "MVTHN")*m_elec;
        auto hole_mass = \
            this->DetMaterial.GetSemi("Silicon", "MVTHP")*m_elec;

#pragma omp parallel shared(electron_mass, hole_mass, carr_index, cnt_elec, cnt_hole)
        {
#pragma omp for
            for (auto i = 0; i<entries; ++i) {
                // Extract Charge
                auto particle_type = \
                    this->InitialData->Get<unsigned int>("flagParticle", i);
                if (particle_type == 1) {
                    // Extract location
                    auto carr_pos = \
                        Loc{
                        this->InitialData->Get<fp_t>("x", i),
                        this->InitialData->Get<fp_t>("y", i),
                        this->InitialData->Get<fp_t>("z", i)
                    } / this->NormFactor;

                    // Set up initial velocity
                    auto carr_vel = Vel{
                        static_cast<fp_t>(0.0),
                        static_cast<fp_t>(0.0),
                        static_cast<fp_t>(0.0)
                    };

#pragma omp critical
                    {
                        auto Electron = \
                            std::make_shared<Carrier>(
                                q_e, carr_pos, carr_vel,
                                electron_mass,
                                carr_index);
                        this->Carriers[carr_index] = Electron;
                        ++carr_index;
                        ++cnt_elec;
                        this->CarrierReadInProgress.Update(1);
                    }

                    // position for holes
                    carr_pos += Loc{
                        carr_pos.x*rand_delta(),
                        carr_pos.y*rand_delta(),
                        carr_pos.z*rand_delta()
                    };

#pragma omp critical
                    {
                        auto Hole = \
                            std::make_shared<Carrier>(
                                q_h, carr_pos, carr_vel,
                                hole_mass,
                                carr_index);
                        this->Carriers[carr_index] = Hole;
                        ++carr_index;
                        ++cnt_hole;
                        this->CarrierReadInProgress.Update(1);
                    }
                } /* if (particle_type == 1) */
                else {
#pragma omp critical
                    {
                        std::cout \
                            << "Neglecting unknown particle: " \
                            << "Flag " \
                            << particle_type << " is not known." \
                            << std::endl;
                        // Looks insane but should be 2 since carrier numbers has been doubled
                        // when initializing the progress bar class.
                        this->CarrierReadInProgress.Update(2);
                    }
                } /* if (particle_type == 1) else */
            } /* #pragma omp critical */

        } /* #pragma omp parallel shared(i, carr_index) */

#pragma omp barrier

        std::cout \
            << ">>> Carrier generation complete!!!" \
            << std::endl;
        std::cout \
            << "--> Collected " \
            << this->Carriers.size() \
            << " Carriers" << std::endl;

        this->InitialData = nullptr;

        this->num_elec = cnt_elec;
        this->num_hole = cnt_hole;
        return 0;
    } /* if (this->InitialData) */

    else {
        std::cerr \
            << ">>>> NBody: Uh crap, can't find any valid carriers!!" \
            << std::endl \
            << ">>>> Check your input file format!!" \
            << std::endl;
        return -1;
    } /* if (this->InitialData) else */

    return 0;

} /* int NBody::generate_carriers() */

  // Reads in carriers from Sqlite3 database.
  //
int NBodyFileIO::generate_carriers_db()
{
    std::cout \
        << "Reading Carriers from database: " \
        << std::endl \
        << this->db_file \
        << std::endl;

    // Read carriers with load_carr library.
    this->Carriers = this->GetCarriers(this->db_file);

    // Setup proper mass for each carriers and also count 
    // electron and holes.
    //
    std::cout \
        << "Updating carrier mass." \
        << std::endl;

    auto electron_mass = \
        this->DetMaterial.GetSemi("Silicon", "MVTHN")*m_elec;
    auto hole_mass = \
        this->DetMaterial.GetSemi("Silicon", "MVTHP")*m_elec;

    auto carr_it = std::begin(this->Carriers);
    for (carr_it;
        carr_it != std::end(this->Carriers); ++carr_it) {

        auto spTmpCarr = carr_it->second;
        auto carr_charge = spTmpCarr->GetCharge();

        if (fp_mt(carr_charge, static_cast<fp_t>(0.0))) {
            spTmpCarr->SetMass(hole_mass);
            this->num_hole++;
        }
        else {
            spTmpCarr->SetMass(electron_mass);
            this->num_elec++;
        }
    }

    // Setting up simulation time.
    this->elapsed_time = this->sim_time;

    return 0;
}







// misc stuff
static fp_t rand_delta()
{
    return uniform_rand<fp_t>(rand_delta_min, rand_delta_max);
}