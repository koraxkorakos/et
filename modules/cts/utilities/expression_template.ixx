// expression_template.ixx

import std; // Imports the C++ Standard Library modules

export module cts:expression_template; // Exports this module as 'cts:expression_template'

import :utilities; // Imports the 'utilities' submodule from 'cts'

export namespace cts {

/// \brief Tag struct to identify expression types.
///        Only types inheriting from `expression` can be considered `Expression`s.
///        Special member functions are defaulted; `inline` and `constexpr` are implicit where applicable.
struct expression_tag {
protected: // Protected access allows `expression` to inherit from it.
   expression_tag() = default;
   ~expression_tag() = default;
   expression_tag(expression_tag const &) = default;
   expression_tag(expression_tag &&) = default;
   expression_tag &operator=(expression_tag const &) = default;
   expression_tag &operator=(expression_tag &&) = default;
};

/// \brief Concept to check if a type `E` is an expression, by verifying it inherits from `expression_tag`.
template <typename E>
concept Expression = std::is_base_of_v<expression_tag, E>;

/// \brief CRTP mixin to create an expression.
///        Derived types inherit from this to gain common expression functionalities.
/// \tparam Derived The actual derived class inheriting from this mixin.
/// \tparam ARGS The types of arguments/operands this expression holds.
template <typename Derived, typename... ARGS>
struct expression : private expression_tag {
  // Constructor forwards arguments to initialize the internal tuple.
  explicit expression(ARGS &&... init_args) : args(std::forward<ARGS>(init_args)...) {}

  /// \brief Returns a reference to the derived type, preserving const-qualification and value category.
  ///        Uses C++23 "deducing this" to avoid boilerplate for lvalue, const lvalue, and rvalue references.
  template <typename Self>
  constexpr decltype(auto) self(this Self&& self_ref) {
      // Determine the return type: Derived with the same ref-qualifier and const-qualifier as self_ref
      using ReturnType = std::conditional_t<
          std::is_lvalue_reference_v<Self>, // If Self is an lvalue reference (e.g., expression<D>& or const expression<D>&)
          std::conditional_t<
              std::is_const_v<std::remove_reference_t<Self>>, // Check if the underlying type of Self is const
              const Derived&, // Then return const Derived&
              Derived&        // Else return Derived&
          >,
          Derived&& // If Self is an rvalue reference (Self is expression<D>, self_ref is expression<D>&&)
      >;

      // Perform the static_cast to the determined ReturnType
      return static_cast<ReturnType>(self_ref);
  }

  /// \brief Tuple to store the arguments/operands of the expression.
  std::tuple<ARGS...> args;

private:
    // Defaulted special member functions. Friend class Derived allows Derived to access these.
    expression() = default;
    expression(expression const &) = default;
    expression(expression &&) = default;
    expression &operator=(expression const &) = default;
    expression &operator=(expression &&) = default;
    ~expression() = default;

    friend class Derived; // Grants Derived access to private members of this base class.
};

// --- Convenience Aliases for Expression Types ---

/// \brief Alias for nullary expressions (expressions with no arguments).
template <typename Derived>
using constant_expr = expression<Derived>; // No ARGS for constant expressions

/// \brief Alias for unary expressions (expressions with one argument).
template <typename Derived, typename ARG>
using unary_expr = expression<Derived, ARG>;

/// \brief Alias for binary expressions (expressions with two arguments).
template <typename Derived, typename LHS, typename RHS>
using binary_expr = expression<Derived, LHS, RHS>; // Correctly uses expression with two ARGS

// --- Individual Expression Implementations ---

template <typename LHS, typename RHS>
struct add_expr : binary_expr<add_expr<LHS, RHS>, LHS, RHS> { // Corrected base to binary_expr
  using binary_expr<add_expr<LHS, RHS>, LHS, RHS>::binary_expr;
};

template <typename LHS, typename RHS>
struct minus_expr : binary_expr<minus_expr<LHS, RHS>, LHS, RHS> { // Corrected base to binary_expr
  using binary_expr<minus_expr<LHS, RHS>, LHS, RHS>::binary_expr;
};

template <typename LHS, typename RHS>
struct prod_expr : binary_expr<prod_expr<LHS, RHS>, LHS, RHS> { // Corrected base to binary_expr
  using binary_expr<prod_expr<LHS, RHS>, LHS, RHS>::binary_expr;
};

// Maybe not needed, but could be useful for some cases
template <typename LHS, typename RHS>
struct outer_prod_expr : binary_expr<outer_prod_expr<LHS, RHS>, LHS, RHS> { // Corrected base to binary_expr
  using binary_expr<outer_prod_expr<LHS, RHS>, LHS, RHS>::binary_expr;
};

// Maybe not needed, but could be useful for some cases
template <typename LHS, typename RHS>
struct inner_prod_expr : binary_expr<inner_prod_expr<LHS, RHS>, LHS, RHS> { // Corrected base to binary_expr
  using binary_expr<inner_prod_expr<LHS, RHS>, LHS, RHS>::binary_expr;
}; // Takes metric from context

// template <typename LHS, typename RHS>
// struct join_expr :  binary_expr<join_expr, LHS, RHS> {
//   using binary_expr<join_expr, LHS, RHS>::binary_expr;
//};

// template <typename LHS, typename RHS>
// struct meet_expr :  binary_expression<meet_expr, LHS, RHS> {
//   using binary_expr<meet_expr, LHS, RHS>::binary_expr;
//};

// Maybe not needed, but could be useful for some cases
template <typename ARG>
struct uminus_expr : unary_expr<uminus_expr<ARG>, ARG> { // Corrected base to unary_expr
  using unary_expr<uminus_expr<ARG>, ARG>::unary_expr;
};

// Maybe not needed, but could be useful for some cases
template <typename ARG>
struct uplus_expr : unary_expr<uplus_expr<ARG>, ARG> { // Corrected base to unary_expr and template arg
  using unary_expr<uplus_expr<ARG>, ARG>::unary_expr;
};

template <typename ARG>
struct reverse : unary_expr<reverse<ARG>, ARG> { // Corrected base to unary_expr
  using unary_expr<reverse<ARG>, ARG>::unary_expr;
};

//template <typename ARG> using squared_expr = prod_expr<ARG, ARG>;

} // namespace cts