module;
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

export module ctv.expression_template;

export namespace ctv {

struct expression_tag {
protected:
  constexpr expression_tag() = default;
  ~expression_tag() = default;
};

template <class E>
concept Expression = std::is_base_of_v<expression_tag, std::remove_cvref_t<E>>;

/// Domain-independent syntax node. Operands are deliberately owned by value.
template <class Derived, class... Args>
struct expression : private expression_tag {
  using arguments_type = std::tuple<Args...>;
  static constexpr std::size_t arity = sizeof...(Args);

  constexpr expression() requires(sizeof...(Args) == 0) = default;

  constexpr explicit expression(Args... values)
      noexcept(std::is_nothrow_constructible_v<arguments_type, Args...>)
    requires(sizeof...(Args) > 0)
      : args(std::move(values)...) {}

  template <std::size_t I, class Self>
  constexpr decltype(auto) argument(this Self &&self) noexcept {
    return std::get<I>(std::forward<Self>(self).args);
  }

  arguments_type args;
};

template <class Derived> using constant_expr = expression<Derived>;
template <class Derived, class Arg> using unary_expr = expression<Derived, Arg>;
template <class Derived, class Lhs, class Rhs>
using binary_expr = expression<Derived, Lhs, Rhs>;

#define CTV_BINARY_EXPRESSION(Name)                                            \
  template <class Lhs, class Rhs>                                             \
  struct Name : binary_expr<Name<Lhs, Rhs>, Lhs, Rhs> {                       \
    using binary_expr<Name<Lhs, Rhs>, Lhs, Rhs>::binary_expr;                 \
  }

CTV_BINARY_EXPRESSION(add_expr);
CTV_BINARY_EXPRESSION(minus_expr);
CTV_BINARY_EXPRESSION(prod_expr);
CTV_BINARY_EXPRESSION(outer_prod_expr);
CTV_BINARY_EXPRESSION(inner_prod_expr);
#undef CTV_BINARY_EXPRESSION

#define CTV_UNARY_EXPRESSION(Name)                                             \
  template <class Arg> struct Name : unary_expr<Name<Arg>, Arg> {              \
    using unary_expr<Name<Arg>, Arg>::unary_expr;                              \
  }

CTV_UNARY_EXPRESSION(uminus_expr);
CTV_UNARY_EXPRESSION(uplus_expr);
CTV_UNARY_EXPRESSION(reverse_expr);
#undef CTV_UNARY_EXPRESSION

template <Expression Lhs, Expression Rhs>
[[nodiscard]] constexpr auto operator+(Lhs &&lhs, Rhs &&rhs) {
  return add_expr<std::remove_cvref_t<Lhs>, std::remove_cvref_t<Rhs>>{
      std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)};
}

template <Expression Lhs, Expression Rhs>
[[nodiscard]] constexpr auto operator-(Lhs &&lhs, Rhs &&rhs) {
  return minus_expr<std::remove_cvref_t<Lhs>, std::remove_cvref_t<Rhs>>{
      std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)};
}

template <Expression Lhs, Expression Rhs>
[[nodiscard]] constexpr auto operator*(Lhs &&lhs, Rhs &&rhs) {
  return prod_expr<std::remove_cvref_t<Lhs>, std::remove_cvref_t<Rhs>>{
      std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)};
}

template <Expression Arg> [[nodiscard]] constexpr auto operator-(Arg &&arg) {
  return uminus_expr<std::remove_cvref_t<Arg>>{std::forward<Arg>(arg)};
}

template <Expression Arg> [[nodiscard]] constexpr auto operator+(Arg &&arg) {
  return uplus_expr<std::remove_cvref_t<Arg>>{std::forward<Arg>(arg)};
}

template <Expression Lhs, Expression Rhs>
[[nodiscard]] constexpr auto outer_product(Lhs &&lhs, Rhs &&rhs) {
  return outer_prod_expr<std::remove_cvref_t<Lhs>, std::remove_cvref_t<Rhs>>{
      std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)};
}

template <Expression Lhs, Expression Rhs>
[[nodiscard]] constexpr auto inner_product(Lhs &&lhs, Rhs &&rhs) {
  return inner_prod_expr<std::remove_cvref_t<Lhs>, std::remove_cvref_t<Rhs>>{
      std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)};
}

template <Expression Arg> [[nodiscard]] constexpr auto reverse(Arg &&arg) {
  return reverse_expr<std::remove_cvref_t<Arg>>{std::forward<Arg>(arg)};
}

} // namespace ctv
