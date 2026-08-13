// additive_group.ixx
export module cts:additive_group;

import :cts_array;
import :expression_template;

export namespace cts {

template <CTS_Value LHS, CTS_Value RHS, auto index>
auto get(add_expr<LHS, RHS> const &arg) {
  return get<index>(arg.lhs) + get<index>(arg.rhs);
}

template <CTS_Value LHS, CTS_Value RHS, auto index>
auto get(minus_expr<LHS, RHS> const &arg) {
  return get<index>(arg.lhs) - get<index>(arg.rhs);
}

template <CTS_Value ARG, auto index> auto get(uminus_expr<ARG> const &arg) {
  return -get<index>(arg);
}

template <CTS_Value ARG, auto index> auto get(uplus_expr<ARG> const &arg) {
  return get<index>(arg);
}

} // namespace cts