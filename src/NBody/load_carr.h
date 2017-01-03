#ifndef __load_carr_h__
#define __load_carr_h__

/**
 *
 * load_carr.h
 *
 * Loads carriers from an incomplete Carrier database.
 *
 * Written by Taylor Shin
 * Aug. 22th, 2016
 *
**/

#include <string>

#ifdef _MSC_VER
#include <cctype>
#endif

#include "fputils.h"
#include "typedefs.h"

#define FP_T_ZERO FP_T(0)

/**
 *
 * The LoadCarrDB class
 *
 *
**/
class LoadCarrDB
{

private:
	// Read in carriers will be stored here.
	CarrierList dbCarriers;

	// Carrier Database Pointer
	sqlite3*    CarrierDB;

	// Some management stuff.
	uint64_t num_of_carriers;
	std::string db_fname;
	spCarrier temp_carrier;
	std::vector<std::string> tbl_list;

	// Actuallyl reads in database
	int read_database();
	int read_database(std::string elapsed_time);

	// Selects a proper table and returns its name.
	std::string select_latest_data();

protected:
	fp_t sim_time;

public:
	// Sqlite3 read in stuffs.
	int Callback(int argc, char** argv, char** azColName);
	int TableListCallback(int argc, char** argv, char** azColName);
	int ChkTableCallback(int argc, char** argv, char** azColName);

	// Returns current Carriers we have.
	CarrierList GetCarriers();
	CarrierList GetCarriers(
		std::string input_db_fname);
	CarrierList GetCarriers(
		std::string input_db_fname, std::string elapsed_time);

	// Constructors and Destructors
	LoadCarrDB(); // A Dummy constructor
	LoadCarrDB(const char* input_db_fname); // a const char* input
	LoadCarrDB(std::string input_db_fname); // std::string input

	virtual ~LoadCarrDB(); // Destructor
};


// Sqlite3 callback function --> redirects into LoadCarrDB
static int loadcarrdb_callback(
	void* param, int argc, char** argv, char** azColName);
static int loadcarrdb_tablelist_callback(
	void* param, int argc, char** argv, char** azColName);
static int loadcarrdb_chk_table_callback(
	void* param, int argc, char** argv, char** azColName);

// Numeric string comparison
#ifndef _MSC_VER
constexpr bool is_not_digit(char c) { return !std::isdigit(c); }
#else
static bool is_not_digit(char c) { return !std::isdigit(c); }
#endif
bool numeric_string_compare(
	const std::string& s1, const std::string& s2);

// Pick up max element index
uint64_t max_location(const std::vector<fp_t>& vec);

#endif /* Include guard */