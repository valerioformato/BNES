//
// Created by Valerio Formato on 15/12/21.
//

#ifndef BNES_UTILS_H
#define BNES_UTILS_H

#include "common/Types.h"

#include "spdlog/spdlog.h"

#include <chrono>
#include <source_location>
#include <string_view>

#define TODO(message)                                                                                                  \
  {                                                                                                                    \
    const auto loc = std::source_location::current();                                                                  \
    spdlog::error("TODO reached in {}:{} ({}): {}", loc.file_name(), loc.line(), loc.function_name(), message);        \
    assert(false);                                                                                                     \
  };

// adapted from https://github.com/SerenityOS/serenity/blob/master/AK/Try.h
// FIXME: At some point we should use the AK implementation or at least make this one more robust
#define TRY(expression)                                                                                                \
  ({                                                                                                                   \
    auto &&_temporary_result = (expression);                                                                           \
    static_assert(!std::is_lvalue_reference_v<std::remove_cvref_t<decltype(_temporary_result)>::value_type>,           \
                  "Do not return a reference from a fallible expression");                                             \
    if (!_temporary_result.has_value()) [[unlikely]]                                                                   \
      return std::unexpected(_temporary_result.error());                                                               \
    _temporary_result.value();                                                                                         \
  })

#define TRY_REPEATED(expression, max_tries)                                                                            \
  ({                                                                                                                   \
    std::remove_cvref_t<decltype(max_tries)> tries{0};                                                                 \
    auto &&_temporary_result = (expression);                                                                           \
    static_assert(!std::is_lvalue_reference_v<std::remove_cvref_t<decltype(_temporary_result)>::value_type>,           \
                  "Do not return a reference from a fallible expression");                                             \
    while (!_temporary_result.has_value() && tries < max_tries) {                                                      \
      _temporary_result = (expression);                                                                                \
      ++tries;                                                                                                         \
    }                                                                                                                  \
    if (!_temporary_result.has_value()) [[unlikely]]                                                                   \
      return _temporary_result;                                                                                        \
    _temporary_result.value();                                                                                         \
  })

namespace BNES::Utils {
namespace String {
inline std::vector<std::string_view> TokenizeString(const std::string_view str, const char delimiter) {
  std::vector<std::string_view> tokens;
  size_t start = 0;
  size_t end = str.find(delimiter);

  while (end != std::string_view::npos) {
    tokens.emplace_back(str.substr(start, end - start));
    start = end + 1;
    end = str.find(delimiter, start);
  }

  tokens.emplace_back(str.substr(start));
  return tokens;
}

inline std::vector<std::string> TokenizeString(const std::string &str, const char delimiter) {
  std::vector<std::string> tokens;
  size_t start = 0;
  size_t end = str.find(delimiter);

  while (end != std::string::npos) {
    tokens.emplace_back(str.substr(start, end - start));
    start = end + 1;
    end = str.find(delimiter, start);
  }

  tokens.emplace_back(str.substr(start));
  return tokens;
}
} // namespace String

} // namespace BNES::Utils

#endif // BNES_UTILS_H
