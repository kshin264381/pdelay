/**
 * pdelay.cc
 *
 * Main function for the N-body simulation.
 *
 * Written by Taylor Shin
 * Feb. 20th, 2016
 *
**/

#include "pdelay.h"

/**
 *
 * The main function...
 *
**/
int main(int argc, char* argv[])
{
    std::unique_ptr<PDelay> SimulationControl = \
        std::make_unique<PDelay>(argc, argv);

#if defined(__posix) || defined(__linux) || defined(__unix)
    SimulationControl->set_stack_size(STACK_SIZE*1024*1024);
#endif

    return SimulationControl->Run();
}


/**
 *
 * PDelay class method implementations...
 *
**/

// Implementation of cpuNUM
int PDelay::cpuNUM()
{
#if defined(WIN32) || defined(_MSC_VER)
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;

#elif MACOS
    int nm[2];
    size_t len = 4;
    uint32_t count;

    nm[0] = CTL_HW; nm[1] = HW_AVAILCPU;
    sysctl(nm, 2, &count, &len, NULL, 0);

    if(count < 1) {
        nm[1] = HW_NCPU;
        sysctl(nm, 2, &count, &len, NULL, 0);
        if(count < 1) { count = 1; }
    }
    return count;
#else
    return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

#if defined(__posix) || defined(__linux) || defined(__unix)
// Set stack size
int PDelay::set_stack_size(const rlim_t& stack_size)
{
    struct rlimit rl;
    int rs;

    rs = getrlimit(RLIMIT_STACK, &rl);
    if (!rs) {
        if (rl.rlim_cur < stack_size) {
            rl.rlim_cur = stack_size;
            rs = setrlimit(RLIMIT_STACK, &rl);
            if (rs)
            {
                std::cerr << "setrlimit returned: %d" << rs << std::endl;
            }
        }
    }

    return 0;
}
#endif
/* #if defined(__posix) || defined(__linux) || defined(__unix) */


// Implementation of usage()
void PDelay::usage(const char* exec_name)
{
    std::string tarball_note = \
        "Notes\t: Tarball needs to be gzip or bzip2 compressed file!!";
    std::string options_description = \
        "-p <n>    : Defines number (<n>) of cores. \n";
    options_description += \
        "            i.e. -p 4 runs the simulation with 4 cores.\n";
    options_description += \
        "            If this variable is not supplied, the simulator \n";
    options_description += \
        "            detects number of cores automatically.\n";
    options_description += \
        "-i <file> : Input MonteCarlo data file. \n";
    options_description += \
        "            In fact, you don't need this option but\n";
    options_description += \
        "            just put in filename... \n";
    options_description += \
        "-d <file> : Input (SQLite3) material database.\n";
    options_description += \
        "            Not so needed since the program knows the defaults for silicon, anyway.\n";
    options_description += \
        "-c <file> : Carrier information database file.\n";
    options_description += \
        "            Continues calculation from latest state.\n";
    options_description += \
        "-e <delta_t_in_sec> : Forces simulation timestep.\n";
    options_description += \
        "            If not given, the simulator actively adjusts \n            delta_t based on carrier density.\n";
    options_description += \
        "-a <algorithm_name> : Determines calculation algorithm, OneShot, SKDK, SDKD.\n";
    options_description += \
        "            If not given, assumes single shot mode automatically.\n";
    options_description += \
        "-m <simulation_model> : Determines simulation model (OneToOne or Octree)\n";
    options_description += \
        "            If not given, it assumes One-To-One model.\n";
    options_description += \
        "-o <doping_concentration> : forces doping concentration.\n";
    options_description += \
        "            If not given, assumes 3.59e+11.\n";


    std::cerr << std::endl;
    std::cerr << "Usage: " << exec_name << \
        " <Tarball compressed datafile> <options>" << std::endl;
    std::cerr << std::endl;
    std::cerr << "<Tarball compressed datafile>" << std::endl \
        << tarball_note << std::endl;
    std::cerr << std::endl;
    std::cerr << "<options>" << std::endl << \
        options_description << std:: endl;
    std::cerr << std::endl;
}


// Get path(dir) from given file std::string
std::string PDelay::GetPath(std::string some_file_path)
{
// #if !defined(__CYGWIN__)
    // Setting up slash
    auto full_path_str = this->GetFullPath(some_file_path);
    auto full_path = fs::wpath(full_path_str);

    // Check if input file string has directory path.
    std::string path_dir = \
        full_path.make_preferred().remove_filename().string();
    if (path_dir.empty()) path_dir = ".";

    if (!fs::exists(fs::status(fs::wpath(path_dir)))) {
        std::cerr << "Directory not found: " \
            << path_dir << std::endl;
        exit(-1);
    }

// #else



// #endif

    return path_dir;
}
// Same one as above but accepts const char* 
std::string PDelay::GetPath(const char* some_file_path)
{
    return GetPath(std::string(some_file_path));
}

// Get full file path
std::string PDelay::GetFullPath(std::string some_file_path)
{
// #if !defined(__CYGWIN__)
    // Setting up slash
    fs::wpath slash("/");
    fs::wpath preferred_slash = slash.make_preferred().native();
    some_file_path = replace(
        some_file_path, slash.string(), preferred_slash.string());

    // Preparing full path variable.
    fs::wpath full_path(fs::initial_path<fs::path>());
    full_path = fs::complete(fs::wpath(some_file_path));
    auto full_path_str = full_path.make_preferred().string();

    if (!fs::exists(full_path)) {
        std::cerr << "File not found: " \
            << full_path_str << std::endl;
        exit(-1);
    }

// #else


// #endif


    return full_path_str;
}
std::string PDelay::GetFullPath(const char* some_file_path)
{
    return GetFullPath(std::string(some_file_path));
}

// Run the actual simulation
int PDelay::Run()
{
    ReportStatus();

    if (this->sim_mode_i == onetoone)
        return RunOneToOne();
    else if (this->sim_mode_i == octree)
        return RunOctree();
    else
        return RunOneToOne();
}

// Runs one to one simulation
int PDelay::RunOneToOne()
{
    // Initialize Simulation runners
    if (!continued)
        InitOneToOneS();
    else
        InitOneToOneC();

    // Set up default unit for simulation spaces.
    this->NBodyRunner->SetInputUnit(def_unit);

    // Setting up Delta T if we want to fix it to a certain value.
    if (force_delta_t)
        NBodyRunner->SetDeltaT(delta_t);

    // Setting up visualization data (carrier log data) format.
    this->NBodyRunner->SetCarrierDataFormat(vis_mode);

    // Now run the simulation
    if (this->sim_algorithm_i == sdkd)
        return NBodyRunner->RunSDKD();
    else if (this->sim_algorithm_i == skdk)
        return NBodyRunner->RunSKDK();
    else
        return NBodyRunner->Run();
}

// Runs one to one simulation from the beginning
int PDelay::InitOneToOneS()
{
    this->NBodyRunner = std::make_unique<NBody>(
        input_file.c_str(),
        SensorChunk,
        DetBias,
        doping_concentration,
        temperature,
        num_of_procs,
        database_file.c_str());

    this->NBodyRunner->SetCarrLogGen(this->c_log);

    this->NBodyOctreeRunner = nullptr;
    
    return 0;
}


// Runs one to one simulation from carrier location log database
int PDelay::InitOneToOneC()
{
    this->NBodyRunner = std::make_unique<NBody>(
        input_file.c_str(),
        SensorChunk,
        DetBias,
        doping_concentration,
        temperature,
        num_of_procs,
        database_file.c_str(),
        true);

    this->NBodyRunner->SetCarrLogGen(this->c_log);

    this->NBodyOctreeRunner = nullptr;

    return 0;
}

// Runs Octree simulation.
int PDelay::RunOctree()
{
    // Initialize Simulation runners
    if (!continued)
        InitOctreeS();
    else
        InitOctreeC();

    // Set up default unit for simulation spaces.
    this->NBodyOctreeRunner->SetInputUnit(def_unit);

    // Setting up Delta T if we want to fix it to a certain value.
    if (force_delta_t)
        this->NBodyOctreeRunner->SetDeltaT(delta_t);

    // Setting up visualization data (carrier log data) format.
    this->NBodyOctreeRunner->SetCarrierDataFormat(vis_mode);

    // Now run the simulation
    if (this->sim_algorithm_i == sdkd)
        return this->NBodyOctreeRunner->RunSDKD();
    else if (this->sim_algorithm_i == skdk)
        return this->NBodyOctreeRunner->RunSKDK();
    else
        return this->NBodyOctreeRunner->Run();

    return 0;
}

// Runs Octree simulation from the beginning
int PDelay::InitOctreeS()
{
    this->NBodyOctreeRunner = \
        std::make_unique<NBody_Octree>(
            input_file.c_str(),
            SensorChunk,
            DetBias,
            doping_concentration,
            temperature,
            num_of_procs,
            database_file.c_str());
    this->NBodyOctreeRunner->SetCarrLogGen(this->c_log);

    this->NBodyRunner = nullptr;

    return 0;
}


// Runs Octree simulation from carrier location log database
int PDelay::InitOctreeC()
{
    this->NBodyOctreeRunner = \
        std::make_unique<NBody_Octree>(
            input_file.c_str(),
            SensorChunk,
            DetBias,
            doping_concentration,
            temperature,
            num_of_procs,
            database_file.c_str(),
            true);
    this->NBodyOctreeRunner->SetCarrLogGen(this->c_log);

    this->NBodyRunner = nullptr;

    return 0;
}


// parse options and establish inputs.
int PDelay::ParseOptions(int argc, char* argv[])
{
    options.add_options()
        ("p,procs", "Number of processes", cxxopts::value<unsigned int>(num_of_procs))
        ("i,input", "Input MonteCarlo Simulation File", cxxopts::value<std::string>(input_file))
        ("h,help", "Shows help message")
        ("d,database", "Material Database File", cxxopts::value<std::string>(database_file))
        ("c,continue", "Carrier information database file", cxxopts::value<std::string>(cr_file))
        ("t,temperature", "Temperature", cxxopts::value<fp_t>(temperature))
        ("e,delta_t", "Time step", cxxopts::value<fp_t>(delta_t))
        ("a,algorithm", "N-Body calculation model", cxxopts::value<std::string>(algorithm))
        ("m,model", "N-Body simulation mode", cxxopts::value<std::string>(sim_mode))
        ("o,doping_concentration", "Doping Concentration of the sensor", cxxopts::value<fp_t>(doping_concentration))
        ("l,carrier_log", "Generate carrier log (default True)", cxxopts::value<std::string>(c_log_str));

    options.parse_positional({ "input", "procs", "positional" });
    options.parse(argc, argv);

    // Setting up self path
    SetSelfPath(argv[0]);

    // if cr_file is given, set to continuation mode.
    if (!cr_file.empty()) {
        input_file = cr_file;
        continued = true;
    }

    // Convert input file path to proper native path
    input_file = GetFullPath(input_file);

    // Prepare database file path
    SetMatDBFilePath();
    std::cout << input_file << std::endl;
        // if no input files are given, ditch the session.
    if (input_file.empty() || input_file == "")
    {
        usage(argv[0]);
        exit(0);
    }

    // If -l option was not given (delta_t)
    if (fp_equal(delta_t, FP_T(0.0))) {
    }
    else {
        std::cout << "Setting delta_t as: " \
            << delta_t << " seconds." << std::endl;
    }

    // Set up simulation mode
    this->SetSimMode(sim_mode);
    
    // Set up simulation calculation mode
    this->SetAlgorithm(algorithm);

    // Set up c_log
    if (str_to_lower(c_log_str) == "false")
        this->c_log = false;

    return 0;
}


/**
 *
 * Set and Get methods... Implementation part
 *
**/
void PDelay::SetAlgorithm(const std::string& new_calc_algorithm)
{
    if (new_calc_algorithm == "OneShot" ||
        new_calc_algorithm == "SKDK" ||
        new_calc_algorithm == "SDKD") {
        this->algorithm = new_calc_algorithm;
    }
    else {
        std::cout << "Error!! Wrong algorithm!!" << std::endl;
        std::cout << "Use one of: OneShot, SKDK, SDKD" << std::endl;
        exit(-1);
    }
}
void PDelay::SetContinued(bool i_continued)
{
    continued = i_continued;
}
void PDelay::SetProcesses(const unsigned int& new_procs)
{
    num_of_procs = new_procs;
}
void PDelay::SetDimension(const Box& new_dimension)
{
    SensorChunk = new_dimension;

    std::cout << "New Dimension... ";
    this->ReportDimension();

}
void PDelay::SetDimension(
    const fp_t& x_start, const fp_t& x_end,
    const fp_t& y_start, const fp_t& y_end,
    const fp_t& z_start, const fp_t& z_end) {
    SetDimension(
        Box{ x_start, x_end, y_start, y_end, z_start, z_end }
    );
}
void PDelay::SetBias(const Bias& new_bias)
{
    this->DetBias = new_bias;

    std::cout << "New Bias... ";
    this->ReportBias();
}
void PDelay::SetBias(const fp_t& top_bias, const fp_t& bottom_bias)
{
    this->SetBias(Bias{ top_bias, bottom_bias });
}
void PDelay::SetDetMaterial(const std::string& new_det_material)
{
    this->DetMaterial = new_det_material;
}
void PDelay::SetDetMaterial(const char* new_det_material)
{
    this->SetDetMaterial(std::string(new_det_material));
}
void PDelay::SetInsulatorMaterial(const std::string& new_insulator_material)
{
    this->InsulatorMaterial = new_insulator_material;
}
void PDelay::SetInsulatorMaterial(const char* new_insulator_material)
{
    this->SetInsulatorMaterial(std::string(new_insulator_material));
}
void PDelay::SetTemperature(const fp_t& new_temp)
{
    this->temperature = new_temp;
}
void PDelay::SetDeltaT(const fp_t& new_delta_t)
{
    this->delta_t = new_delta_t;
    if (fp_equal(delta_t, FP_T(0.0))) {
        std::cout << \
            "Setting delta_t as automatic adjustment." \
            << std::endl;
        this->force_delta_t = false;
    }
    else {
        std::cout << \
            "Setting delta_t as: " \
            << delta_t << " seconds." << std::endl;
        this->force_delta_t = true;
    }

    ReportDeltaT();
}
bool PDelay::SetSelfPath(const char* argv_zero)
{
    this->self_path = GetPath(argv_zero);
    return true;
}
bool PDelay::SetMatDBFile(const std::string& new_DB_file)
{
    this->database_file = GetPath(new_DB_file);
    this->SetMatDBFilePath();
    return true;
}
bool PDelay::SetMatDBFile(const char* new_DB_file)
{
    return this->SetMatDBFile(std::string(new_DB_file));
}
void PDelay::SetMatDBFilePath()
{
    this->database_file = this->GetFullPath(this->database_file);
}
bool PDelay::SetInputFile(const std::string& new_input_file)
{
    this->input_file = GetPath(new_input_file);
    return true;
}
bool PDelay::SetInputFile(const char* new_input_file)
{
    return SetInputFile(std::string(new_input_file));
}
int PDelay::SetSimMode(std::string mode)
{
    sim_modes sim_mode_i = onetoone;
    if (str_to_lower(mode) == "onetoone") {
        this->sim_mode = "OneToOne";
        this->sim_mode_i = onetoone;
    }
    else if (str_to_lower(mode) == "octree") {
        this->sim_mode = "Octree";
        this->sim_mode_i = octree;
    }
    else {
        this->sim_mode = "OneToOne";
        this->sim_mode_i = onetoone;
    }

    std::string sim_mode_print = "";
    int ret_code = 0;
    switch (sim_mode_i) {
    case onetoone:
        sim_mode_print = "One-To-One";
    case octree:
        sim_mode_print = "Octal Tree";
    }
    ret_code = sim_mode_i;

    std::cout << \
        "Setting up simulation mode: " \
        << sim_mode_print << std::endl;

    return ret_code;
}
int PDelay::SetSimMode(const char* new_sim_mode)
{
    return SetSimMode(std::string(new_sim_mode));
}

/**
 *
 * Report status on current session
 *
**/
void PDelay::ReportStatus()
{
    std::cout << std::endl;
    std::cout << "========= Basic Simulation Info. =========" << std::endl;
    ReportSimMode();
    ReportDeltaT();
    ReportAlgorithm();
    std::cout << "==========================================" << std::endl;

    ReportDimension();
    ReportBias();
    std::cout << std::endl;
}

void PDelay::ReportBias()
{
    // Shows bias info.
    std::cout << "=============== Bias Info. ===============" << std::endl;
    std::cout << "Anode bias: " << DetBias.x << " V" << std::endl;
    std::cout << "Cathode bias: " << DetBias.y << " V" << std::endl;
    std::cout << "Thus, the bias between Anode and Cathode is..." << std::endl;
    std::cout << "Bias across detector is: " << \
        DetBias.y - DetBias.x << " V" << std::endl;
    std::cout << "==========================================" << std::endl;
}

void PDelay::ReportDimension()
{
    // Shows dimension info.
    std::cout << "========= Sensor Dimension Info. =========" << std::endl;
    //std::cout << "x.start: " << SensorChunk.x_start << " um" << std::endl;
    //std::cout << "x.end: " << SensorChunk.x_end << " um" << std::endl;
    //std::cout << "y.start: " << SensorChunk.y_start << " um" << std::endl;
    //std::cout << "y.end: " << SensorChunk.y_end << " um" << std::endl;
    //std::cout << "z.start: " << SensorChunk.z_start << " um" << std::endl;
    //std::cout << "z.end: " << SensorChunk.z_end << " um" << std::endl;
    //std::cout << std::endl;
    //std::cout << "So, the sensor dimension is..." << std::endl;
    std::cout << "x length: " << SensorChunk.x_end - SensorChunk.x_start << " um" << std::endl;
    std::cout << "y length: " << SensorChunk.y_end - SensorChunk.y_start << " um" << std::endl;
    std::cout << "z length: " << SensorChunk.z_end - SensorChunk.z_start << " um" << std::endl;
    std::cout << "Photon source: (x, y, z): (0, 0, 0)" << std::endl;
    std::cout << "==========================================" << std::endl;
}

void PDelay::ReportDeltaT()
{
    // Shows delta_t setting
    if (!fp_equal(delta_t, static_cast<fp_t>(0.0))) {
        std::cout << \
            "Delta_T is fixed to: " \
            << delta_t << " seconds." << std::endl;
    }
    else {
        std::cout << \
            "Delta_T will be defined automatically by simulator." << \
            std::endl;
    }
}

void PDelay::ReportSimMode()
{
    std::string sim_mode_print = "";

    switch (sim_mode_i) {
    case onetoone:
        sim_mode_print = "One-To-One";
        break;
    case octree:
        sim_mode_print = "Octal Tree";
        break;
    }

    std::cout << "Simulation Mode: " \
        << sim_mode_print \
        << std::endl;
}

void PDelay::ReportAlgorithm()
{
    std::string sim_algorithm_print = "";

    switch (sim_algorithm_i) {
    case oneshot:
        sim_algorithm_print = "One Shot";
        break;
    case skdk:
        sim_algorithm_print = "Select-Kick-Drift-Kick";
        break;
    case sdkd:
        sim_algorithm_print = "Select-Drift-Kick-Drift";
        break;
    }

    std::cout << "Calculation Algorithm: " \
        << sim_algorithm_print \
        << std::endl;
}