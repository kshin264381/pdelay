/**
 *
 * visual.cc
 *
 * Visual library for nbody algorithm
 * (Implementation)
 *
 * Written by Taylor Shin
 * May. 27th, 2016
 *
**/

#include <sqlite3.h>

/**
 *
 * Uncomment the line below to enable multiprocessing (openmp) 
 * for sqlite3.
 *
**/
#define __MULTIPROCESSING_SQLITE3__

#ifdef __MULTIPROCESSING_SQLITE3__
#include <omp.h>
#include "sqlite_mt.h"
#endif

#include "visual.h"
#include "nbody.h"

/**
 *
 * Private stuff
 *
**/

// csv output
int NBodyVisual::write_carrier_csv()
{
    if( this->write_carr<fp_t, fp_int_t, fp_t>
            (this->elapsed_time, this->carr_index, this->carr_type,
            this->x_coord, this->y_coord, this->z_coord,
            this->x_vel, this->y_vel, this->z_vel,
            this->x_for, this->y_for, this->z_for) )
        return -1;
    return 0;
}

// log output (default)
int NBodyVisual::write_carrier_log()
{
    if( this->write_carr<fp_t, fp_int_t, fp_t>
            (this->elapsed_time, this->carr_index, this->carr_type,
            this->x_coord, this->y_coord, this->z_coord,
            this->x_vel, this->y_vel, this->z_vel,
            this->x_for, this->y_for, this->z_for) )
        return -1;
    return 0;
}

// sqlite3 database output --> writes data into sql table.
//
int NBodyVisual::write_carrier_sqlite3()
{
    sqlite3*   carriers_db;
    char*      zErrMsg      = 0;
    int        rc;

    // Check if current timestamp exists
    if (!this->check_table(this->timestamp_str.c_str())) {
#pragma omp critical
        {
        this->make_table();
        }
    }

    // SQL Statements
    std::string  input_data("");

#if defined(__MULTIPROCESSING_SQLITE3__)
    rc = sqlite3_open_mt(this->output_filename.c_str(), &carriers_db);
#else
    rc = sqlite3_open(this->output_filename.c_str(), &carriers_db);
#endif

    if ( rc != SQLITE_OK ) {
        std::cerr \
            << "write SQL Error open: " \
            << sqlite3_errmsg(carriers_db) << std::endl;
        return -1;
    }

    // Prepare data input
    auto c_ind = this->to_str(this->carr_index);
    auto carr_mass = this->to_str(this->carr_mass);
    auto xc    = this->to_str(this->x_coord);
    auto yc    = this->to_str(this->y_coord);
    auto zc    = this->to_str(this->z_coord);
    auto vxc   = this->to_str(this->x_vel);
    auto vyc   = this->to_str(this->y_vel);
    auto vzc   = this->to_str(this->z_vel);
    auto fxc   = this->to_str(this->x_for);
    auto fyc   = this->to_str(this->y_for);
    auto fzc   = this->to_str(this->z_for);

    input_data = \
        std::string("INSERT INTO ")+\
        " \""+this->timestamp_str+"\" "+\
        " (CARR_INDEX, TYPE, MASS, X, Y, Z, VX, VY, VZ, FX, FY, FZ) " +\
        "VALUES ("+c_ind+", "+"'"+this->carr_type+"', "+\
        carr_mass+", "+\
        xc+", "+yc+", "+zc+", "+vxc+", "+vyc+", "+vzc+", "+\
        fxc+", "+fyc+", "+fzc+" )";

    // Performing Query
    sqlite3_stmt*      p_stmt = nullptr;
    const char**       pz_tail = nullptr;

#if defined(__MULTIPROCESSING_SQLITE3__)
    rc = sqlite3_prepare_v2_mt(
        carriers_db, input_data.c_str(), -1, &p_stmt, NULL);
#else
    rc = sqlite3_prepare_v2(
        carriers_db, input_data.c_str(), -1, &p_stmt, NULL);
#endif
    if ( rc != SQLITE_OK ) {
        std::cerr << "write SQL Error prepare: " \
            << sqlite3_errmsg(carriers_db) << std::endl;
        std::cerr << input_data << std::endl;
        return -1;
    }

    if ( p_stmt ) {
#if defined(__MULTIPROCESSING_SQLITE3__)
        rc = sqlite3_step_mt(p_stmt);
        if (rc != SQLITE_ROW) sqlite3_reset(p_stmt);
        sqlite3_finalize_mt(p_stmt);
        //sqlite3_exec_mt(carriers_db, "COMMIT", NULL, NULL, NULL);
#else
        rc = sqlite3_step(p_stmt);
        if (rc != SQLITE_ROW) sqlite3_reset(p_stmt);
        sqlite3_finalize(p_stmt);
        //sqlite3_exec(carriers_db, "COMMIT", NULL, NULL, NULL);
#endif
    }
    else {
        std::cerr << "Statment is NULL" << std::endl;
        return -1;
    }

    if ( rc != SQLITE_OK ) {
        std::cerr << "SQL Error exec: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        return -1;
    }

    if ( sqlite3_close(carriers_db) != SQLITE_OK ) {
        std::cerr << "write SQL close error" \
            << sqlite3_errmsg(carriers_db) \
            << std::endl;
        return -1;
    }

    return 0;
}

// sqlite3 database output for OpenMP
//
int NBodyVisual::write_carrier_sqlite3_omp(
    const std::string& timestamp_str,
    const uint64_t& carr_index,
    const fp_t& carr_mass,
    const fp_t& x_coord,
    const fp_t& y_coord,
    const fp_t& z_coord,
    const fp_t& x_vel,
    const fp_t& y_vel,
    const fp_t& z_vel,
    const fp_t& x_for,
    const fp_t& y_for,
    const fp_t& z_for,
    const std::string& carr_type)
{
    sqlite3*   carriers_db;
    char*      zErrMsg      = 0;
    int        rc;

    // SQL Statements
    std::string  input_data("");

#pragma omp critical
    {
#if defined(__MULTIPROCESSING_SQLITE3__)
        rc = sqlite3_open_mt(this->output_filename.c_str(), &carriers_db);
#else
        rc = sqlite3_open(this->output_filename.c_str(), &carriers_db);
#endif
    }
    if (rc != SQLITE_OK) {
        std::cerr << "write SQL Error open[" << rc << "]: " \
            << sqlite3_errmsg(carriers_db) << std::endl;
        return -1;
    }


    // Prepare data input
    auto c_ind = this->to_str(carr_index);
    auto c_mass = this->to_str(carr_mass);
    auto xc    = this->to_str(x_coord);
    auto yc    = this->to_str(y_coord);
    auto zc    = this->to_str(z_coord);
    auto vxc   = this->to_str(x_vel);
    auto vyc   = this->to_str(y_vel);
    auto vzc   = this->to_str(z_vel);
    auto fxc   = this->to_str(x_for);
    auto fyc   = this->to_str(y_for);
    auto fzc   = this->to_str(z_for);

    input_data = \
        std::string("INSERT INTO ")+\
        " \""+timestamp_str+"\" "+\
        " (CARR_INDEX, TYPE, MASS, X, Y, Z, VX, VY, VZ, FX, FY, FZ) " +\
        "VALUES ("+c_ind+", "+"'"+carr_type+"', "+\
        c_mass+", "+\
        xc+", "+yc+", "+zc+", "+vxc+", "+vyc+", "+vzc+", "+\
        fxc+", "+fyc+", "+fzc+" )";

    // Performing Query
    sqlite3_stmt*      p_stmt = nullptr;
    const char**       pz_tail = nullptr;

#if defined(__MULTIPROCESSING_SQLITE3__)
    rc = sqlite3_prepare_v2_mt(
        carriers_db, input_data.c_str(), -1, &p_stmt, pz_tail);
#else
    rc = sqlite3_prepare_v2(
        carriers_db, input_data.c_str(), -1, &p_stmt, pz_tail);
#endif
    if ( rc != SQLITE_OK ) {
        std::cerr \
            << "write SQL Error prepare[" << rc << "]: " \
            << sqlite3_errmsg(carriers_db) \
            << std::endl;
        std::cerr << input_data << std::endl;
        return -1;
    }

    if ( p_stmt ) {
#if defined(__MULTIPROCESSING_SQLITE3__)
        rc = sqlite3_step_mt(p_stmt);
        if (rc != SQLITE_ROW) sqlite3_reset(p_stmt);
        sqlite3_finalize_mt(p_stmt);
        //sqlite3_exec_mt(carriers_db, "COMMIT", NULL, NULL, NULL);
#else
        rc = sqlite3_step(p_stmt);
        if (rc != SQLITE_ROW) sqlite3_reset(p_stmt);
        sqlite3_finalize(p_stmt);
        //sqlite3_exec(carriers_db, "COMMIT", NULL, NULL, NULL);
#endif
    }
    else {
        std::cerr << "Statment is NULL" << std::endl;
        return -1;
    }

    if ( sqlite3_close(carriers_db) != SQLITE_OK ) {
        std::cerr << "write_omp SQL close error" \
            << sqlite3_errmsg(carriers_db) \
            << std::endl;
        return -1;
    }

    return 0;
}





// Initializes db
int NBodyVisual::init_db()
{
    sqlite3*     carriers_db;
    char*        zErrMsg = 0;
    int          rc;

#if defined(_MSC_VER)
#if defined(__MULTIPROCESSING_SQLITE3__)
    rc = sqlite3_open_v2_mt(this->output_filename.c_str(), &carriers_db,
        SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, NULL);
#else
    rc = sqlite3_open_v2(this->output_filename.c_str(), &carriers_db,
        SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, NULL);
#endif
#else /* if defined (_MSC_VER) */
#if defined(__MULTIPROCESSING_SQLITE3__)
    rc = sqlite3_open_mt(this->output_filename.c_str(), &carriers_db);
    //rc = sqlite3_open_v2_mt(this->output_filename.c_str(), &carriers_db, SQLITE_OPEN_NOMUTEX, "");
#else
    rc = sqlite3_open(this->output_filename.c_str(), &carriers_db);
#endif
#endif /* if defined (_MSC_VER) */

    if (rc != SQLITE_OK) {
        std::cerr << "init_db SQL Error open: " \
            << sqlite3_errmsg(carriers_db) << std::endl;
        return -1;
    }

    if (sqlite3_close(carriers_db) != SQLITE_OK) {
        std::cerr << "init_db SQL close error" \
            << sqlite3_errmsg(carriers_db) \
            << std::endl;
        return -1;
    }

    return this->make_table();
}







// Makes sqlite3 table for current timestamp
int NBodyVisual::make_table()
{
    return this->make_table(this->timestamp_str.c_str());
}

// Makes sqlite3 table for given table name
int NBodyVisual::make_table(std::string table_name)
{
    sqlite3*   carriers_db;
    char*      zErrMsg = 0;
    int        rc;

    // SQL Statements
    std::string    create_table;
    sqlite3_stmt*  p_stmt;
    //const char**   pz_tail;

#if defined(__MULTIPROCESSING_SQLITE3__)
    rc = sqlite3_open_mt(this->output_filename.c_str(), &carriers_db);
#else
    rc = sqlite3_open(this->output_filename.c_str(), &carriers_db);
#endif
    if (rc != SQLITE_OK) {
        std::cerr << "make_table SQL Error open: " \
            << sqlite3_errmsg(carriers_db) << std::endl;
        return -1;
    }

    // Ok, create table for current timestamp
    //this->timestamp_str = this->to_str<fp_t>(this->timestamp);
    create_table = \
        "CREATE TABLE \"" + table_name + "\"" + \
        " ( " + \
        "CARR_INDEX INT PRIMARY KEY   NOT NULL," + \
        "TYPE       TEXT              NOT NULL," + \
        "MASS       REAL              NOT NULL," + \
        "X          REAL              NOT NULL," + \
        "Y          REAL              NOT NULL," + \
        "Z          REAL              NOT NULL," + \
        "VX         REAL              NOT NULL," + \
        "VY         REAL              NOT NULL," + \
        "VZ         REAL              NOT NULL," + \
        "FX         REAL              NOT NULL," + \
        "FY         REAL              NOT NULL," + \
        "FZ         REAL              NOT NULL " + \
        " )";

    // Insert new table for current timestamp
    //rc = sqlite3_exec(carriers_db, create_table.c_str(), __callback__, 0, &zErrMsg);

#if defined(__MULTIPROCESSING_SQLITE3__)
    rc = sqlite3_prepare_v2_mt(
        carriers_db, create_table.c_str(), -1, &p_stmt, NULL);
#else
    rc = sqlite3_prepare_v2(
        carriers_db, create_table.c_str(), -1, &p_stmt, NULL);
#endif
    if (rc != SQLITE_OK) {
        std::cerr << "make_table SQL Error prepare: " \
            << sqlite3_errcode(carriers_db) << std::endl << std::endl;
        sqlite3_close(carriers_db);
        return -1;
    }

    if (p_stmt) {
#if defined(__MULTIPROCESSING_SQLITE3__)
        rc = sqlite3_step_mt(p_stmt);
        if (rc != SQLITE_ROW) sqlite3_reset(p_stmt);
        sqlite3_finalize_mt(p_stmt);
        //sqlite3_exec_mt(carriers_db, "COMMIT", NULL, NULL, NULL);
#else
        rc = sqlite3_step(p_stmt);
        if (rc != SQLITE_ROW) sqlite3_reset(p_stmt);
        sqlite3_finalize(p_stmt);
        //sqlite3_exec(carriers_db, "COMMIT", NULL, NULL, NULL);
#endif
    }

    if (sqlite3_close(carriers_db) != SQLITE_OK) {
        std::cerr << "make_table SQL close error: " \
            << sqlite3_errmsg(carriers_db) \
            << std::endl;
        return -1;
    }

    return 0;
}

// Tests table existence in current db
int NBodyVisual::check_table(const char* tab_name)
{
    sqlite3*   carriers_db;
    char*      zErrMsg      = 0;
    int        rc;
    int        query_result;

    std::stringstream ss;
    sqlite3_stmt* p_stmt;

#if defined(__MULTIPROCESSING_SQLITE3__)
    rc = sqlite3_open_v2_mt(
        this->output_filename.c_str(), &carriers_db,
        SQLITE_OPEN_READONLY, NULL);
#else
    rc = sqlite3_open_v2(
        this->output_filename.c_str(), &carriers_db,
        SQLITE_OPEN_READONLY, NULL);
#endif
    if ( rc != SQLITE_OK ) {
        std::cerr << "check_talbe SQL Error open: " \
            << sqlite3_errmsg(carriers_db) << std::endl;
        sqlite3_close(carriers_db);
        return -1;
    }

    ss  << "SELECT name FROM sqlite_master WHERE type='table' AND " \
        << "name='" << tab_name << "' ";
    std::string check_query = ss.str();

#if defined(__MULTIPROCESSING_SQLITE3__)
    rc = sqlite3_prepare_v2_mt(
        carriers_db, check_query.c_str(), -1, &p_stmt, NULL);
#else
    rc = sqlite3_prepare_v2(
        carriers_db, check_query.c_str(), -1, &p_stmt, NULL);
#endif
    if ( rc != SQLITE_OK ) {
        std::cerr << \
            "check_table SQL Error prepare: " << \
            sqlite3_errmsg(carriers_db) << std::endl;
        return -1;
    }

    if ( p_stmt ) {
#if defined(__MULTIPROCESSING_SQLITE3__)
        query_result = sqlite3_step_mt(p_stmt);
        if (query_result != SQLITE_ROW) sqlite3_reset(p_stmt);
        sqlite3_finalize_mt(p_stmt);
        //sqlite3_exec_mt(carriers_db, "COMMIT", NULL, NULL, NULL);
#else
        query_result = sqlite3_step(p_stmt);
        if (query_result != SQLITE_ROW) sqlite3_reset(p_stmt);
        sqlite3_finalize(p_stmt);
        //sqlite3_exec(carriers_db, "COMMIT", NULL, NULL, NULL);
#endif
    }

    if ( sqlite3_close(carriers_db) != SQLITE_OK ) {
        std::cerr << "check_table SQL close error" \
            << sqlite3_errmsg(carriers_db) \
            << std::endl;
        return -1;
    }

    if (query_result == SQLITE_ROW) {
        return SQLITE_CHECK_EXISTS;
    }
    else {
        return SQLITE_CHECK_INEXISTS;
    }
}



// Write a line
int NBodyVisual::write_line(std::string in_text)
{
    this->output_file.open(
        this->output_filename, ios::out | ios::app);

    if (this->output_file.is_open()) {
        this->output_file << in_text;
    }
    else return -1;

    this->output_file.close();

    return 0;
}




// Write legend line
std::string NBodyVisual::make_legend()
{
    std::stringstream ss;

    ss  << "\"Timestamp(ps)\"" \
        << this->Delimiters[this->output_mode] \
        << "\"Carrier Index\"" \
        << this->Delimiters[this->output_mode] \
        << "\"Carrier Type\"" \
        << this->Delimiters[this->output_mode] \
        << "\"Carrier Mass\"" \
        << this->Delimiters[this->output_mode] \
        << "\"X(um)\"" \
        << this->Delimiters[this->output_mode] \
        << "\"Y(um)\"" \
        << this->Delimiters[this->output_mode] \
        << "\"Z(um)\"" \
        << this->Delimiters[this->output_mode] \
        << "\"VelX(um/s)\"" \
        << this->Delimiters[this->output_mode] \
        << "\"VelY(um/s)\"" \
        << this->Delimiters[this->output_mode] \
        << "\"VelZ(um/s)\"" \
        << this->Delimiters[this->output_mode] \
        << "\"ForceX(um/s^2)\"" \
        << this->Delimiters[this->output_mode] \
        << "\"ForceY(um/s^2)\"" \
        << this->Delimiters[this->output_mode] \
        << "\"ForceZ(um/s^2)\"" << this->end_delim \
        << "\n";
    return ss.str();
}

// set up filename
void NBodyVisual::retrieve_filename()
{
    auto base_filename = \
        this->GetBaseFilename();
    this->output_filename = \
        base_filename + "_Carriers_" + \
        this->time_string + "." + \
        ExtensionMap[this->output_mode];

    return;
}

// Make end delimiter
void NBodyVisual::set_end_delim()
{
    // set up line end delimitors
    switch (this->output_mode) {
        case NBV_OMODE_LOG:
            this->end_delim = "";
            break;
        case NBV_OMODE_CSV:
            this->end_delim = this->Delimiters[NBV_OMODE_CSV];
            break;
        case NBV_OMODE_SQLITE3:
            this->end_delim = "";
            break;
        default:
            this->end_delim = "";
            break;
    }
}








/**
 *
 * Public stuff
 *
**/

// Setup output mode
void NBodyVisual::SetOutputMode(unsigned int N)
{
    if (N > NBV_OMODE_MAX) N = 0;
    this->output_mode = N;
    this->retrieve_filename();
    this->set_end_delim();
}

// Get number of entries
fp_uint_t NBodyVisual::GetEntries()
{
    return \
        static_cast<fp_uint_t>(this->Carriers.size());
}

















// Write current carriers subroutine. (for OpenMP implementation)
int NBodyVisual::WriteCarriers_sub(
    const uint64_t& istart, const uint64_t& ipoints,
    const std::string& ts_str)
{
    auto it = std::begin(this->Carriers);
    auto end = std::begin(this->Carriers);
    std::advance(it, istart);
    std::advance(end, istart+ipoints);

    for (it; it!=end; ++it) {
        this->write_carrier_sqlite3_omp(
            ts_str,
            it->second->GetIndex(),
            it->second->GetMass(),
            it->second->GetPos().x,
            it->second->GetPos().y,
            it->second->GetPos().z,
            it->second->GetVel().x,
            it->second->GetVel().y,
            it->second->GetVel().z,
            it->second->GetForce().x,
            it->second->GetForce().y,
            it->second->GetForce().z,
            it->second->GetType());

        this->WriteCarrBar.Update();
    }

    return 0;
}

// Write current carriers in the object
int NBodyVisual::WriteCarriers()
{
    this->timestamp_str = this->to_str(this->elapsed_time);
    if (this->gen_carr_log) {
        return WriteCarriers(this->timestamp_str);
    }
    else 0;
}

// Write carriers to Init table... 
int NBodyVisual::WriteCarriersInit()
{
    return WriteCarriers("Init");
}

// Write Carriers to desginated table.
int NBodyVisual::WriteCarriers(const std::string& table_name)
{
    // Do not perform db carrier generation if gen_carr_log is false
    if (!this->gen_carr_log)
        return 0;

    // Initialize progress bar
    this->WriteCarrBar = \
        ProgressBar("Writing Carr.", this->Carriers.size());

    // Setting up timestamp string.
    this->timestamp_str = table_name;

    // Multi-processing case...
#if defined(__MULTIPROCESSING_SQLITE3__)
    if (this->output_mode == NBV_OMODE_LOG || \
        this->output_mode == NBV_OMODE_CSV) {
        std::cerr \
            << "Text output mode is not safe for multi-processing..." \
            << std::endl;
        exit(-1);
    }

    // Initialize the Carrier DB
    if (this->init_db()) {
        std::cerr << "Initializing or Making database was hampered." << std::endl;
        exit(-1);
    }

    // Preparing OpenMP loop.
    uint64_t ith, nthreads, ipoints, istart;
    auto npoints = this->Carriers.size();
    auto omp_timestamp_str = this->timestamp_str;

#pragma omp parallel private(ith, nthreads, ipoints, istart)
    {
    ith = omp_get_thread_num();
    nthreads = omp_get_num_threads();
    ipoints = npoints / nthreads;
    istart = ith * ipoints;
    if (ith == nthreads - 1) ipoints = npoints - istart;

    this->WriteCarriers_sub(istart, ipoints, omp_timestamp_str);
    }

    // Not so multi-processing case.
#else

    if (this->output_mode == NBV_OMODE_SQLITE3)
        this->init_db();

    auto c_begin = std::begin(this->Carriers);
    auto c_end = std::end(this->Carriers);
    for (auto it = c_begin; it != c_end; ++it) {
        this->carr_index = it->second->GetIndex();
        this->carr_mass = it->second->GetMass();
        this->x_coord = it->second->GetPos().x;
        this->y_coord = it->second->GetPos().y;
        this->z_coord = it->second->GetPos().z;
        this->x_vel = it->second->GetVel().x;
        this->y_vel = it->second->GetVel().y;
        this->z_vel = it->second->GetVel().z;
        this->x_for = it->second->GetForce().x;
        this->y_for = it->second->GetForce().y;
        this->z_for = it->second->GetForce().z;
        this->carr_type = it->second->GetType();

        switch (this->output_mode) {
        case NBV_OMODE_LOG:
            this->write_carrier_log();
            break;
        case NBV_OMODE_CSV:
            this->write_carrier_csv();
            break;
        case NBV_OMODE_SQLITE3:
            this->write_carrier_sqlite3();
            break;
        default:
            this->output_mode = NBV_OMODE_LOG;
            this->write_carrier_log();
            break;
        }
        this->WriteCarrBar.Update();

    }

#endif /* #if defined(__MULTIPROCESSING_SQLITE3__) */

    return 0;

}


// Sqlite3 callback function
int NBodyVisual::sql_callback(int argc, char** argv, char** azColName)
{
    int i;

    for (i=0; i<argc; ++i) {
        std::cout << azColName[i] \
            << " = " \
            << (argv[i] ? argv[i] : "NULL") \
            << std::endl;
    }
    std::cout << std::endl;

    return 0;
}

/**
 * Constructors and Destructors
 *
**/
// Dummy stuff
NBodyVisual::NBodyVisual() : \
    output_mode(NBV_OMODE_LOG),
    output_filename(""),
    WriteCarrBar(ProgressBar())
{
    // Populating extensions
    ExtensionMap[NBV_OMODE_LOG] = "log";
    ExtensionMap[NBV_OMODE_CSV] = "csv";
    ExtensionMap[NBV_OMODE_SQLITE3] = "db";

    // Setting up delimiters
    Delimiters[NBV_OMODE_LOG] = "\t";
    Delimiters[NBV_OMODE_CSV] = ",";
    Delimiters[NBV_OMODE_SQLITE3] = "N/A";

    this->output_file = std::ofstream();
    this->set_end_delim();
}

// Destructor
NBodyVisual::~NBodyVisual()
{
}


/**
 * Sqlite3 callback wrapper
 *
**/
static int __callback__(void* param, int argc, char** argv, char** azColName)
{
    NBodyVisual* nb_vis = reinterpret_cast<NBodyVisual*>(param);
    return nb_vis->sql_callback(argc, argv, azColName);
}
