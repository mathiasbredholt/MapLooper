/*
 MapLooper - Embedded Live-Looping Tools for Digital Musical Instruments
 Copyright (C) 2020 Mathias Bredholt
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#pragma once

#include <array>

namespace MapLooper {

template <class data, int MAXLEN>
class queue_t {
 public:
  int push(data d) {
    int next;
    // next is where head will point to after this write.
    next = m_front + 1;
    if (next >= MAXLEN) {
      next = 0;
    }
    // if the head + 1 == tail, circular buffer is full
    if (next == m_rear) {
      return -1;
    }
    m_data[m_front] = d;  // Load data and then move
    m_front = next;
    ++m_size;
    return 0;
  }

  int pop() {
    int next;
    // if the head == tail, we don't have any data
    if (m_front == m_rear) {
      return -1;
    }
    // next is where tail will point to after this read.
    next = m_rear + 1;
    if (next >= MAXLEN) {
      next = 0;
    }
    m_rear = next;  // tail to next offset.
    --m_size;
    return 0;  // return success to indicate successful pop.
  }

  inline data front() { return m_data[m_rear]; }

  inline int size() { return m_size; }

  void clear() {
    m_front = 0;
    m_rear = 0;
    m_size = 0;
  }

 private:
  int m_front{0};
  int m_rear{0};
  int m_size{0};
  std::array<data, MAXLEN> m_data;
};

}  // namespace MapLooper
