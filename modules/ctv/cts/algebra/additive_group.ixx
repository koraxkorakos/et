// additive_group.ixx
export module cts:additive_group;

import :cts_array;
import :expression_template;

export namespace cts {
// Additive group concept and implementation
template <typename T>
concept AdditiveGroup = requires(T a, T b) {
  { a + b } -> std::convertible_to<T>;
  { a - b } -> std::convertible_to<T>;
  { -a } -> std::convertible_to<T>;
  { T{} } -> std::convertible_to<T>;
};

template <typename T>
  requires AdditiveGroup<T>
class AdditiveGroupWrapper : public CTSArray<T, 1> {
  // Extends CTSArray with additive group operations
};
} // namespace cts