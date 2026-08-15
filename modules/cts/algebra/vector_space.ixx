module;
#include <type_traits>
#include <utility>
export module ctv.vector_space;
export import ctv.additive_group;

export namespace ctv {
template <class E> concept ScalarCTSValue = CTS_Value<E> &&
  (set_difference_t<expression_indices_t<E>, index_set<0>>::size == 0);

template <class L, class R, class Set>
struct cts_scale_expr : cts_value_mixin<cts_scale_expr<L, R, Set>, Set>,
                        binary_expr<cts_scale_expr<L, R, Set>, L, R> {
  using indices = Set; using index_type = typename Set::value_type;
  using value_type = std::common_type_t<typename L::value_type, typename R::value_type>;
  using base = binary_expr<cts_scale_expr<L, R, Set>, L, R>; using base::base;
private:
  template <index_type I> constexpr auto get_impl() const {
    if constexpr (ScalarCTSValue<L>) return static_cast<value_type>(get<0>(this->template argument<0>())) * static_cast<value_type>(get<I>(this->template argument<1>()));
    else return static_cast<value_type>(get<I>(this->template argument<0>())) * static_cast<value_type>(get<0>(this->template argument<1>()));
  }
  friend cts_value_mixin<cts_scale_expr, indices>;
};

template <class Derived> struct VectorSpaceContextBase : AdditiveGroupContextBase<Derived> {
  using AdditiveGroupContextBase<Derived>::operator();
  template <class L, class R> constexpr auto operator()(prod_expr<L, R> const &e) const {
    auto const &self = static_cast<Derived const &>(*this);
    auto l = self(e.template argument<0>()); auto r = self(e.template argument<1>());
    static_assert(ScalarCTSValue<decltype(l)> || ScalarCTSValue<decltype(r)>,
                  "vector-space multiplication requires a scalar operand");
    using set = std::conditional_t<ScalarCTSValue<decltype(l)>, expression_indices_t<decltype(r)>, expression_indices_t<decltype(l)>>;
    return cts_scale_expr<decltype(l), decltype(r), set>{std::move(l), std::move(r)};
  }
};
struct VectorSpaceContext : VectorSpaceContextBase<VectorSpaceContext> {
  using VectorSpaceContextBase<VectorSpaceContext>::operator();
};

template <ValueSet Indices, class T>
struct vector_array : cts_array<VectorSpaceContext, Indices, T> {
  using base = cts_array<VectorSpaceContext, Indices, T>;
  using base::base;
  using base::operator=;
};

template <Expression E>
vector_array(E const &)
    -> vector_array<typename lowered_expression_t<E, VectorSpaceContext>::indices,
                    typename lowered_expression_t<E, VectorSpaceContext>::value_type>;
} // namespace ctv
