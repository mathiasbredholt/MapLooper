/*

          ___  
         (   ) 
  .--.    | |  
 /    \   | |  
;  ,-. '  | |  
| |  | |  | |  
| |  | |  | |  
| |  | |  | |  
| '  | |  | |  
'  `-' |  | |  
 `.__. | (___) 
 ( `-' ;       
  `.__.        

Gesture Looper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

Parameter.hpp
A general parameter class with max and min and
change detection.

*/

#pragma once

#include <algorithm>
#include <functional>

namespace gl {

template <typename T>
class Parameter {
 public:
  Parameter() {}

  inline bool set_relative(int val, T min_val, T max_val, int step_size) {
    val = std::min(std::max(val, -step_size), step_size);
    T new_val = std::min<int>(std::max<int>(m_value + val, min_val), max_val);
    bool is_changed = m_value != new_val;
    m_value = new_val;
    return is_changed;
  }

  inline T get() const { return m_value; }

  inline void set(T val) { m_value = val; }

 private:
  T m_value{0};
};

}  // namespace gl
