// vector_space.ixx
export module cts:vector_space;

import :additive_group;

export namespace cts {

template <CTS_Value LHS, ScalarCTS_Value RHS, auto index>
auto get(prod_expr<LHS, RHS> const &arg) {
  return get<index>(arg.lhs) * get<0>(arg.rhs);
}

template <ScalarCTS_Value LHS, CTS_Value RHS, auto index>
auto get(prod_expr<LHS, RHS> const &arg) {
  return get<0>(arg.lhs) * get<index>(arg.rhs);
}

template <ScalarCTS_Value LHS, ScalarCTS_Value RHS, auto index>
  requires(!index)
auto get(prod_expr<LHS, RHS> const &arg) {
  return get<0>(arg.lhs) * get<0>(arg.rhs);
}

} // namespace cts