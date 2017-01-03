/**
 *
 * pbar.cc
 *
 * Simple progress bar impelementation.
 *
 * Impelementations!!
 *
 * Written by Taylor Shin
 * Feb. 26th, 2016
 *
**/

#include "pbar.h"

// Private stuff
void ProgressBar::draw_progress()
{
    // Initial point
    if (this->Title.length())
        std::cout << this->Title << " :[";
    else std::cout << "[";

    // Draw the actual bar
    for (int i = 0; i < this->bar_width; ++i) {
        if (i < this->pos) std::cout << "=";
        else if (i == this->pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] ";

    // Write progress info.
    std::cout << "(" << int(this->progress * 100.0) << " %) ";
    std::cout << "[" << this->IterCurr << "/" << this->IterMax << "]\r";
    std::cout.flush();
}

void ProgressBar::update_current_progress()
{
    this->progress = \
        (float)(this->IterCurr)/this->IterMax;
    this->pos = (int)(this->bar_width * this->progress);
}

// Public stuff
void ProgressBar::Update()
{
    this->IterCurr++;
    this->update_current_progress();
    this->draw_progress();

    if (fp_equal<float>(this->progress, 1.0)) std::cout << std::endl;
}

void ProgressBar::Update(const uint64_t& iter_amount)
{
    this->IterCurr += iter_amount;
    this->update_current_progress();
    this->draw_progress();

    if (fp_equal<float>(this->progress, 1.0)) std::cout << std::endl;
}

// Constructors and Destructors
ProgressBar::ProgressBar()
{
    this->Title = "";
    this->IterMax = 100;
    this->IterCurr = 0;
    this->bar_max_width = BAR_MAX_WIDTH;
    this->bar_width = this->bar_max_width;
    this->pos = 0;
    this->progress = 0.0;
}

ProgressBar::ProgressBar(std::string title, const uint64_t& max_iter)
{
    this->bar_max_width = BAR_MAX_WIDTH;
    if (title.length() > 10) {
        this->Title = std::string(title, 0, 8);
    }

    this->Title = title;
    this->IterMax = max_iter;
    this->IterCurr = 0;
    auto title_len = this->Title.length() + 2;
    this->bar_width = this->bar_max_width - (int)title_len;
    this->pos = 0;
    this->progress = 0.0;
}

ProgressBar::~ProgressBar()
{
    // nothing to do...
}
