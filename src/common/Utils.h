//
// Created by Valerio Formato on 15/12/21.
//

#ifndef BNES_UTILS_H
#define BNES_UTILS_H

#include <fmt/format.h>

#include <algorithm>
#include <charconv>
#include <chrono>
#include <expected>
#include <string_view>
#include <type_traits>
#include <utility>

namespace std {
#ifndef __cpp_lib_remove_cvref
template <class T> struct remove_cvref { using type = std::remove_cv_t<std::remove_reference_t<T>>; };
template <class T> using remove_cvref_t = typename remove_cvref<T>::type;
#endif

#ifndef __cpp_lib_to_underlying
template <class Enum> constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept {
  return static_cast<std::underlying_type_t<Enum>>(e);
}
#endif

#ifndef __cpp_lib_clamp
template <class T, class Compare> constexpr const T &clamp(const T &v, const T &lo, const T &hi, Compare comp) {
  return comp(v, lo) ? lo : comp(hi, v) ? hi : v;
}

template <class T> constexpr const T &clamp(const T &v, const T &lo, const T &hi) { return clamp(v, lo, hi, less{}); }
#endif
} // namespace std

// adapted from https://github.com/SerenityOS/serenity/blob/master/AK/Try.h
// FIXME: At some point we should use the AK implementation or at least make this one more robust
#define TRY(expression)                                                                                                \
  ({                                                                                                                   \
    auto &&_temporary_result = (expression);                                                                           \
    static_assert(!std::is_lvalue_reference_v<std::remove_cvref_t<decltype(_temporary_result)>::value_type>,           \
                  "Do not return a reference from a fallible expression");                                             \
    if (_temporary_result.has_error()) [[unlikely]]                                                                    \
      return _temporary_result.error();                                                                                \
    _temporary_result.value();                                                                                         \
  })

#define TRY_REPEATED(expression, max_tries)                                                                            \
  ({                                                                                                                   \
    std::remove_cvref_t<decltype(max_tries)> tries{0};                                                                 \
    auto &&_temporary_result = (expression);                                                                           \
    static_assert(!std::is_lvalue_reference_v<std::remove_cvref_t<decltype(_temporary_result)>::value_type>,           \
                  "Do not return a reference from a fallible expression");                                             \
    while (_temporary_result.has_error() && tries < max_tries) {                                                       \
      _temporary_result = (expression);                                                                                \
      ++tries;                                                                                                         \
    }                                                                                                                  \
    if (_temporary_result.has_error()) [[unlikely]]                                                                    \
      return _temporary_result;                                                                                        \
    _temporary_result.value();                                                                                         \
  })

namespace BNES {
struct Error {
  Error() = default;
  Error(std::error_code code, std::string_view msg) : m_code(code), m_msg(msg) {}
  Error(std::errc code, std::string_view msg) : m_code(std::make_error_code(code)), m_msg(msg) {}

  const std::string_view Message() const { return m_msg; }
  std::error_code Code() const { return m_code; }

private:
  std::error_code m_code;
  std::string m_msg;
};
static_assert(std::is_default_constructible_v<Error>, "BNES::Error must be default constructible");

template <typename... Args> inline std::unexpected<Error> make_error(Args... args) {
  return std::unexpected<Error>(std::in_place_t(), std::forward<Args>(args)...);
}
} // namespace BNES

namespace BNES {
template <typename T> using ErrorOr = std::expected<T, Error>;
} // namespace BNES

namespace BNES::Utils {
template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

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

inline std::vector<std::string> TokenizeString(const std::string str, const char delimiter) {
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

} // namespace BNES::Utils

#endif // BNES_UTILS_H
