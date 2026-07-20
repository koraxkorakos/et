// vector_space.ixx
export module cts:vector_space;

import :additive_group;

export namespace cts {
// Vector space concept
template <typename T, typename Scalar = double>
concept VectorSpace = AdditiveGroup<T> && requires(T v, Scalar s) {
  { v * s } -> std::convertible_to<T>;
  { s * v } -> std::convertible_to<T>;
};

template <typename T, typename Scalar = double>
  requires VectorSpace<T, Scalar>
class VectorSpaceWrapper : public AdditiveGroupWrapper<T> {
  // Extends AdditiveGroup with scalar multiplication
};
} // namespace cts