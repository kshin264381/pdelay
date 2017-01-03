/**
 *
 * A homemade tarfile (tar.gz, tar.bz2) reader.
 * adopted from 
 * https://techoverflow.net/blog/2013/03/29/reading-tar-files-in-c/
 *
 * Written by Taylor Shin
 * Feb. 16th 2016
 *
**/

#ifndef __untar_h__
#define __untar_h__

#include <cstdlib>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <cmath>
#include <iostream>
#include <string>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/algorithm/string.hpp>

using namespace boost::iostreams;

// Converts an ascii digit to the corresponding number
constexpr int ascii_to_number(int i) { return (i-48); }

// Converts std::string to lowercase
std::string lowercase(std::string input_str);

// Converts std::string to uppercase
std::string uppercase(std::string input_str);

/**
 * Decode a TAR octal number.
 *
**/
static uint64_t decodeTarOctal(char* data, size_t size = 12)
{
	unsigned char* currentPtr = (unsigned char*)data + size;
	uint64_t sum = 0;
	uint64_t currentMultiplier = 1;

	// Skipping everything after NUL/space character.
	unsigned char* checkPtr = currentPtr;
	for (; checkPtr >= (unsigned char*)data; checkPtr--) {
		if ((*checkPtr) == 0 || (*checkPtr) == ' ') {
			currentPtr = --checkPtr;
		}
	}

	for (; currentPtr >= (unsigned char*)data; currentPtr--) {
		sum += ascii_to_number(*currentPtr) * currentMultiplier;
		currentMultiplier *= 8;
	}
	return sum;
}

/**
 * List Files within a tarball (tar.gz or tar.bz2 only...)
 *
**/
int untarFile(const char* inputFilename, std::string& csvData);

/**
 * Defining struct to maintain TAR fileheader
 *
**/
struct TARFileHeader {
    char filename[100]; // null-terminated... should be..
    char mode[8];
    char uid[8];
    char gid[8];
    char filesize[12];
    char lastModification[12];
    char checksum[8];
    char typeFlag;
    char linkedFileName[100];
    char ustarIndicator[6];
    char ustarVersion[2]; // 00
    char ownerUserName[32];
    char ownerGroupName[32];
    char deviceMajorNumber[8];
    char deviceMinorNumber[8];
    char filenamePrefix[155];
    char padding[12];

    bool isUSTAR() {
        return (memcmp("ustar", ustarIndicator, 5) == 0);
    }

    size_t getFileSize() {
        return decodeTarOctal(filesize, 12);
    }

    bool checkChecksum() {
        char* originalChecksum;
        memcpy(originalChecksum, checksum, 8);
        memset(checksum, ' ', 8);

        int64_t unsignedSum = 0;
        int64_t signedSum = 0;
        for (auto i = 0; i < sizeof (TARFileHeader); i++) {
            unsignedSum += ((unsigned char*) this)[i];
            signedSum += ((signed char*) this)[i];
        }
        memcpy(checksum, originalChecksum, 8);

        uint64_t referenceChecksum = decodeTarOctal(originalChecksum, 12);
        return (referenceChecksum == unsignedSum || referenceChecksum == signedSum);
    }
};



#endif