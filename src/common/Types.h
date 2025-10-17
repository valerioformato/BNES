//
// Created by vformato on 10/17/25.
//

#ifndef BNES_TYPES_H
#define BNES_TYPES_H

#include <chrono>
#include <expected>
#include <string_view>
#include <type_traits>
#include <utility>

namespace BNES {
struct Error {
  Error() = default;
  Error(std::error_code code, std::string_view msg) : m_code(code), m_msg(msg) {}
  Error(std::errc code, std::string_view msg) : m_code(std::make_error_code(code)), m_msg(msg) {}

  [[nodiscard]] std::string_view Message() const { return m_msg; }
  [[nodiscard]] std::error_code Code() const { return m_code; }

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
} // namespace BNES::Utils
#endif // TYPES_H
