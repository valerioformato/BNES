//
// Ranges compatibility layer
// Provides a unified interface for ranges operations that works with both
// C++23 std::ranges and range-v3 library for older compilers
//

#ifndef BNES_RANGES_COMPAT_H
#define BNES_RANGES_COMPAT_H

#include <version>

namespace BNES {

// Check if C++23 ranges features are available
#if defined(__cpp_lib_ranges_chunk) && defined(__cpp_lib_ranges_enumerate)
// Use standard library ranges
#define BNES_USE_STD_RANGES
#include <ranges>
namespace rg = std::ranges;
namespace rv = std::ranges::views;
#else
// Fall back to range-v3
#define BNES_USE_RANGES_V3

// MSVC compatibility: include these before range-v3 to avoid conflicts
#if defined(_MSC_VER)
  #include <algorithm>
  #include <iterator>
  #include <utility>
  #pragma warning(push)
  #pragma warning(disable : 4244) // Disable conversion warnings in range-v3
  #pragma warning(disable : 4267)
#endif

#include <range/v3/all.hpp>

#if defined(_MSC_VER)
  #pragma warning(pop)
#endif

namespace rg = ranges;
namespace rv = ranges::views;
#endif

} // namespace BNES

#endif // BNES_RANGES_COMPAT_H
