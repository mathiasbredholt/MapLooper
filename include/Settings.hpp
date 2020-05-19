/*
           __  
  /\/\    / /  
 /    \  / /   
/ /\/\ \/ /___ 
\/    \/\____/ 
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

Settings.hpp
Settings class

*/

#pragma once

namespace MapLooper {
class Settings {
 public:
  static void load();
  static void save();
};
}  // namespace MapLooper
