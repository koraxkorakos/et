module;
#include <cstddef>
#include <type_traits>
#include <utility>

export module ctv.cts_base;

export import ctv.concepts;

export namespace ctv {

/// A CTS terminal with one coefficient at a specified structural index.
template <std::size_t Index, class T = int>
  requires CTS_Field<T>
struct cts_one_expression
    : cts_value_mixin<cts_one_expression<Index, T>, index_set<Index>>,
      constant_expr<cts_one_expression<Index, T>> {
  using indices = index_set<Index>;
  using index_type = typename indices::value_type;
  using value_type = T;

private:
  template <index_type I>
  constexpr T get_impl() const
      noexcept(noexcept(coefficient_traits<T>::one())) {
    static_assert(I == Index);
    return coefficient_traits<T>::one();
  }

  friend cts_value_mixin<cts_one_expression, indices>;
};

/// Construct the unit basis blade at the bit-mask index Index.
template <std::size_t Index, class T = int>
[[nodiscard]] constexpr auto basis_blade() noexcept {
  return cts_one_expression<Index, T>{};
}

/// The CTS representation of a domain-independent value terminal.
template <class Arg>
struct cts_scalar_expression
    : cts_value_mixin<cts_scalar_expression<Arg>, index_set<0>>,
      unary_expr<cts_scalar_expression<Arg>, Arg> {
  using indices = index_set<0>;
  using index_type = typename indices::value_type;
  using value_type =
      std::remove_cv_t<typename std::remove_cvref_t<Arg>::value_type>;
  using base = unary_expr<cts_scalar_expression<Arg>, Arg>;
  using base::base;

private:
  template <index_type I, class Self>
  constexpr decltype(auto) get_impl(this Self &&self) noexcept {
    static_assert(I == 0);
    return std::forward<Self>(self).template argument<0>().get();
  }

  friend cts_value_mixin<cts_scalar_expression, indices>;
};

template <class T>
[[nodiscard]] constexpr auto lower_scalar(constant_expression<T> const &value) {
  return cts_scalar_expression<constant_expression<T>>{value};
}

template <class T>
[[nodiscard]] constexpr auto lower_scalar(reference_expression<T> const &value) {
  return cts_scalar_expression<reference_expression<T>>{value};
}

/// Sparse value projection/cast adapter.
///
/// TargetIndices is the advertised support. Projection may narrow support, or
/// widen it by materializing a source's structural zero as value_type{0}.
/// This is valid for values: populated does not imply nonzero. The adapter is
/// deliberately read-only, so it cannot weaken the CTS sink contract.
template <ValueSet TargetIndices, CTS_Value Arg>
struct cts_project
    : cts_value_mixin<cts_project<TargetIndices, Arg>, TargetIndices>,
      unary_expr<cts_project<TargetIndices, Arg>, Arg> {
  using expression_base = unary_expr<cts_project<TargetIndices, Arg>, Arg>;
  using indices = TargetIndices;
  using index_type = typename indices::value_type;
  using value_type = typename std::remove_cvref_t<Arg>::value_type;

  constexpr explicit cts_project(Arg arg) : expression_base(std::move(arg)) {}

private:
  template <index_type Index, class Self>
  constexpr value_type get_impl(this Self &&self) noexcept(noexcept(
      static_cast<value_type>(
          get<Index>(std::forward<Self>(self).template argument<0>())))) {
    return static_cast<value_type>(
        get<Index>(std::forward<Self>(self).template argument<0>()));
  }

  friend cts_value_mixin<cts_project, indices>;
};

template <ValueSet TargetIndices, CTS_Value Arg>
[[nodiscard]] constexpr auto project(Arg &&arg) {
  using A = std::remove_cvref_t<Arg>;
  return cts_project<TargetIndices, A>{std::forward<Arg>(arg)};
}

} // namespace ctv
