/**
 * pd_tcad.cc
 *
 * pd_tcad.h implementations
 *
 *  Created on March 22th 2016
 *       by Taylor Shin
**/

#include "pd_tcad.h"

using namespace pdTCAD;

/**
 * pdTCAD methods (private)
 *
**/
void TCAD::Parsor()
{
    if (!this->filename) {
        std::cerr << "pd_tcad: Wrong filename has given!!" << std::endl;
        exit(-1);
    }
}

/**
 * pdTCAD methods (public)
 *
**/

/**
 * Constructors and Destructors
 *
**/
TCAD::TCAD()
{
    this->filename = nullptr;
    this->model = 0;
    this->solver_range = std::vector<fp_t>();
}

TCAD::TCAD(const char* deck_filename)
{
    this->filename = deck_filename;
    this->Parsor();
}

TCAD::~TCAD()
{
    this->filename = nullptr;
}