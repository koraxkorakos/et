// expression_template.ixx
export module cts:expression_template;

import :utilities;

export namespace cts {
// Expression template infrastructure
template <typename Derived> struct Expression {
  // Common expression template functionality
};

// Example expression template implementation
template <typename T>
class ScalarExpression : public Expression<ScalarExpression<T>> {
  T value_;

public:
  explicit ScalarExpression(T value) : value_(value) {}
  // ...
};
} // namespace cts