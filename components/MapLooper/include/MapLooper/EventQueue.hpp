/*
           __  
  /\/\    / /  
 /    \  / /   
/ /\/\ \/ /___ 
\/    \/\____/ 
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

EventQueue.hpp
A priority queue implementation

*/

#include <array>
#include <climits>
#include <cstdio>

#pragma once

namespace MapLooper {

struct off_event_t {
  int32_t time_stamp = INT_MAX;
  uint8_t pitch = 0;
  uint8_t channel = 0;
};

template <class Event, int N>
class event_queue_t {
 public:
  void push(Event e) {
    if (m_size < N) {
      int mom = m_size++;
      int me = mom;
      for (; mom > 0;) { /* percolate up heap */
        mom = (mom - 1) >> 1;
        if (e.time_stamp < m_events[mom].time_stamp) {
          m_events[me] = m_events[mom];
          me = mom;
        } else {
          break;
        }
      }
      m_events[me] = e;
    } else {
      printf("NOTE OFF QUEUE FULL\n");
    }
  }

  void pop() {
    if (--m_size > 0) {
      Event temp = m_events[m_size];
      int mom = 0;
      int me = 1;
      for (; me < m_size;) { /* demote heap */
        if (me + 1 < m_size &&
            m_events[me].time_stamp > m_events[me + 1].time_stamp) {
          me++;
        }
        if (temp.time_stamp > m_events[me].time_stamp) {
          m_events[mom] = m_events[me];
          mom = me;
          me = (me << 1) + 1;
        } else {
          break;
        }
      }
      m_events[mom] = temp;
    }
  }

  Event top() { return m_events[0]; }

  void clear() {
    m_events.fill(0);
    for (int i = 0; i < N; ++i) {
      m_events[i].time_stamp = INT_MAX;
    }
    m_size = 0;
  }

  int size() { return m_size; }

 private:
  std::array<Event, N> m_events;
  int m_size{0};
};

}  // namespace MapLooper
