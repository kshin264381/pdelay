/**
 * readcsv.cc
 *
 * Implementation of the ReadData dictionary.
 *
 * Written by Taylor Shin
 * Feb. 22th, 2016
 *
**/

#include "readcsv.h"
#include "fputils.h"

/**
 * ReadData
 *
 * Private part
 *
**/
// Read from filename
int ReadData::read_from_file(const char* filename)
{
    __str__ fdata;
    __str__ fdata_line;
    this->key_list = {};
    __str__ fname_str = __str__(filename);

    // if filename has tar.gz or tar.bz2
    __str__ extension = fname_str.substr(fname_str.find_last_of(".")+1);
    boost::algorithm::to_lower(extension);
    if ( extension == "gz" || \
        extension == "bz2" || \
        extension == "tgz") {
        untarFile(fname_str.c_str(), fdata);

        if (fdata == __str__()) {
            std::cout << "Looks like an empty tarball. No point to continue anymore!!" << std::endl;
            exit(0);
        }

        std::stringstream fdata_s(fdata);

        uint64_t f_i = 0; // vertical scan index
        uint64_t t_i = 0; // horizontal scan index
        while (getline(fdata_s, fdata_line, '\n')) {
            std::stringstream fdata_line_s(fdata_line);
            __str__ token = {};
            t_i = 0; // Initialize horizontal scan index
            while (getline(fdata_line_s, token, ',')) {
                // Generate and assign DictEntries for each 
                // sections...
                if (!f_i) {
                    this->key_list.push_back(token);
                    str_list entryData = {};
                    this->table.push_back(
                        std::make_unique<TableEntry>(token, entryData));
                    t_i++;
                    continue;
                }
                // Assign numeric data from the datafile
                this->table[t_i]->push_data(token);
                t_i++;
            }
            f_i++;
        }
        std::cout << "Tarball Data Container: " << filename << std::endl;
        // Display how many stuffs we've read out.
        this->Stats();
    }
    // if the input was sqlite3 database file.
    else if ( extension == "db" ) {
        // TODO: Implement sqlite3 data readout routine here.
    }
    else {
        std::cout << "Not valid filetype.!!" << std::endl;
        exit(-1);
    }

    return 0;
}

// Initialize (or delete everything)
void ReadData::init()
{
    this->key_list = {};
    this->table = Table{};
}

// finds key location
int64_t ReadData::search_me(__str__ key)
{
    str_list_i s_r_pos = \
        find(this->key_list.begin(),
            this->key_list.end(), key);
    if (s_r_pos != this->key_list.end())
        return static_cast<int64_t>(s_r_pos - this->key_list.begin());
    else return -1;
}

/**
 * ReadData
 *
 * Public part
 *
**/
// retrieve data (vector) by key
str_list ReadData::get(__str__ key)
{
    auto key_pos = this->search_me(key);
    if (key_pos > -1) {
        return this->table[key_pos]->getdata();
    }
    else {
        return str_list{};
    }
}

// retrieve a specific element from a key
__str__ ReadData::get(__str__ key, int64_t index)
{
    auto key_pos = this->search_me(key);
    if (key_pos > -1) return this->table[key_pos]->getdata()[index];
    else return __str__("");
}

// Read file manually after initialization.
void ReadData::ReadFile(const char* filename)
{
    this->init();
    this->read_from_file(filename);
}

// Add Entry with key and str_list
void ReadData::Add(__str__ key, str_list data)
{
    this->key_list.push_back(key);
    this->table.push_back(std::make_unique<TableEntry>(key, data));
}

// Put a vector into a certain key
void ReadData::Put(__str__ key, str_list data)
{
    int64_t elem_loc = search_me(key);
    // If the key exists, overwrite the data.
    if (elem_loc > 0) {
        this->table[elem_loc]->put_data(data);
    }
    else {
        this->Add(key, data);
    }
}

// Delete a vector with key
void ReadData::Del(__str__ key)
{
    int64_t elem_loc = search_me(key);
    if (elem_loc >= 0) {
        // Remove heap allocated DictEntry
        // delete[] this->table[elem_loc];
        // --> deprecated since unique pointers. They will disappear 
        // as soon as the data vector disappears.
        //

        // now delete the element.
        this->table.erase( this->table.begin() + elem_loc );
        this->key_list.erase( this->key_list.begin() + elem_loc );
    }
}

// Show current status
void ReadData::Stats()
{
    std::cout << "***********************************************" << std::endl;
    std::cout << "*********** ReadData Status Report ************" << std::endl;
    std::cout << "***********************************************" << std::endl;
    // Show the number of keys I have
    std::cout << std::endl;
    std::cout << " Total # of Keys: " << this->key_list.size() << std::endl;

    // Show data entry size per key.
    std::cout << " Entries: " << std::endl;
    for (auto key_name : this->key_list) {
        std::cout << "     " << key_name << " : ";
        std::cout << this->get(key_name).size()
            << " entries" << std::endl;
    }
    std::cout << std::endl;
    std::cout << "***********************************************" << std::endl;
    std::cout << "***********************************************" << std::endl;
    std::cout << "***********************************************" << std::endl;
}

// Returns Keys
str_list ReadData::Keys()
{
    return this->key_list;
}

// Returns data entry length
uint64_t ReadData::Len()
{
    std::vector<uint64_t> len_list = this->LenList();
    if (vec_all_equal<uint64_t>(len_list)) return len_list[0];
    else {
        std::cerr << "ReadData.Len(): Found data length mismatch between entries..." << std::endl;
        std::cerr << "Try to use LenList()" << std::endl;
        return 0;
    }
}

// Returns a list of entry length
std::vector<uint64_t> ReadData::LenList()
{
    std::vector<uint64_t> len_list;
    for (__str__ key : this->key_list) {
        len_list.push_back(this->get(key).size());
    }
    return len_list;
}

// Returns data of given entry length
uint64_t ReadData::Len(__str__ key)
{
    return (uint64_t)this->get(key).size();
}

// Returns number of entries
uint64_t ReadData::Entries()
{
    return this->key_list.size();
}
