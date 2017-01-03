/**
 *
 * readcsv.h
 * 
 * Reads in and stores the crazy monte-carlo simulation data as
 * dictionary. Yes! that dictionary in Python!!!
 *
 * i.e. if a csv looks like this,
 *
 * some_crappy_data.csv:
 *  stuff_a, stuff_b, stuff_c,
 *  1212,1432,1e-12,
 *  12,56,4.2,
 *  894,21345,5e11,
 *  435,2356,1e-12,
 *
 * then a dictionary called SCD will store the data as,
 *
 * SCD["stuff_a"] = {"1212", "12", "894", "435"}
 * SCD["stuff_b"] = {"1432", "56", "21345", "2356"}
 * SCD["stuff_c"] = {"1e-12", "4.2", "5e11", "1e-12"}
 *
 * Written by Taylor Shin
 * Feb. 22th, 2016
 *
**/

#ifndef __readcsv_h__
#define __readcsv_h__

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <sqlite3.h>

#include "untar.h"
#include "unique_ptr.h"
#include "datatype_hash.h"

// Typedefs... maybe change them with 'using' later.
using __str__         = std::string;
using str_list        = std::vector<__str__>;
using str_list_i      = std::vector<__str__>::iterator;
using TableEntry      = DataType::DictEntryVec<__str__>;
using upTableEntry    = std::unique_ptr<TableEntry>;
using Table           = std::vector<upTableEntry>;

/**
 *
 * ReadData (class)
 *
 * the dictionary class!!
 *
**/
class ReadData
{
private:
    // Stores hash table
    Table table;
    //vector<DictEntry *> table;

    // key list
    str_list key_list;

    // reads in from file
    int read_from_file(const char* filename);
    // find if key exists
    int64_t search_me(__str__ key);

    // wipe out everything
    void init();

public:
    // retrieve the data by key
    str_list get(__str__ key);
    // retrieving by element
    __str__ get(__str__ key, int64_t index);

    // retrieve the data and convert to corresponding type T
    //
    template <typename T>
    std::vector<T> Get(__str__ key)
    {
        T Ttemp;
        std::vector<T> ret_vector;

        for (auto elem : this->get(key)) {
            Ttemp = boost::lexical_cast<T>(elem);
            ret_vector.push_back(Ttemp);
        }
        return ret_vector;
    }
    // Referencing by element
    template <typename T>
    T Get(__str__ key, int64_t index)
    { return boost::lexical_cast<T>(this->get(key, index)); }

    // Initialize with file (if initilaized without any entry)
    void ReadFile(const char* filename);
    // Add Entry with key and vector<string>
    void Add(__str__ key, str_list data);
    // Put a vector into a certain key
    void Put(__str__ key, str_list data);
    // Delete a vector with key
    void Del(__str__ key);
    // Show current status
    void Stats();
    // Returns keys
    str_list Keys();
    // Returns data entry length
    uint64_t Len();
    // Returns a list of entry length
    std::vector<uint64_t> LenList();
    // Returns data of given entry length
    uint64_t Len(__str__ key);
    // Returns number of entries
    uint64_t Entries();

    // initialize with nothing
    ReadData() : key_list(), table() {};
    // initialize with filename
    ReadData(__str__ filename) : ReadData()
    {
        read_from_file(filename.c_str());
    }
    // initialize with const char* filename
    ReadData(const char* filename) : ReadData()
    {
        read_from_file(filename);
    }

    // Destructor
    virtual ~ReadData() {}
};

/**
 *
 * Some other stuff
 *
 **/
template <typename T>
int vec_all_equal(std::vector<T> input_vec)
{
    if (input_vec.empty())
        return 0;

    T fst_elem = input_vec[0];
    for (T elem : input_vec) {
        if (fst_elem != elem) return 0;
    }
    return 1;
}

#endif