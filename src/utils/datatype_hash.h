#ifndef __datatype_hash_h__
#define __datatype_hash_h__
/**
 * datatype_hash.h
 *
 * Implementation of the Hash (sparse list) datatype.
 * --> Merely a wrapper for std::unordered_map(or just map) cor C++ 11
 * --> Might implement an old school Dict class if needed.
 *
 *  Created on: May 6, 2016
 *      Author: Taylor Shin
**/

#include <vector>
#include <deque>
#include <string>
#include <iostream>

// Select between map and unordered_map
#ifdef USE_MAP
#include <map>
#else
#include <unordered_map>
#endif
// Now include boost
#include <boost/functional/hash.hpp>

namespace DataType
{

/**
 * Defining Dict type 
 *
 * Usage example: string type key, float type vector data space ==>
 *
 *   Dict<std::string, std::vector<float>> MyStuff;
 *   --> Then access the data as
 *   MyStuff["Harro!!"] = { 0.23, 0.12, 12e13 };
 *
**/

// We have to provide hash table for specialized datatypes 
// like std::vector since std::map or std::unordered_map
// doesn't have any method to index those things.
//
template <typename SEQ> 
struct seq_hash {
    std::size_t operator() (const SEQ& seq) const
    {
        std::size_t hash = 0;
        boost::hash_range( hash, seq.begin(), seq.end() );
        return hash;
    }
};

template <class KEYTYPE, class DATATYPE>
#ifdef USE_MAP
using Dict = std::map<KEYTYPE, DATATYPE, seq_hash<KEYTYPE>>;
#else
using Dict = std::unordered_map<KEYTYPE, DATATYPE, seq_hash<KEYTYPE>>;
#endif

// Just a simple map override for simple key datatypes --> such as int, float....
template <class KEYTYPE, class DATATYPE>
#ifdef USE_MAP
using Map = std::map<KEYTYPE, DATATYPE>;
#else
using Map = std::unordered_map<KEYTYPE, DATATYPE>;
#endif

/**
 * class DictEntry
 *
 * Data container for any Dicts...
 *
**/
template <class T>
class DictEntry
{
private:
    std::string key;
    T data;

public:
    // returns key as a string
    std::string getkey() { return this->key; }
    // returns data as it is
    T getdata() { return this->data; }
    // returns a specified element from data
    std::string Get(const uint64_t i) { return this->data[i]; }
    // Assign entire vector of data
    void put_data(T input_data) { this->data = input_data; }
    // Assign data
    void push_data(std::string stuff) { this->data.push_back(stuff); }
    // set key
    void setkey(std::string input_key) { this->key = input_key; }
    // print entry
    void Stats()
    {
        std::cout << "Key: " << this->key << std::endl;
        std::cout << "Data: ";
        for (auto d : this->data) {
            std::cout << d << ", ";
        }
        std::cout << std::endl;
    }
    // print length of data
    uint64_t Len()
    { return this->data.size(); }

    // Constructors and Destructors
    DictEntry() : key() {};
    DictEntry(
        std::string key,
        T data) : \
        key(key), data(data)
    {}
    virtual ~DictEntry() {};
};


// SubTemplateClasses
template <typename VT>
using DictEntryVec = DictEntry<std::vector<VT>>;
template <typename VT>
using DictEntryDeque = DictEntry<std::deque<VT>>;


} // namespace DataType


#endif /* Include Guard */
