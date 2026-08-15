module;

export module ctv.special_values;

export namespace ctv {
struct zero_type {
  template <typename T>
    requires requires { T{}; }
  constexpr operator T() const noexcept(noexcept(T{})) {
    return T{};
  }
};

inline constexpr zero_type zero{};

struct one_type {
  template <typename T>
    requires requires { T{1}; }
  constexpr operator T() const noexcept(noexcept(T{1})) {
    return T{1};
  }
};

inline constexpr one_type one{};

struct minus_one_type {
  template <typename T>
    requires requires { -T{1}; }
  constexpr operator T() const noexcept(noexcept(-T{1})) {
    return -T{1};
  }
};

inline constexpr minus_one_type minus_one{};
} // namespace ctv
