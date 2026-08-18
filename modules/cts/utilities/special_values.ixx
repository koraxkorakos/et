module;
#include <concepts>
#include <type_traits>

export module ctv.special_values;

export namespace ctv {
/// Customization point for the additive and multiplicative identities of a
/// coefficient field. Specialize this trait for types whose identities cannot
/// be formed as T{} and T{1}.
template <class T> struct coefficient_traits {
  [[nodiscard]] static constexpr T zero() noexcept(noexcept(T{}))
    requires requires { T{}; }
  {
    return T{};
  }

  [[nodiscard]] static constexpr T one() noexcept(noexcept(T{1}))
    requires requires { T{1}; }
  {
    return T{1};
  }
};

template <class T>
concept CTS_Field =
    requires {
      { coefficient_traits<std::remove_cv_t<T>>::zero() }
          -> std::same_as<std::remove_cv_t<T>>;
      { coefficient_traits<std::remove_cv_t<T>>::one() }
          -> std::same_as<std::remove_cv_t<T>>;
    };

struct zero_type {
  template <typename T>
    requires CTS_Field<T>
  constexpr operator T() const
      noexcept(noexcept(coefficient_traits<T>::zero())) {
    return coefficient_traits<T>::zero();
  }
};

inline constexpr zero_type zero{};

struct one_type {
  template <typename T>
    requires CTS_Field<T>
  constexpr operator T() const noexcept(noexcept(coefficient_traits<T>::one())) {
    return coefficient_traits<T>::one();
  }
};

inline constexpr one_type one{};

struct minus_one_type {
  template <typename T>
    requires CTS_Field<T> && requires { -coefficient_traits<T>::one(); }
  constexpr operator T() const noexcept(noexcept(-coefficient_traits<T>::one())) {
    return -coefficient_traits<T>::one();
  }
};

inline constexpr minus_one_type minus_one{};
} // namespace ctv
