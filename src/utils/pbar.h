/**
 *
 * pbar.h
 *
 * Simple progress bar impelementation.
 *
 * Impelementations!!
 *
 * Written by Taylor Shin
 * Feb. 26th, 2016
 *
**/

#ifndef __pbar_h__
#define __pbar_h__

#include <iostream>
#include <iomanip>
#include <ctime>

#include "fputils.h"

#define BAR_MAX_WIDTH 55

class ProgressBar {
private:
    std::string Title;
    uint64_t IterMax;
    uint64_t IterCurr;

    int      bar_width;
    int      pos;
    float    progress;
    int      bar_max_width;

    void draw_progress();
    void update_current_progress();

public:
    void Update();
    void Update(const uint64_t& iter_amount);

    ProgressBar();
    ProgressBar(std::string title, const uint64_t& max_iter);
    virtual ~ProgressBar();
};

#endif