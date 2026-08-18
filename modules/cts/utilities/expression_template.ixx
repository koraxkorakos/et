module;
#include <concepts>
#include <cstddef>
#include <memory>
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

/// A terminal expression which owns its value.
template <class T>
struct constant_expression : constant_expr<constant_expression<T>> {
  using value_type = T;

  template <class U>
    requires std::constructible_from<T, U>
  constexpr explicit constant_expression(U &&value)
      noexcept(std::is_nothrow_constructible_v<T, U>)
      : stored_value(std::forward<U>(value)) {}

  template <class Self>
  [[nodiscard]] constexpr decltype(auto) get(this Self &&self) noexcept {
    return std::forward_like<Self>(self.stored_value);
  }

private:
  T stored_value;
};

template <class T> constant_expression(T) -> constant_expression<T>;

/// A terminal expression which refers to an externally owned value.
///
/// Like std::reference_wrapper, copying this expression copies the reference,
/// not the referenced object. The object must outlive the expression tree.
template <class T>
struct reference_expression : constant_expr<reference_expression<T>> {
  using value_type = T;

  constexpr explicit reference_expression(T &value) noexcept
      : stored_reference(std::addressof(value)) {}

  [[nodiscard]] constexpr T &get() const noexcept { return *stored_reference; }
  constexpr operator T &() const noexcept { return get(); }

private:
  T *stored_reference;
};

template <class T> reference_expression(T &) -> reference_expression<T>;

/// Lift a value into the expression-template domain by owning it.
template <class T>
[[nodiscard]] constexpr auto constant(T &&value)
    noexcept(std::is_nothrow_constructible_v<std::decay_t<T>, T>) {
  return constant_expression<std::decay_t<T>>{std::forward<T>(value)};
}

template <class T>
[[nodiscard]] constexpr auto value(T &&object)
    noexcept(noexcept(constant(std::forward<T>(object)))) {
  return constant(std::forward<T>(object));
}

/// Lift an lvalue into the expression-template domain without copying it.
template <class T>
[[nodiscard]] constexpr auto reference(T &value) noexcept {
  return reference_expression<T>{value};
}

template <class T> void reference(T const &&) = delete;

template <class T>
[[nodiscard]] constexpr auto ref(T &object) noexcept {
  return reference(object);
}

template <class T> void ref(T const &&) = delete;

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
