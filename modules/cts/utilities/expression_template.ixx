// expression_template.ixx

import std;

export module cts:expression_template;

import :utilities;

export namespace cts {

struct expression_tag {};

template <typename E>
concept Expression = std::is_base_of_v<expression_tag, E>;

template <typename Derived, typename... ARGS> struct expression {
  explicit expression(ARGS &&...) : args{std::forward<ARGS>(args)...} {}
  template <typename Self> auto &&self(this Self &&this_) {
    return static_cast<Derived &&>(this_);
  }
  std::tuple<ARGS...> args;
};

template <typename T> inline constexpr bool is_expression_v = false;

template <typename Derived, typename... ARGS>
inline constexpr bool is_expression_v<expression<Derived, ARGS...>> = true;

template <typename E>
concept Expression = is_expression_v<std::remove_cvref_t<E>>;

template <typename LHS, typename RHS>
struct add_expr : expression<add_expr, LHS, RHS> {
  using expression::expression;
};

template <typename LHS, typename RHS>
struct minus_expr : expression<minus_expr, LHS, RHS> {};

template <typename LHS, typename RHS>
struct prod_expr : expression<prod_expr, LHS, RHS> {};

// maybe not needed, but could be useful for some cases
template <typename LHS, typename RHS>
struct outer_prod_expr : expression<outer_prod_expr, LHS, RHS> {};

// maybe not needed, but could be useful for some cases
template <typename LHS, typename RHS>
struct inner_prod_expr : expression<inner_prod_expr, LHS, RHS> {
}; // takes metric fom context

// template <typename LHS, typename RHS>
// struct join_expr :  expression<join_expr, LHS, RHS> {};

// template <typename LHS, typename RHS>
// struct meet_expr :  expression<meet_expr, LHS, RHS> {};

// maybe not needed, but could be useful for some cases
template <typename ARG> struct uminus_expr : expression<uminus_expr, ARG> {};

// maybe not needed, but could be useful for some cases
template <typename ARG> struct uplus_expr : expression<uplus_expr, ARG> {};

template <typename ARG> struct reverse : expression<reverse, ARG> {};

template <typename ARG> using squared_expr = prod_expr<ARG, ARG>;

} // namespace cts
