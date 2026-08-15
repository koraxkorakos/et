module;
#include <type_traits>
#include <utility>

export module ctv.cts_base;

export import ctv.concepts;

export namespace ctv {

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
