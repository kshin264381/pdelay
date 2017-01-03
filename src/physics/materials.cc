/**
 *
 * materials.cc
 *
 * Include material constant definitions.
 * --> Implementation part
 *
 * Written by Taylor Shin
 * May 12th 2016
 *
**/

#include <boost/lexical_cast.hpp>

#include "materials.h"
#include "physical_constants.h"

using namespace Materials;

/**
 *
 * Private Stuff
 *
**/
// Effective mass for silicon
// took from Silvaco manual
fp_t MatData::SiEffMassN()
{
    return 1.045 + 4.5e-4 * this->lattice_temp;
}
fp_t MatData::SiEffMassP()
{
    return 0.523 + 1.4e-3 * this->lattice_temp - \
        1.48e-6 * this->lattice_temp * this->lattice_temp;
}

// Clearing this->TempMatList
int MatData::ClearTempMatList()
{
    for (MatType::iterator it=this->TempMatList.begin();
        it != this->TempMatList.end(); it++) {
        it->second.clear();
    }
    this->TempMatList.clear();

    return 0;
}

// Sets up intrinsic carrier concentration for silicon and poly
void MatData::set_intrinsic()
{
    auto N_c = this->Semiconductor["Silicon"]["NC300"];
    auto N_v = this->Semiconductor["Silicon"]["NV300"];
    auto E_gap_300 = this->Semiconductor["Silicon"]["EG300"];
    auto eg_alpha = this->Semiconductor["Silicon"]["EGALPHA"];
    auto eg_beta = this->Semiconductor["Silicon"]["EGBETA"];

    auto E_gap = E_gap_300+eg_alpha*( \
            (300.0*300.0)/(300.0+eg_beta) - \
            (this->lattice_temp*this->lattice_temp)/\
            (this->lattice_temp+eg_beta) );

    this->Semiconductor["Silicon"]["N_I"] = \
        sqrt(N_c*N_v)*\
        exp((-1.0*E_gap)/(2.0*k_B_eV*this->lattice_temp));

    this->Semiconductor["Poly"]["N_I"] = \
        this->Semiconductor["Silicon"]["N_I"];

}

/**
 *
 * Public Stuff
 *
**/
// Initialize... a few materials
// parameters were taken from Silvaco Atlas Manual.

// Silicon
void MatData::InitSilicon()
{
    // Initialize Silicon
    Material Silicon;

    // Band parameters
    Silicon["EG300"] = 1.08;
    Silicon["A"] = 4.73e-4;
    Silicon["B"] = 636.0;
    Silicon["NC300"] = 2.8e19;
    Silicon["NV300"] = 1.04e19;
    Silicon["XSI"] = 4.17;
    Silicon["MVTHN"] = this->SiEffMassN();
    Silicon["MVTHP"] = this->SiEffMassP();

    // Dielectric Constant
    Silicon["EPS"] = 11.8;

    // Lattice Mobility
    Silicon["MUN"] = 1000.0;
    Silicon["MUP"] = 500.0;
    Silicon["TMUN"] = 1.5;
    Silicon["TMUP"] = 1.5;

    // Bandgap Narrowing Parameters
    Silicon["BGN.E"] = 6.92e-3;
    Silicon["BGN.N"] = 1.3e17;
    Silicon["BGN.C"] = 0.5;

    // SRH Lifetime Parameters
    Silicon["TAUN0"] = 1.0e-7;
    Silicon["TAUP0"] = 1.0e-7;
    Silicon["NSRHN"] = 5.0e16;
    Silicon["NSRHP"] = 5.0e16;

    // Auger Parameters
    Silicon["AUGN"] = 2.8e-31;
    Silicon["AUGP"] = 9.9e-32;

    // Impact Ionization Model
    Silicon["EGRAN"] = 4.0e5;
    Silicon["BETAN"] = 1.0;
    Silicon["BETAP"] = 1.0;
    Silicon["AN1"] = 7.03e5;
    Silicon["AN2"] = 7.03e5;
    Silicon["BN1"] = 1.231e6;
    Silicon["BN2"] = 1.231e6;
    Silicon["AP1"] = 6.71e5;
    Silicon["AP2"] = 1.582e5;
    Silicon["BP1"] = 1.693e6;
    Silicon["BP2"] = 2.036e6;

    // Richardson Coefficients
    Silicon["ARICHN"] = 110.0;
    Silicon["ARICHP"] = 30.0;

    // EG
    Silicon["EGALPHA"] = 0.000473;
    Silicon["EGBETA"]  = 636.0;

    // consrh stuff
    Silicon["AN"] = 1.0;
    Silicon["BN"] = 1.0;
    Silicon["CN"] = 0.0;
    Silicon["EN"] = 0.0;
    Silicon["AP"] = 1.0;
    Silicon["BP"] = 1.0;
    Silicon["CP"] = 0.0;
    Silicon["EP"] = 0.0;

    // Ok, now put that Silicon object into this->Semiconductor
    this->Semiconductor["Silicon"] = Silicon;
    this->Semiconductor["Poly"] = Silicon;
}

// Silicon Dioxide
void MatData::InitSiO2()
{
    Material SiO2;
    SiO2["EPS"] = 3.9;

    this->Insulator["SiO2"] = SiO2;
    this->Insulator["Oxide"] = SiO2;
}

// Silicon Nitride

void MatData::InitSi3N4()
{
    Material Si3N4;
    Si3N4["EPS"] = 7.5;

    this->Insulator["SiN"] = Si3N4;
    this->Insulator["Si3N4"] = Si3N4;
    this->Insulator["Nitride"] = Si3N4;
}

// Air, Ambient, Vacuum
void MatData::InitVacuum()
{
    Material Vacuum;
    Vacuum["EPS"] = 1.0;

    this->Insulator["Air"] = Vacuum;
    this->Insulator["Vacuum"] = Vacuum;
    this->Insulator["Ambient"] = Vacuum;
}

// Metal
void MatData::InitMetal()
{
    std::vector<std::string> MetalList = \
    {
        "Aluminum",
        "Gold",
        "Silver",
        "Tungsten",
        "Titanium",
        "Platinum",
        "Palladium",
        "Cobalt",
        "Molybdinum",
        "Lead",
        "Iron",
        "Tantalum",
        "Copper",
        "Tin",
        "Nickel",
    };

    std::vector<fp_t> ResistivityList = \
    {
        2.6540,
        2.35,
        1.59,
        5.65,
        42.0,
        10.6,
        10.8,
        6.24,
        5.2,
        20.648,
        9.71,
        12.45,
        6.24,
        11.0,
        6.84,
    };

    std::vector<fp_t> TemperatureCoeff = \
    {
        0.00429,
        0.004,
        0.0041,
        0.0,
        0.0,
        0.003927,
        0.00377,
        0.00604,
        0.0,
        0.00336,
        0.00651,
        0.00383,
        0.00604,
        0.0047,
        0.0069,
    };

    // Material object container
    std::vector<Material> Metals;

    // Now update Metals
    short i;
    for (i=0; i<MetalList.size(); i++) {
        Material AMetal;
        AMetal["RESIS"] = ResistivityList[i];
        AMetal["TEMPC"] = TemperatureCoeff[i];

        // Updating workfunctions...
        switch (i) {
            // Aluminum
            case 0:
                AMetal["WF"] = 4.10;
            // Molybdinum
            case 8:
                AMetal["WF"] = 4.53;
            // Tungsten
            case 3:
                AMetal["WF"] = 4.63;
            default:
                AMetal["WF"] = 0.0;
        } // TODO: Might change the default Workfunction value later...
    }
}


// Update lattice temperature and effective mass values for silicon and polysilicon
void MatData::SetTemp(fp_t new_temperature)
{
    this->lattice_temp = new_temperature;
    auto iter = this->Semiconductor.find("Silicon");
    if (iter != this->Semiconductor.end()) {
        this->Semiconductor["Silicon"]["MVTHN"] = \
            this->SiEffMassN();
        this->Semiconductor["Silicon"]["MVTHP"] = \
            this->SiEffMassP();
    }

    iter = this->Semiconductor.find("Poly");
    if (iter != this->Semiconductor.end()) {
        this->Semiconductor["Poly"]["MVTHN"] = \
            this->SiEffMassN();
        this->Semiconductor["Poly"]["MVTHP"] = \
            this->SiEffMassP();
    }

    this->set_intrinsic();
}

// Sqlite3 callback function for MatData class
// --> reads in database and stores everything to Temporary storage
//     called TempMat.
int MatData::Callback(int argc, char** argv, char** azColName)
{
    Material SomeMaterial;
    std::string MaterialName;
    std::string Property;
    fp_t Value;
    //std::stringstream ss;

    // Read in current table.
    for (auto i=0; i<argc; i++) {
        if (argv[i]) {
            Property = azColName[i];
            if (Property == "Name") {
                MaterialName = argv[i];
                continue;
            }
            //ss << argv[i];
            //ss >> Value;
            try {
                Value = boost::lexical_cast<fp_t>(std::string(argv[i]));
            }
            catch(const boost::bad_lexical_cast&) {
                Value = 0.0;
            }
            SomeMaterial[Property] = Value;
        }
    }

    this->TempMatList[MaterialName] = SomeMaterial;

    return 0;
}

/**
 *
 * Constructors and destructors
 *
**/
// Default option (just initialize a few materials)
MatData::MatData()
{
    this->Semiconductor = MatType{};
    this->Insulator = MatType{};
    this->Metal = MatType{};
    this->TempMatList = MatType{};

    this->lattice_temp = 300.0;

    this->InitSiO2();
    this->InitSi3N4();
    this->InitVacuum();
    this->InitMetal();

    this->InitSilicon();
}
// With lattice temperature
MatData::MatData(fp_t LatTemp) : MatData()
{
    this->lattice_temp = LatTemp;
    this->InitSilicon();
    this->set_intrinsic();
}

// With data file
MatData::MatData(const char* sqlite3_filename) : MatData()
{
    sqlite3*          MatDB;
    char*             zErrMsg = nullptr;
    int               rc;
    const char*       data = "Callback function called!!";

    // Stop everything if sqlite3_filename is a bogus
    if ((sqlite3_filename != nullptr) && (sqlite3_filename == '\0')) {
        std::cerr \
            << "Ouch!! Material Database file is not given!!" \
            << std::endl;
        exit(-1);
    }

    // SQL command store.
    std::string             sql;

    // Opening the Material database.
    rc = sqlite3_open(sqlite3_filename, &MatDB);
    // Error handling... who knows? we may not use it or not?
    fprintf(stderr,
        "Trying to read Material Database from:\n%s\n",
        sqlite3_filename);
    if (rc != SQLITE_OK) {
        fprintf(stderr,
            "%s] Material Database read in failure!!\n",
            zErrMsg);
        exit(-1);
    }

    // Ok, read det_material from Semiconductor
    sql = "SELECT * from Semiconductor";
    // Cleaning up temoprary data space
    this->ClearTempMatList();
    rc = sqlite3_exec(MatDB, sql.c_str(), Materials::Callback, this, &zErrMsg);
    this->Semiconductor = this->TempMatList;
    this->SetTemp(this->lattice_temp);

    // Ok, read all material from Insulator
    sql = "SELECT * from Insulator";
    // Cleaning up temoprary data space
    this->ClearTempMatList();
    rc = sqlite3_exec(MatDB, sql.c_str(), Materials::Callback, this, &zErrMsg);
    this->Insulator = this->TempMatList;

    // Ok, read all material from Metal
    sql = "SELECT * from Metal";
    // Cleaning up temoprary data space
    this->ClearTempMatList();
    rc = sqlite3_exec(MatDB, sql.c_str(), Materials::Callback, this, &zErrMsg);
    this->Metal = this->TempMatList;

    // Ok, readout all the data from database
    sqlite3_close(MatDB);
    fprintf(stdout, "Database readout complete!!\n\n");
}

// Some other stuff
// Outside Callback for Sqlite3 data retrieval
// --> Look upside for the 'real' callback function.
int Materials::Callback(void* param, int argc, char** argv, char** azColName)
{
    MatData* mat_data = reinterpret_cast<MatData*>(param);
    return mat_data->Callback(argc, argv, azColName);
}
