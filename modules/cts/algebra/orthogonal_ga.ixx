module;
#include <bit>
#include <concepts>
#include <type_traits>
#include <utility>
export module ctv.orthogonal_ga;
export import ctv.grassmann;

namespace ctv::detail {
template <class L, class R> struct product_indices;
template <class T, T... R> struct product_indices<value_set<T>, value_set<T, R...>> { using type = value_set<T>; };
template <class T, T L, T... LS, T... R>
struct product_indices<value_set<T, L, LS...>, value_set<T, R...>> {
  using head = normalize_t<value_list<T, static_cast<T>(L ^ R)...>>;
  using type = set_union_t<head, typename product_indices<value_set<T, LS...>, value_set<T, R...>>::type>;
};
template <std::unsigned_integral T> constexpr int geometric_sign(T lhs, T rhs) {
  int sign = 1;
  while (lhs) {
    auto bit = std::countr_zero(lhs);
    auto lower = bit == 0 ? T{} : static_cast<T>((T{1} << bit) - 1);
    if (std::popcount(static_cast<T>(rhs & lower)) & 1) sign = -sign;
    lhs &= static_cast<T>(lhs - 1);
  }
  return sign;
}
template <class Metric, std::unsigned_integral T>
constexpr auto metric_factor(T overlap) {
  using result = std::remove_cvref_t<decltype(Metric{}(T{1}))>;
  result factor{1};
  while (overlap) {
    auto bit = std::countr_zero(overlap);
    factor *= Metric{}(static_cast<T>(T{1} << bit));
    overlap &= static_cast<T>(overlap - 1);
  }
  return factor;
}

template <class Metric, bool InnerOnly, auto I, auto A, class L, class R,
          class T, T... RI>
constexpr auto metric_coefficient_row(L const &l, R const &r,
                                      value_set<T, RI...>) {
  using result = std::common_type_t<typename L::value_type, typename R::value_type>;
  result sum{};
  ([&] {
    if constexpr ((A ^ RI) == I && (!InnerOnly || (A & RI) != 0))
      sum += static_cast<result>(
                 geometric_sign(A, RI) *
                 metric_factor<Metric>(static_cast<T>(A & RI))) *
             static_cast<result>(get<A>(l)) *
             static_cast<result>(get<RI>(r));
  }(), ...);
  return sum;
}

template <class Metric, bool InnerOnly, auto I, class L, class R, class T,
          T... LI, T... RI>
constexpr auto metric_coefficient(L const &l, R const &r,
                                  value_set<T, LI...>,
                                  value_set<T, RI...>) {
  using result =
      std::common_type_t<typename L::value_type, typename R::value_type>;
  result sum{};
  ((sum += metric_coefficient_row<Metric, InnerOnly, I, LI>(
        l, r, value_set<T, RI...>{})), ...);
  return sum;
}
} // namespace ctv::detail

export namespace ctv {
/// Metric maps a one-bit basis-blade index to its diagonal metric value.
template <class Metric, class L, class R, bool InnerOnly>
struct cts_metric_expr
    : cts_value_mixin<cts_metric_expr<Metric, L, R, InnerOnly>,
                      typename detail::product_indices<expression_indices_t<L>, expression_indices_t<R>>::type>,
      binary_expr<cts_metric_expr<Metric, L, R, InnerOnly>, L, R> {
  // Conservative support: cancellation and the grade convention may make
  // some of these coefficients zero. A later optimizer can prune them.
  using indices = typename detail::product_indices<expression_indices_t<L>, expression_indices_t<R>>::type;
  using index_type = typename indices::value_type;
  using value_type = std::common_type_t<typename L::value_type, typename R::value_type>;
  using base = binary_expr<cts_metric_expr<Metric, L, R, InnerOnly>, L, R>; using base::base;
private:
  template <index_type I> constexpr auto get_impl() const {
    return detail::metric_coefficient<Metric, InnerOnly, I>(this->template argument<0>(), this->template argument<1>(),
                                                            expression_indices_t<L>{}, expression_indices_t<R>{});
  }
  friend cts_value_mixin<cts_metric_expr, indices>;
};

template <class Metric>
struct OrthogonalGAContext : GrassmannContextBase<OrthogonalGAContext<Metric>> {
  using GrassmannContextBase<OrthogonalGAContext>::operator();
private:
  template <bool Inner, class Node> constexpr auto lower_metric(Node const &e) const {
    auto l = (*this)(e.template argument<0>()); auto r = (*this)(e.template argument<1>());
    return cts_metric_expr<Metric, decltype(l), decltype(r), Inner>{std::move(l), std::move(r)};
  }
public:
  template <class L, class R> constexpr auto operator()(prod_expr<L, R> const &e) const { return lower_metric<false>(e); }
  template <class L, class R> constexpr auto operator()(inner_prod_expr<L, R> const &e) const { return lower_metric<true>(e); }
};

/// Metric is necessarily explicit because it is the semantic choice. Support
/// and coefficient type are still deduced only after lowering the expression.
template <class Metric, Expression E>
[[nodiscard]] constexpr auto make_orthogonal_array(E const &syntax) {
  return make_cts_array<OrthogonalGAContext<Metric>>(syntax);
}
} // namespace ctv
