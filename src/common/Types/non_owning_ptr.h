//
// Created by vformato on 10/17/25.
// Based on GSL not_null pointer class (https://github.com/microsoft/GSL/blob/main/include/gsl/pointers)
//

#ifndef BNES_NON_OWNING_PTR_H
#define BNES_NON_OWNING_PTR_H

#include <type_traits>
#include <utility>

namespace BNES {
namespace details {
template <typename T, typename = void> struct is_comparable_to_nullptr : std::false_type {};

template <typename T>
struct is_comparable_to_nullptr<T,
                                std::enable_if_t<std::is_convertible_v<decltype(std::declval<T>() != nullptr), bool>>>
    : std::true_type {};

// Resolves to the more efficient of `const T` or `const T&`, in the context of returning a const-qualified value
// of type T.
//
// Copied from cppfront's implementation of the CppCoreGuidelines F.16
// (https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-in)
template <typename T>
using value_or_reference_return_t =
    std::conditional_t<sizeof(T) <= 2 * sizeof(void *) && std::is_trivially_copy_constructible_v<T>, const T,
                       const T &>;

} // namespace details

//
// not_null
//
// Restricts a pointer or smart pointer to only hold non-null values.
//
// Has zero size overhead over T.
//
// If T is a pointer (i.e. T == U*) then
// - allow construction from U*
// - disallow construction from nullptr_t
// - disallow default construction
// - ensure construction from null U* fails
// - allow implicit conversion to U*
//
template <class T> class non_owning_ptr {
public:
  static_assert(details::is_comparable_to_nullptr<T>::value, "T cannot be compared to nullptr.");

  using element_type = T;

  template <typename U>
  constexpr explicit non_owning_ptr(U &&u) noexcept(std::is_nothrow_move_constructible_v<T>) requires(
      std::is_convertible_v<U, T>)
      : ptr_(std::forward<U>(u)) {
    assert(ptr_ != nullptr);
  }

  constexpr explicit non_owning_ptr(T u) noexcept(std::is_nothrow_move_constructible_v<T>) requires(
      !std::is_same_v<std::nullptr_t, T>)
      : ptr_(std::move(u)) {
    assert(ptr_ != nullptr);
  }

  template <typename U>
  constexpr explicit non_owning_ptr(const non_owning_ptr<U> &other) noexcept(
      std::is_nothrow_move_constructible_v<T>) requires(std::is_convertible_v<U, T>)
      : non_owning_ptr(other.get()) {}

  non_owning_ptr(const non_owning_ptr &other) = default;
  non_owning_ptr &operator=(const non_owning_ptr &other) = default;
  constexpr details::value_or_reference_return_t<T> get() const
      noexcept(noexcept(details::value_or_reference_return_t<T>(std::declval<T &>()))) {
    return ptr_;
  }

  constexpr explicit operator T() const { return get(); }
  constexpr decltype(auto) operator->() const { return get(); }
  constexpr decltype(auto) operator*() const { return *get(); }

  // prevents compilation when someone attempts to assign a null pointer constant
  non_owning_ptr(std::nullptr_t) = delete;
  non_owning_ptr &operator=(std::nullptr_t) = delete;

  // unwanted operators... pointers only point to single objects!
  non_owning_ptr &operator++() = delete;
  non_owning_ptr &operator--() = delete;
  non_owning_ptr operator++(int) = delete;
  non_owning_ptr operator--(int) = delete;
  non_owning_ptr &operator+=(std::ptrdiff_t) = delete;
  non_owning_ptr &operator-=(std::ptrdiff_t) = delete;
  void operator[](std::ptrdiff_t) const = delete;

  void swap(non_owning_ptr<T> &other) noexcept { std::swap(ptr_, other.ptr_); }

private:
  T ptr_;
};

template <typename T>
void swap(non_owning_ptr<T> &a, non_owning_ptr<T> &b) noexcept
    requires(std::is_move_assignable_v<T> &&std::is_move_constructible_v<T>) {
  a.swap(b);
}

template <class T> auto make_not_null(T &&t) noexcept {
  return non_owning_ptr<std::remove_cv_t<std::remove_reference_t<T>>>{std::forward<T>(t)};
}

template <class T> std::ostream &operator<<(std::ostream &os, const non_owning_ptr<T> &val) {
  os << val.get();
  return os;
}

template <class T, class U>
constexpr auto operator==(const non_owning_ptr<T> &lhs,
                          const non_owning_ptr<U> &rhs) noexcept(noexcept(lhs.get() == rhs.get()))
    -> decltype(lhs.get() == rhs.get()) {
  return lhs.get() == rhs.get();
}

template <class T, class U>
constexpr auto operator!=(const non_owning_ptr<T> &lhs,
                          const non_owning_ptr<U> &rhs) noexcept(noexcept(lhs.get() != rhs.get()))
    -> decltype(lhs.get() != rhs.get()) {
  return lhs.get() != rhs.get();
}

template <class T, class U>
constexpr auto operator<(const non_owning_ptr<T> &lhs,
                         const non_owning_ptr<U> &rhs) noexcept(noexcept(std::less<>{}(lhs.get(), rhs.get())))
    -> decltype(std::less<>{}(lhs.get(), rhs.get())) {
  return std::less<>{}(lhs.get(), rhs.get());
}

template <class T, class U>
constexpr auto operator<=(const non_owning_ptr<T> &lhs,
                          const non_owning_ptr<U> &rhs) noexcept(noexcept(std::less_equal<>{}(lhs.get(), rhs.get())))
    -> decltype(std::less_equal<>{}(lhs.get(), rhs.get())) {
  return std::less_equal<>{}(lhs.get(), rhs.get());
}

template <class T, class U>
constexpr auto operator>(const non_owning_ptr<T> &lhs,
                         const non_owning_ptr<U> &rhs) noexcept(noexcept(std::greater<>{}(lhs.get(), rhs.get())))
    -> decltype(std::greater<>{}(lhs.get(), rhs.get())) {
  return std::greater<>{}(lhs.get(), rhs.get());
}

template <class T, class U>
constexpr auto operator>=(const non_owning_ptr<T> &lhs,
                          const non_owning_ptr<U> &rhs) noexcept(noexcept(std::greater_equal<>{}(lhs.get(), rhs.get())))
    -> decltype(std::greater_equal<>{}(lhs.get(), rhs.get())) {
  return std::greater_equal<>{}(lhs.get(), rhs.get());
}

// more unwanted operators
template <class T, class U> std::ptrdiff_t operator-(const non_owning_ptr<T> &, const non_owning_ptr<U> &) = delete;
template <class T> non_owning_ptr<T> operator-(const non_owning_ptr<T> &, std::ptrdiff_t) = delete;
template <class T> non_owning_ptr<T> operator+(const non_owning_ptr<T> &, std::ptrdiff_t) = delete;
template <class T> non_owning_ptr<T> operator+(std::ptrdiff_t, const non_owning_ptr<T> &) = delete;

template <class T, class U = decltype(std::declval<const T &>().get()),
          bool = std::is_default_constructible_v<std::hash<U>>>
struct non_owning_ptr_hash {
  std::size_t operator()(const T &value) const { return std::hash<U>{}(value.get()); }
};

} // namespace BNES

#endif // NON_OWNING_PTR_H
