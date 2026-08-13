// orthogonal_ga.ixx
export module cts:orthogonal_ga;

import :grassmann;

export namespace cts {
// Orthogonal geometric algebra (Euclidean/spacetime)
template <size_t Dim, int Signature = Dim> // Positive definite by default
class OrthogonalGeometricAlgebra : public GrassmannAlgebra<Dim> {
public:
  using GrassmannAlgebra<Dim>::GrassmannAlgebra;

  // Hodge star operator
  OrthogonalGeometricAlgebra hodge_star() const;

  // Reverse operation
  OrthogonalGeometricAlgebra reverse() const;
};
} // namespace cts