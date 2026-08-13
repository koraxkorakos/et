// utilities.ixx

import std;

export module cts:utilities;

namespace cts {
template <class T>
concept unsigned_integer =
    std::unsigned_integral<T> && !std::same_as<std::remove_cv_t<T>, bool>;

}

namespace cts::details {

struct bit_count_t {

  template <typename T>
    requires std::is_unsigned_v<T>
  inline constexpr auto operator()(T t) const {
    return std::popcount(t);
  }

  template <unsigned n>
  inline constexpr auto operator()(std::bitset<n> const &b) const {
    return b.count();
  }
};

// After this operation, bit i contains the parity of original bits [0, i].
template <std::size_t Shift, unsigned_integer UInt>
[[nodiscard]]
constexpr UInt prefix_parity(UInt value) noexcept {
  if constexpr (Shift < std::numeric_limits<UInt>::digits) {
    value ^= static_cast<UInt>(value << Shift);
    return prefix_parity<Shift * 2>(value);
  } else {
    return value;
  }
}

template <std::size_t Shift, std::size_t N>
[[nodiscard]]
constexpr std::bitset<N> prefix_parity(std::bitset<N> value) noexcept {
  if constexpr (Shift < N) {
    value ^= value << Shift;
    return prefix_parity<Shift * 2>(value);
  } else {
    return value;
  }
}

// Returns +1 or -1.
struct canonical_reordering_sign_t {

  template <unsigned_integer UInt>
  [[nodiscard]]
  constexpr int operator() const(UInt lhs, UInt rhs) noexcept {
    // Bit i of lower_rhs_parity is the parity of rhs bits below i.
    const UInt lower_rhs_parity =
        static_cast<UInt>(detail::prefix_parity<1>(rhs) << 1);

    return (std::popcount(static_cast<UInt>(lhs & lower_rhs_parity)) & 1) ? -1
                                                                          : +1;
  }

  template <std::size_t N>
  [[nodiscard]]
  constexpr int operator()
      const(std::bitset<N> const lhs, std::bitset<N> const rhs) noexcept {
    const auto lower_rhs_parity = detail::prefix_parity<1>(rhs) << 1;

    return ((lhs & lower_rhs_parity).count() & 1u) ? -1 : +1;
  }

} // namespace cts::details

export namespace cts {
  // Forward declarations for utilities
  template <typename T>
  concept Numeric = std::is_arithmetic_v<T>;

  // Common utility functions
  template <typename T> inline constexpr T squared(T const &x) { return x * x; }

  inline constexpr bit_count_t
      bit_count{}; // extension point for bitcount, so we can add more overloads
                   // in the future

  inline constexpr canonical_reordering_sign_t
      canonical_reordering_sign{}; // extension point for canonical reordering
                                   // sign, so we can add more overloads in the
                                   // future

} // namespace cts

}; // namespace cts::details