/**
 *
 * load_carr.cc
 *
 * Loads carriers from an incomplete Carrier database.
 * (Implementation)
 *
 * Written by Taylor Shin
 * Aug. 22th, 2016
 *
**/

#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include <map>

#include <sqlite3.h>

#include <boost/lexical_cast.hpp>

#include "carrier.h"
#include "load_carr.h"

/**
 *
 * Private stuffs
 *
 *
**/



// Discovers proper(?) elapsed time and reads out.
int LoadCarrDB::read_database()
{
    char*        zErrMsg = nullptr;
    int          rc;
    const char*  data = "Callback function ended!!";
    std::string  elapsed_time;

    // SQL command storage
    std::string  sqlcmd;

    // Scrap everything if db_fname is empty... obviously.
    if (this->db_fname.empty()) {
        std::cerr << "Database file is not given!! Exiting!!" << std::endl;
        exit(-1);
    }

    // Opening the Carrier database.
    rc = sqlite3_open(this->db_fname.c_str(), &this->CarrierDB);
    // Error handling
    if (rc != SQLITE_OK) {
        std::cerr << zErrMsg << "] Carrier read in failure!!" << \
        std::endl;
        exit(-1);
    }

    // Ok let's list tables and select a newest elapsed time.
    sqlcmd = "SELECT * FROM SQLITE_MASTER WHERE type='table'";
    // Now populate table list
    this->tbl_list.erase(this->tbl_list.begin(), this->tbl_list.end());
    rc = sqlite3_exec(
        this->CarrierDB,
        sqlcmd.c_str(),
        loadcarrdb_tablelist_callback,
        this, &zErrMsg);

    // Then find out a proper newest data.
    elapsed_time = this->select_latest_data();

    // Preparing SQLite command
    sqlcmd = "SELECT * from '"+elapsed_time+"'";

    // Now read in table of given elapsed time.
    rc = sqlite3_exec(
        this->CarrierDB,
        sqlcmd.c_str(),
        loadcarrdb_callback,
        this, &zErrMsg);

    // Close up DB
    sqlite3_close(this->CarrierDB);
    std::cout \
        << "Carrier readout from timestamp " \
        << elapsed_time << " ps has been completed!!" \
        << std::endl;

    // Set up time
    this->sim_time = boost::lexical_cast<fp_t>(elapsed_time);

    return 0;
}



// Reads certain table at specific time.
int LoadCarrDB::read_database(std::string elapsed_time)
{
    char*        zErrMsg = nullptr;
    int          rc;
    const char*  data = "Callback function ended!!";

    // SQL command storage
    std::string  sqlcmd;

    // Opening the Carrier database.
    rc = sqlite3_open(this->db_fname.c_str(), &this->CarrierDB);
    if (rc != SQLITE_OK) {
        std::cerr \
            << zErrMsg << "] Carrier read in failure!!" \
            << std::endl;
        exit(-1);
    }

    // Preparing SQLite command
    sqlcmd = "SELECT * from '"+elapsed_time+"'";

    // Now read in table of given elapsed time.
    rc = sqlite3_exec(
        this->CarrierDB, sqlcmd.c_str(), loadcarrdb_callback,
        this, &zErrMsg);

    // Close up DB
    sqlite3_close(this->CarrierDB);
    std::cout \
        << "Carrier readout from timestamp " \
        << elapsed_time << " ps has been completed!!" \
        << std::endl;

    // Set up time
    this->sim_time = boost::lexical_cast<fp_t>(elapsed_time);

    return 0;
}

// Helps read_database() to select the proper table.
std::string LoadCarrDB::select_latest_data()
{
    if (this->tbl_list.empty()) {
        std::cerr \
            << "LoadCarrDB: table list is empty!!" << std::endl;
        exit(-1);
    }

    std::string sql_cmd;
    std::stringstream sql_cmd_ss;
    int rc = 0;

    // Weed out weird name other than number
    std::vector<std::string> tbl_list_numeric;
    std::vector<fp_t> tbl_list_numeric_fp_t;
    fp_t tbl_time = FP_T_ZERO;
    for (auto tbl_name : this->tbl_list) {
        try {
            tbl_time = boost::lexical_cast<fp_t>(tbl_name);
            tbl_list_numeric.push_back(tbl_name);
            tbl_list_numeric_fp_t.push_back(tbl_time);
        }
        catch (const boost::bad_lexical_cast) {
            std::cerr \
                << "Table: " << tbl_name << std::endl \
                << "Doesn't seem to be valid. Skipping.." \
                << std::endl;
        }
    } /* for (auto tbl_name : this->tbl_list) */

    // Ok, now we got to select a proper table!
    uint64_t max_element_index;
    std::string max_table;
    bool table_empty = false;
    sqlite3_stmt* stmt;
    while (true) {
        max_element_index = \
            max_location(tbl_list_numeric_fp_t);
        max_table = tbl_list_numeric[max_element_index];

        // Check the table if it's NULL or not.
        sql_cmd_ss.clear();
        sql_cmd_ss.str(std::string());
        sql_cmd_ss << "SELECT * from '" << max_table << "'";
        sql_cmd = sql_cmd_ss.str();
        rc = sqlite3_prepare_v2(
            this->CarrierDB, sql_cmd.c_str(), -1, &stmt, 0);
        if (rc != SQLITE_OK) {
            std::cerr \
                << "select_latest_data: SQLite Prepare V2 error!!" \
                << std::endl;
            exit(-1);
        }
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
            std::cerr \
                << "select_latest_data: Oops, sqlite3 step error!!" \
                << std::endl;
        }


        if (rc == SQLITE_DONE) table_empty = true;
        else table_empty = false;

        if (!table_empty) {
            std::cout << "Selecting table: " \
                << max_table << std::endl;
            break;
        }
        else {
            tbl_list_numeric.erase(
                tbl_list_numeric.begin()+max_element_index);
            tbl_list_numeric_fp_t.erase(
                tbl_list_numeric_fp_t.begin()+max_element_index);
        }
    } /* while (true) */

    return max_table;
}









/**
 *
 * Protected stuffs
 *
 *
**/















/**
 *
 * Public stuffs
 *
 *
**/


//
// Sqlite3 callbacks
//

// Reads in Carriers
int LoadCarrDB::Callback(int argc, char** argv, char** azColName)
{
    this->temp_carrier = nullptr;
    std::string colname = {};

    uint64_t index;
    std::string carr_type;
    fp_t carr_mass;
    fp_t X, Y, Z;
    fp_t VX, VY, VZ;
    fp_t FX, FY, FZ;

    Loc tempLoc = Loc{};
    Vel tempVel = Vel{};
    Force tempForce = Force{};

    // Read in current table
    for (auto i=0; i<argc; ++i) {
        if (argv[i]) {
            // Initialize a carrier
            colname = azColName[i];
            // Ok, let's parse columns.
            if (colname == "CARR_INDEX") {
                try {
                    index = boost::lexical_cast<uint64_t>(argv[i]);
                }
                catch(const boost::bad_lexical_cast&) {
                    index = 0;
                }
            }
            else if (colname == "TYPE") {
                carr_type = std::string(argv[i]);
            }
            else if (colname == "MASS") {
                try {
                    carr_mass = boost::lexical_cast<fp_t>(argv[i]);
                }
                catch (const boost::bad_lexical_cast&) {
                    carr_mass = FP_T_ZERO;
                }
            }
            else if (colname == "X") {
                try {
                    X = boost::lexical_cast<fp_t>(argv[i]);
                }
                catch(const boost::bad_lexical_cast&) {
                    X = FP_T_ZERO;
                }
            }
            else if (colname == "Y") {
                try {
                    Y = boost::lexical_cast<fp_t>(argv[i]);
                }
                catch(const boost::bad_lexical_cast&) {
                    Y = FP_T_ZERO;
                }
            }
            else if (colname == "Z") {
                try {
                    Z = boost::lexical_cast<fp_t>(argv[i]);
                }
                catch(const boost::bad_lexical_cast&) {
                    Z = FP_T_ZERO;
                }
            }
            else if (colname == "VX") {
                try {
                    VX = boost::lexical_cast<fp_t>(argv[i]);
                }
                catch(const boost::bad_lexical_cast&) {
                    VX = FP_T_ZERO;
                }
            }
            else if (colname == "VY") {
                try {
                    VY = boost::lexical_cast<fp_t>(argv[i]);
                }
                catch(const boost::bad_lexical_cast&) {
                    VY = FP_T_ZERO;
                }
            }
            else if (colname == "VZ") {
                try {
                    VZ = boost::lexical_cast<fp_t>(argv[i]);
                }
                catch(const boost::bad_lexical_cast&) {
                    VZ = FP_T_ZERO;
                }
            }
            else if (colname == "FX") {
                try {
                    FX = boost::lexical_cast<fp_t>(argv[i]);
                }
                catch(const boost::bad_lexical_cast&) {
                    FX = FP_T_ZERO;
                }
            }
            else if (colname == "FY") {
                try {
                    FY = boost::lexical_cast<fp_t>(argv[i]);
                }
                catch(const boost::bad_lexical_cast&) {
                    FY = FP_T_ZERO;
                }
            }
            else if (colname == "FZ") {
                try {
                    FZ = boost::lexical_cast<fp_t>(argv[i]);
                }
                catch(const boost::bad_lexical_cast&) {
                    FZ = FP_T_ZERO;
                }
            }
        } /* if (argv[i]) */
    } /* for (auto i=0; i<argc; i++) */

    tempLoc = {X, Y, Z};
    tempVel = {VX, VY, VZ};
    tempForce = {FX, FY, FZ};

    // Ok, ready the temporary carrier.
    this->temp_carrier = std::make_shared<Carrier>();
    // this->temp_carrier->Set(
    // 	carr_type, tempLoc, tempVel, tempForce);
    this->temp_carrier->SetType(carr_type);
    this->temp_carrier->SetPos(tempLoc);
    this->temp_carrier->SetVel(tempVel);
    this->temp_carrier->SetForce(tempForce);
    this->temp_carrier->SetIndex(index);

    // Setting up mass... but we need to reconsider where to set up
    // mass, anyway...
    this->temp_carrier->SetMass(carr_mass);
    this->temp_carrier->setID();

    // then push it into container.
    this->dbCarriers[index] = this->temp_carrier;

    return 0;
}

// Extracts Table list from database.
int LoadCarrDB::TableListCallback(int argc, char** argv, char** azColName)
{
    std::string colname;

    for (auto i=0; i<argc; ++i) {
        colname = azColName[i];
        if (colname == std::string("tbl_name")) {
            this->tbl_list.push_back(std::string(argv[i]));
        }
    }

    return 0;
}


// Check if current table is empty or not.
int LoadCarrDB::ChkTableCallback(int argc, char** argv, char** azColName)
{
    return 0;
}








//
// Input & Output stuffs
//

// Returns the actual Carrier Map
CarrierList LoadCarrDB::GetCarriers()
{
    return this->dbCarriers;
}

// Reads in database and returns the carriers
CarrierList LoadCarrDB::GetCarriers(std::string input_db_fname)
{
    this->db_fname = input_db_fname;
    this->read_database();
    return this->dbCarriers;
}

// Reads in database with given time slot and returns the carriers
CarrierList LoadCarrDB::GetCarriers(std::string input_db_fname, std::string elapsed_time)
{
    this->db_fname = input_db_fname;
    this->read_database(elapsed_time);
    return this->dbCarriers;
}








/**
 *
 * Constructors and destructors
 *
 *
**/
LoadCarrDB::LoadCarrDB() :
    num_of_carriers(0),
    db_fname(std::string()),
    temp_carrier(nullptr),
    tbl_list({}),
    CarrierDB(nullptr),
    sim_time(FP_T(0.0))
{
}

LoadCarrDB::LoadCarrDB(const char* input_db_fname) : LoadCarrDB::LoadCarrDB()
{
    this->db_fname = std::string(input_db_fname);
}

LoadCarrDB::LoadCarrDB(std::string input_db_fname) : LoadCarrDB::LoadCarrDB()
{
    this->db_fname = input_db_fname;
}

// Destructor
LoadCarrDB::~LoadCarrDB()
{

}













/**
 *
 * Callback functions (Redirections)
 *
 * --> Redirects callbacks to LoadCarrDB class via void pointer.
 *
**/
static int loadcarrdb_callback(
    void* param, int argc, char** argv, char** azColName)
{
    LoadCarrDB* loadcarrdb = reinterpret_cast<LoadCarrDB*>(param);
    return loadcarrdb->Callback(argc, argv, azColName);
}

static int loadcarrdb_tablelist_callback(
    void* param, int argc, char** argv, char** azColName)
{
    LoadCarrDB* loadcarrdb = reinterpret_cast<LoadCarrDB*>(param);
    return loadcarrdb->TableListCallback(argc, argv, azColName);
}

static int loadcarrdb_chk_table_callback(void* param, int argc, char** argv, char** azColName)
{
    LoadCarrDB* loadcarrdb = reinterpret_cast<LoadCarrDB*>(param);
    return loadcarrdb->ChkTableCallback(argc, argv, azColName);
}










/**
 *
 * Some utilities
 *
 *
**/
bool numeric_string_compare(
    const std::string& s1, const std::string& s2)
{
    std::string empty = {};
    if (s1 == empty || s2 == empty) {
        std::cerr << "numeric_string_compare: Can't handle empty string!!" << std::endl;
        //return false;
        exit(-1);
    }

    std::string::const_iterator it1 = s1.begin(), it2 = s2.begin();

    if (std::isdigit(s1[0]) && std::isdigit(s2[0])) {
        int n1, n2;
        std::stringstream ss(s1);
        ss >> n1;
        ss.clear();
        ss.str(s2);
        ss >> n2;

        if (n1 != n2) return n1 < n2;

        it1 = std::find_if(s1.begin(), s1.end(), is_not_digit);
        it2 = std::find_if(s2.begin(), s2.end(), is_not_digit);
    }

    return std::lexicographical_compare(it1, s1.end(), it2, s2.end());

}


/**
 *
 * Detects max element location.
 *
 *
**/
uint64_t max_location(const std::vector<fp_t>& vec)
{
    if (vec.empty()) {
        std::cerr << "max_location: Oops cant' work with empty vector!!" << std::endl;
        return 0;
    }

    auto result = std::max_element(vec.begin(), vec.end());
    return static_cast<uint64_t>(std::distance(vec.begin(), result));
}