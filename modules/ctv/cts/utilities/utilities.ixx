// utilities.ixx
export module cts:utilities;

export namespace cts {
// Forward declarations for utilities
template <typename T>
concept Numeric = std::is_arithmetic_v<T>;

// Common utility functions
template <typename T> constexpr T squared(T x) { return x * x; }
} // namespace cts