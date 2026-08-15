module;
#include <type_traits>
#include <utility>
export module ctv.additive_group;
export import ctv.array;

export namespace ctv {
template <class E> using expression_indices_t = typename std::remove_cvref_t<E>::indices;
template <class L, class R> using common_indices_t = set_union_t<expression_indices_t<L>, expression_indices_t<R>>;

template <class L, class R, bool Subtract>
struct cts_sum_expr : cts_value_mixin<cts_sum_expr<L, R, Subtract>, common_indices_t<L, R>>,
                      binary_expr<cts_sum_expr<L, R, Subtract>, L, R> {
  using indices = common_indices_t<L, R>;
  using index_type = typename indices::value_type;
  using value_type = std::common_type_t<typename L::value_type, typename R::value_type>;
  using base = binary_expr<cts_sum_expr<L, R, Subtract>, L, R>;
  using base::base;
private:
  template <index_type I> constexpr auto get_impl() const {
    auto lhs = static_cast<value_type>(get<I>(this->template argument<0>()));
    auto rhs = static_cast<value_type>(get<I>(this->template argument<1>()));
    if constexpr (Subtract) return lhs - rhs;
    else return lhs + rhs;
  }
  friend cts_value_mixin<cts_sum_expr, indices>;
};

template <class A, bool Negate>
struct cts_sign_expr : cts_value_mixin<cts_sign_expr<A, Negate>, expression_indices_t<A>>,
                       unary_expr<cts_sign_expr<A, Negate>, A> {
  using indices = expression_indices_t<A>;
  using index_type = typename indices::value_type;
  using value_type = typename A::value_type;
  using base = unary_expr<cts_sign_expr<A, Negate>, A>;
  using base::base;
private:
  template <index_type I> constexpr auto get_impl() const {
    if constexpr (Negate) return -get<I>(this->template argument<0>());
    else return +get<I>(this->template argument<0>());
  }
  friend cts_value_mixin<cts_sign_expr, indices>;
};

template <class Derived> struct AdditiveGroupContextBase : ArrayContext {
private:
  constexpr Derived const &derived() const { return static_cast<Derived const &>(*this); }
public:
  template <CTS_Value E> constexpr auto operator()(E &&e) const { return std::forward<E>(e); }

  template <class L, class R> constexpr auto operator()(add_expr<L, R> const &e) const {
    auto l = derived()(e.template argument<0>()); auto r = derived()(e.template argument<1>());
    using set = common_indices_t<decltype(l), decltype(r)>;
    auto lp = project<set>(std::move(l)); auto rp = project<set>(std::move(r));
    return cts_sum_expr<decltype(lp), decltype(rp), false>{std::move(lp), std::move(rp)};
  }
  template <class L, class R> constexpr auto operator()(minus_expr<L, R> const &e) const {
    auto l = derived()(e.template argument<0>()); auto r = derived()(e.template argument<1>());
    using set = common_indices_t<decltype(l), decltype(r)>;
    auto lp = project<set>(std::move(l)); auto rp = project<set>(std::move(r));
    return cts_sum_expr<decltype(lp), decltype(rp), true>{std::move(lp), std::move(rp)};
  }
  template <class A> constexpr auto operator()(uminus_expr<A> const &e) const {
    auto a = derived()(e.template argument<0>()); return cts_sign_expr<decltype(a), true>{std::move(a)};
  }
  template <class A> constexpr auto operator()(uplus_expr<A> const &e) const {
    auto a = derived()(e.template argument<0>()); return cts_sign_expr<decltype(a), false>{std::move(a)};
  }
};
struct AdditiveGroupContext : AdditiveGroupContextBase<AdditiveGroupContext> {
  using AdditiveGroupContextBase::operator();
};

template <class E, class Context>
using lowered_expression_t = decltype(std::declval<Context const &>()(
    std::declval<E const &>()));

/// A named sink family supplies the context to CTAD. Thus the context and the
/// still-generic expression first meet in this converting constructor.
template <ValueSet Indices, class T>
struct additive_array : cts_array<AdditiveGroupContext, Indices, T> {
  using base = cts_array<AdditiveGroupContext, Indices, T>;
  using base::base;
  using base::operator=;
};

template <Expression E>
additive_array(E const &)
    -> additive_array<typename lowered_expression_t<E, AdditiveGroupContext>::indices,
                      typename lowered_expression_t<E, AdditiveGroupContext>::value_type>;
} // namespace ctv
