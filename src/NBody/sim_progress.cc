/**
 *
 * sim_progress.cc 
 *
 * Progress monitoring part for N-Body simulation --> 
 * Implementations.
 *
 * Written by Taylor Shin
 * Sep. 7th, 2016
 *
**/

#include "sim_progress.h"

// Prints out elapsed time with a few decoration.
std::string SimProgress::print_elapsed_time()
{
    std::stringstream ss;
    if (fp_mteq<fp_t>(this->elapsed_time, 1.0)) {
        ss << (this->elapsed_time);
        return ss.str() + " s";
    }
    else if (fp_mteq<fp_t>(this->elapsed_time, 1e-3)) {
        ss << (this->elapsed_time * 1e3);
        return ss.str() + " ms";
    }
    else if (fp_mteq<fp_t>(this->elapsed_time, 1e-6)) {
        ss << (this->elapsed_time * 1e6);
        return ss.str() + " us";
    }
    else if (fp_mteq<fp_t>(this->elapsed_time, 1e-9)) {
        ss << (this->elapsed_time * 1e9);
        return ss.str() + " ns";
    }
    else if (fp_mteq<fp_t>(this->elapsed_time, 0.0)) {
        ss << (this->elapsed_time * 1e12);
        return ss.str() + " ps";
    }
    else {
        ss << (this->elapsed_time);
        return ss.str() + " s";
    }
    return std::string(""); // Dummy return.
}

// Prints simulation time
std::string SimProgress::format_time(std::clock_t elapsed_sim_time)
{
    if (!elapsed_sim_time) return std::string("0 second");

    auto sim_time_in_seconds = 
        static_cast<int>(elapsed_sim_time/(double)CLOCKS_PER_SEC);
    auto years = sim_time_in_seconds / (3600*24*365);
    auto years_rem = sim_time_in_seconds - years*(3600*24*365);
    auto months = years_rem / (3600*24*30);
    auto months_rem = years_rem - months*(3600*24*30);
    auto weeks = months_rem / (3600*24*7);
    auto weeks_rem = months_rem - weeks*(3600*24*7);
    auto days = weeks_rem / (3600*24);
    auto days_rem = weeks_rem - days*(3600*24);
    auto hours = days_rem / 3600;
    auto hours_rem = days_rem - hours*3600;
    auto minutes = hours_rem / 60;
    auto seconds = hours_rem - minutes*60;

    std::stringstream ss;
    if (years)
        ss << years << ((years > 1)?" years ":" year ");
    if (months)
        ss << months << ((months > 1)?" months ":" month ");
    if (weeks)
        ss << weeks << ((weeks > 1)?" weeks ":" week ");
    if (days)
        ss << days << ((days > 1)?" days ":" day ");
    if (hours)
        ss << hours << ((hours > 1)?" hours ":" hour ");
    if (minutes)
        ss << minutes << ((minutes > 1)?" minutes ":" minute ");
    if (seconds)
        ss << seconds << ((seconds > 1)?" seconds":" second");

    return ss.str();
}