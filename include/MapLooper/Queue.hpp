/*
           __  
  /\/\    / /  
 /    \  / /   
/ /\/\ \/ /___ 
\/    \/\____/ 
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

Queue.hpp
A queue implementation

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
