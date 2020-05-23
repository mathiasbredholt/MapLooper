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
#include <climits>
#include <cstdio>

#include "esp_log.h"

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
      ESP_LOGW("queue", "Queue full. Ignoring push.");
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
