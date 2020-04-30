/*
           __  
  /\/\    / /  
 /    \  / /   
/ /\/\ \/ /___ 
\/    \/\____/ 
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

ParameterLayout.cpp
Parameter settings

*/

#include "MapLooper/ParameterLayout.hpp"

namespace MapLooper {

const std::array<int, 16> DIVISION_PRESETS = {
    {384, 192, 96, 48, 24, 12, 6, 6, 128, 128, 128, 64, 32, 16, 8, 8}};

const std::array<int, 16> REPEATS_TIME_PRESETS = {
    {96, 96, 96, 48, 24, 12, 6, 6, 128, 128, 128, 64, 32, 16, 8, 8}};

const std::array<int, 16> PTN_LENGTH_PRESETS = {{384, 768, 1152, 1536, 1920,
                                                 2304, 2688, 3072, 384, 192, 96,
                                                 48, 24, 12, 6, 6144}};
}  // namespace MapLooper
