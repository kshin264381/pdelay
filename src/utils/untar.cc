/**
 * A homemade tarfile (tar.gz, tar.bz2) reader.
 * adopted from 
 * https://techoverflow.net/blog/2013/03/29/reading-tar-files-in-c/
 *
 * Written by Taylor Shin
 * Feb. 16th 2016
 *
**/

#include "untar.h"

// Converts std::string to lowercase
std::string lowercase(std::string input_str)
{
    transform(input_str.begin(), input_str.end(), input_str.begin(), ::tolower);
    return input_str;
}

// Converts std::string to uppercase
std::string uppercase(std::string input_str)
{
    transform(input_str.begin(), input_str.end(), input_str.begin(), ::toupper);
    return input_str;
}

/**
 * Decode a TAR octal number.
 * 
 * actual implementation.
**/
//static uint64_t decodeTarOctal(char* data, size_t size = 12)
//{
//    unsigned char* currentPtr = (unsigned char*) data + size;
//    uint64_t sum = 0;
//    uint64_t currentMultiplier = 1;
//
//    // Skipping everything after NUL/space character.
//    unsigned char* checkPtr = currentPtr;
//    for (; checkPtr >= (unsigned char*) data; checkPtr--) {
//        if ((*checkPtr) == 0 || (*checkPtr) == ' ') {
//            currentPtr = --checkPtr;
//        }
//    }
//
//    for (; currentPtr >= (unsigned char*) data; currentPtr--) {
//        sum += ascii_to_number(*currentPtr) * currentMultiplier;
//        currentMultiplier *= 8;
//    }
//    return sum;
//}

/**
 * List Files within a tarball (tar.gz or tar.bz2 only...)
 *
 * In fact, LZMA decompression is already been implemented by
 * some other boost fan: https://github.com/zmij/shaman/wiki/LZMA-CPP-filter
 *
 * But I'm not gonna implement at this stage yet.
 *
**/
int untarFile(const char* inputFilename, std::string& csvData)
{
    std::ifstream fin(inputFilename, std::ios_base::in | std::ios_base::binary);
    filtering_istream in;
    std::string filename(inputFilename);

    // Sudden Decompression!! (which sometimes sucks in Space Quest)
    if (boost::algorithm::iends_with(lowercase(filename), ".gz")) {
        in.push(gzip_decompressor());
    }
    else if (boost::algorithm::iends_with(lowercase(filename), ".bz2")) {
        in.push(bzip2_decompressor());
    }
    else if (boost::algorithm::iends_with(lowercase(filename), ".tar")) {
        // Pass, no decompression required!!
    }
    else {
        std::cerr << "untarFile: Uh oh, wrong file suffix!! only .tar.gz, tar.bz2, or .tar are valid!!" << std::endl;
        return -1;
    }
    in.push(fin);

    char zeroBlock[512];
    memset(zeroBlock, 0, 512);

    // Start reading the file!!
    auto nextEntryHasLongName = false;
    while (in) { // Stopping at EOF
        // Initializes currentFileHeader to zero manually for MSVC
#ifdef _MSC_VER
        TARFileHeader currentFileHeader = \
        {
            {},
            {},
            {},
            {},
            {},
            {},
            {},
            {},
            {},
            {},
            {},
            {},
            {},
            {},
            {},
            {},
            {}
        };
#else
        // Initializing a zero filled block we can compare against.
        TARFileHeader currentFileHeader;
#endif

        // reading file header
        in.read((char*)  &currentFileHeader, 512);

        // Break the read in if zero-only blocks found
        if (memcmp(&currentFileHeader, zeroBlock, 512) == 0) {
            std::cout << "untarFile: Found TAR EOF" << std::endl;
            break;
        }

        // Converting the filename to a std::string
        // However, long filenames (100+) requires special handling and only 
        // USTAR support such long filenames.
        //
        std::string content_fname(currentFileHeader.filename, std::min((size_t)100, strlen(currentFileHeader.filename)));

        // Remove the next block if don't want USTAR, longname file stuff.
        size_t prefixLength = strlen(currentFileHeader.filenamePrefix);
        if (prefixLength > 0) {
            content_fname = std::string(currentFileHeader.filenamePrefix, std::min((size_t)155, prefixLength)) + "/" + filename;
        }

        // Working on tarfile content listing
        //
        // If the file is a normal file!! Just read in as a text!!
        if (currentFileHeader.typeFlag == '0' ||
            currentFileHeader.typeFlag == 0) {

            if (nextEntryHasLongName) {
                content_fname = std::string(currentFileHeader.filename);
                in.read((char*) &currentFileHeader, 512);
                nextEntryHasLongName = false;
            }

            size_t size = currentFileHeader.getFileSize();
            std::cout << "Found File '" << content_fname << "' (" << size << " bytes)" << std::endl;

            if (boost::algorithm::iends_with(lowercase(content_fname), ".csv")) {
                // actually reading in file
                std::cout << "Reading it..." << std::endl;
                char* fileData = new char[size+1];
                //std::string fileData;
                //unique_ptr<char []> fileData(new char[size+1]);
                in.read(fileData, size);
                // terminating fileData string with \0
                fileData[size] = '\0';
                csvData = std::string(fileData);
                //cout << csvData << endl;
                delete[] fileData;

                // cout << "Extracting..." << endl;
                // stream_buffer<file_sink> buffer(filename);
                // ostream out(&buffer);
                // out << csvData;
                break;
            }

            // Ignoring padding bytes...
            size_t paddingBytes = (512 - (size % 512)) % 512;
            in.ignore(paddingBytes);
        }
        else if (currentFileHeader.typeFlag == '5' || currentFileHeader.typeFlag == 5) {
            std::cout << "Found directory '" << filename << "'" << std::endl;
        }
        else if (currentFileHeader.typeFlag == 'L') {
            nextEntryHasLongName = true;
        }
        else {
            std::cout << "Found unhandled TAR entry type... " << currentFileHeader.typeFlag << std::endl;
        } /* if currentFileHeader stuff */

    } /* while (in) */

    // Finishing up!!
    fin.close();
    return 0;
}
