module;

#include <concepts>
#include <type_traits>
#include <utility>

export module ctv.concepts;

export import ctv.expression_template;
export import ctv.value_set;
export import ctv.special_values;

namespace ctv::details {

template <class E, class I, I... ns>
consteval bool cts_sink_getters_valid(value_set<I, ns...>) {
  using T = std::remove_cvref_t<E>;
  using V = typename T::value_type;

  // Test precisely the required operation. This deliberately accepts
  // assignable proxy objects as well as actual V& results.
  return (requires(T &sink, V value) { get<ns>(sink) = value; } && ...);
}

} // namespace ctv::details

export namespace ctv {

/// Common metadata base for compile-time sparse objects.
template <ValueSet value_set> struct cts_base {
  using indices = value_set;
  using index_type = typename value_set::value_type;

protected:
  constexpr cts_base() = default;
  constexpr cts_base(cts_base const &) = default;
  constexpr cts_base(cts_base &&) = default;
  constexpr cts_base &operator=(cts_base const &) = default;
  constexpr cts_base &operator=(cts_base &&) = default;
  ~cts_base() = default;
};

/// Metadata base for readable compile-time sparse values.
///
/// It supplies zero for every index. More-specific hidden-friend overloads in
/// cts_value_mixin and cts_variable_mixin replace this fallback at populated
/// indices.
template <ValueSet value_set> struct cts_value_base : cts_base<value_set> {
  using index_type = typename cts_base<value_set>::index_type;

  template <index_type n>
  friend constexpr zero_type const get(cts_value_base const &) noexcept {
    return {};
  }

  template <index_type n>
  friend constexpr zero_type const get(cts_value_base &&) noexcept {
    return {};
  }

protected:
  constexpr cts_value_base() = default;
  constexpr cts_value_base(cts_value_base const &) = default;
  constexpr cts_value_base(cts_value_base &&) = default;
  constexpr cts_value_base &operator=(cts_value_base const &) = default;
  constexpr cts_value_base &operator=(cts_value_base &&) = default;
  ~cts_value_base() = default;
};

/// An expression carrying compile-time sparse-object metadata.
template <class E>
concept CTS_Object =
    Expression<E> &&
    requires {
      typename std::remove_cvref_t<E>::indices;
      typename std::remove_cvref_t<E>::index_type;
      typename std::remove_cvref_t<E>::value_type;
    } &&
    std::derived_from<std::remove_cvref_t<E>,
                      cts_base<typename std::remove_cvref_t<E>::indices>>;

/// A compile-time sparse expression readable at every index.
///
/// Derivation from cts_value_base certifies the readable interface. The base
/// returns zero at unpopulated indices; the populated getters are supplied by
/// the implementation.
template <class E>
concept CTS_Value =
    CTS_Object<E> &&
    std::derived_from<std::remove_cvref_t<E>,
                      cts_value_base<typename std::remove_cvref_t<E>::indices>>;

/// A compile-time sparse expression writable at all populated indices.
///
/// No getter is required at an unpopulated index. Consequently, attempting to
/// write such an index is ill-formed.
template <class E>
concept CTS_Sink =
    CTS_Object<E> && details::cts_sink_getters_valid<std::remove_cvref_t<E>>(
                         typename std::remove_cvref_t<E>::indices{});

/// A compile-time sparse expression that is both readable and writable.
template <class E>
concept CTS_Variable = CTS_Value<E> && CTS_Sink<E>;

/// CRTP mixin for a readable compile-time sparse value.
///
/// Derived supplies a get_impl<n>() with an explicit object parameter, for
/// example:
///
/// template <index_type n, class Self>
///   requires (is_element_of<value_set>(n))
/// constexpr decltype(auto) get_impl(this Self&& self)
/// {
///   return std::forward_like<Self>(self.values[position<n>]);
/// }
///
/// If get_impl is private, Derived must contain:
///
/// friend cts_value_mixin<Derived, value_set>;
template <class Derived, ValueSet value_set>
struct cts_value_mixin : cts_value_base<value_set> {
private:
  using index_type = typename value_set::value_type;

  template <index_type n>
    requires(is_element_of<value_set>(n))
  friend constexpr decltype(auto)
  get(Derived const &value) noexcept(noexcept(value.template get_impl<n>())) {
    return value.template get_impl<n>();
  }

  template <index_type n>
    requires(is_element_of<value_set>(n))
  friend constexpr decltype(auto) get(Derived &&value) noexcept(
      noexcept(std::move(value).template get_impl<n>())) {
    return std::move(value).template get_impl<n>();
  }
};

/// CRTP mixin for a write-only compile-time sparse sink.
///
/// Only populated indices get an overload. Thus get<n>(sink) is ill-formed
/// when n is absent from value_set.
///
/// If get_impl is private, Derived must contain:
///
/// friend cts_sink_mixin<Derived, value_set>;
template <class Derived, ValueSet value_set>
struct cts_sink_mixin : cts_base<value_set> {
private:
  using index_type = typename value_set::value_type;

  template <index_type n>
    requires(is_element_of<value_set>(n))
  friend constexpr decltype(auto)
  get(Derived &sink) noexcept(noexcept(sink.template get_impl<n>())) {
    return sink.template get_impl<n>();
  }
};

/// CRTP mixin for a readable and writable compile-time sparse variable.
///
/// Populated indices dispatch to get_impl<n>(). Reads at unpopulated indices
/// use cts_value_base's zero fallback. Writes at unpopulated indices are
/// ill-formed because that fallback returns a const zero_type value.
///
/// If get_impl is private, Derived must contain:
///
/// friend cts_variable_mixin<Derived, value_set>;
template <class Derived, ValueSet value_set>
struct cts_variable_mixin : cts_value_base<value_set> {
private:
  using index_type = typename value_set::value_type;

  template <index_type n>
    requires(is_element_of<value_set>(n))
  friend constexpr decltype(auto)
  get(Derived &value) noexcept(noexcept(value.template get_impl<n>())) {
    return value.template get_impl<n>();
  }

  template <index_type n>
    requires(is_element_of<value_set>(n))
  friend constexpr decltype(auto)
  get(Derived const &value) noexcept(noexcept(value.template get_impl<n>())) {
    return value.template get_impl<n>();
  }

  template <index_type n>
    requires(is_element_of<value_set>(n))
  friend constexpr decltype(auto) get(Derived &&value) noexcept(
      noexcept(std::move(value).template get_impl<n>())) {
    return std::move(value).template get_impl<n>();
  }
};

} // namespace ctv
