module;
#include <bit>
#include <type_traits>
#include <utility>
export module ctv.grassmann;
export import ctv.vector_space;

namespace ctv::detail {
template <class L, class R> struct outer_indices;
template <class T, T L, T... R> struct outer_row;
template <class T, T L> struct outer_row<T, L> { using type = value_set<T>; };
template <class T, T L, T R, T... Rest> struct outer_row<T, L, R, Rest...> {
  using head = std::conditional_t<(L & R) == 0, value_set<T, static_cast<T>(L ^ R)>, value_set<T>>;
  using type = set_union_t<head, typename outer_row<T, L, Rest...>::type>;
};
template <class T, T... R>
struct outer_indices<value_set<T>, value_set<T, R...>> { using type = value_set<T>; };
template <class T, T L, T... LS, T... R>
struct outer_indices<value_set<T, L, LS...>, value_set<T, R...>> {
  using head = typename outer_row<T, L, R...>::type;
  using tail = typename outer_indices<value_set<T, LS...>, value_set<T, R...>>::type;
  using type = set_union_t<head, tail>;
};
template <class L, class R> using outer_indices_t = typename outer_indices<L, R>::type;

template <std::unsigned_integral T> constexpr int wedge_sign(T lhs, T rhs) {
  int sign = 1;
  while (lhs) {
    auto bit = std::countr_zero(lhs);
    auto lower = bit == 0 ? T{} : static_cast<T>((T{1} << bit) - 1);
    if (std::popcount(static_cast<T>(rhs & lower)) & 1) sign = -sign;
    lhs &= static_cast<T>(lhs - 1);
  }
  return sign;
}

template <auto I, auto A, class L, class R, class T, T... RI>
constexpr auto outer_coefficient_row(L const &l, R const &r,
                                     value_set<T, RI...>) {
  using result = std::common_type_t<typename L::value_type, typename R::value_type>;
  result sum = coefficient_traits<result>::zero();
  ([&] {
    if constexpr ((A & RI) == 0 && (A ^ RI) == I)
      sum += static_cast<result>(wedge_sign(A, RI)) *
             static_cast<result>(get<A>(l)) *
             static_cast<result>(get<RI>(r));
  }(), ...);
  return sum;
}

template <auto I, class L, class R, class T, T... LI, T... RI>
constexpr auto outer_coefficient(L const &l, R const &r,
                                 value_set<T, LI...>, value_set<T, RI...>) {
  using result =
      std::common_type_t<typename L::value_type, typename R::value_type>;
  result sum = coefficient_traits<result>::zero();
  ((sum += outer_coefficient_row<I, LI>(l, r, value_set<T, RI...>{})), ...);
  return sum;
}
} // namespace ctv::detail

export namespace ctv {
template <class L, class R>
using outer_indices_t = detail::outer_indices_t<expression_indices_t<L>, expression_indices_t<R>>;

template <class L, class R>
struct cts_outer_expr
    : cts_value_mixin<cts_outer_expr<L, R>, detail::outer_indices_t<expression_indices_t<L>, expression_indices_t<R>>>,
      binary_expr<cts_outer_expr<L, R>, L, R> {
  using indices = detail::outer_indices_t<expression_indices_t<L>, expression_indices_t<R>>;
  using index_type = typename indices::value_type;
  using value_type = std::common_type_t<typename L::value_type, typename R::value_type>;
  using base = binary_expr<cts_outer_expr<L, R>, L, R>; using base::base;
private:
  template <index_type I> constexpr auto get_impl() const {
    return detail::outer_coefficient<I>(this->template argument<0>(), this->template argument<1>(),
                                        expression_indices_t<L>{}, expression_indices_t<R>{});
  }
  friend cts_value_mixin<cts_outer_expr, indices>;
};

template <class A>
struct cts_reverse_expr : cts_value_mixin<cts_reverse_expr<A>, expression_indices_t<A>>,
                          unary_expr<cts_reverse_expr<A>, A> {
  using indices = expression_indices_t<A>; using index_type = typename indices::value_type;
  using value_type = typename A::value_type;
  using base = unary_expr<cts_reverse_expr<A>, A>; using base::base;
private:
  template <index_type I> constexpr auto get_impl() const {
    constexpr auto grade = std::popcount(I);
    if constexpr (((grade * (grade - 1) / 2) & 1) != 0) return -get<I>(this->template argument<0>());
    else return +get<I>(this->template argument<0>());
  }
  friend cts_value_mixin<cts_reverse_expr, indices>;
};

template <class Derived> struct GrassmannContextBase : VectorSpaceContextBase<Derived> {
  using VectorSpaceContextBase<Derived>::operator();
private:
  template <class Node> constexpr auto lower_outer(Node const &e) const {
    auto const &self = static_cast<Derived const &>(*this);
    auto l = self(e.template argument<0>()); auto r = self(e.template argument<1>());
    return cts_outer_expr<decltype(l), decltype(r)>{std::move(l), std::move(r)};
  }
public:
  template <class L, class R> constexpr auto operator()(outer_prod_expr<L, R> const &e) const { return lower_outer(e); }
  // In an exterior algebra, juxtaposition/geometric syntax has the zero metric.
  template <class L, class R> constexpr auto operator()(prod_expr<L, R> const &e) const { return lower_outer(e); }
  template <class A> constexpr auto operator()(reverse_expr<A> const &e) const {
    auto a = static_cast<Derived const &>(*this)(e.template argument<0>()); return cts_reverse_expr<decltype(a)>{std::move(a)};
  }
};
struct GrassmannContext : GrassmannContextBase<GrassmannContext> {
  using GrassmannContextBase<GrassmannContext>::operator();
};

template <ValueSet Indices, class T>
struct grassmann_array : cts_array<GrassmannContext, Indices, T> {
  using base = cts_array<GrassmannContext, Indices, T>;
  using base::base;
  using base::operator=;
};

template <Expression E>
grassmann_array(E const &)
    -> grassmann_array<typename lowered_expression_t<E, GrassmannContext>::indices,
                       typename lowered_expression_t<E, GrassmannContext>::value_type>;
} // namespace ctv
