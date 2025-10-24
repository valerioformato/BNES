//
// Created by Valerio Formato on 24-Oct-25.
//

#ifndef BNES_TIME_H
#define BNES_TIME_H

#include "spdlog/sinks/stdout_color_sinks-inl.h"

#include <chrono>
#include <thread>

#ifdef WIN32
#include <windows.h>
#endif

namespace BNES::Chrono {
template <class Duration> inline void sleep_for(Duration duration) {
#ifdef WIN32
  static auto logger = spdlog::stdout_color_st("Chrono::sleep_for");

  // NOTE: Windows can't sleep reliably for less than ~1.9 ms
  // (see
  // https://stackoverflow.com/questions/5801813/c-usleep-is-obsolete-workarounds-for-windows-mingw/11470617#11470617)
  // So we need to work around this using performance counters and Sleep(0)
  static auto frequency = LARGE_INTEGER{.QuadPart = 0};

  // logger->trace("Sleeping for {0} ns", std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count());

  if (frequency.QuadPart == 0) {
    auto query_result = QueryPerformanceFrequency(&frequency);

    if (!query_result) {
      throw std::runtime_error("QueryPerformanceFrequency() failed");
    }
  }

  LARGE_INTEGER initial_counter_value{.QuadPart = 0};
  QueryPerformanceCounter(&initial_counter_value);

  LARGE_INTEGER current_counter_value = initial_counter_value;
  LARGE_INTEGER delta = {.QuadPart = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() *
                                     frequency.QuadPart / 1'000'000'000u};

  unsigned int num_iter{0};
  std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
  while (current_counter_value.QuadPart < initial_counter_value.QuadPart + delta.QuadPart) {
    Sleep(0);

    QueryPerformanceCounter(&current_counter_value);
    ++num_iter;
  }
  std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();

  // logger->trace("Initial counter value is {}", initial_counter_value.QuadPart);
  // logger->trace("Waited for {} delta (took {} iterations)", delta.QuadPart, num_iter);
  // logger->trace("Counter value is {} (now - initial = {})", current_counter_value.QuadPart,
  //               current_counter_value.QuadPart - initial_counter_value.QuadPart);
  // logger->trace("Took {} ns", std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

#else
  std::this_thread::sleep_for(duration);
#endif
}
} // namespace BNES::Chrono

#endif // BNES_TIME_H
