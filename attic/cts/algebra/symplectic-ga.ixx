// Legacy symplectic_ga.ixx
export module cts:symplectic_ga;

import :multivector;

export namespace cts {
// Symplectic geometric algebra
template <size_t Dim> // Dim must be even
class SymplecticGeometricAlgebra : public Multivector<Dim> {
public:
  using Multivector<Dim>::Multivector;

  // Symplectic product
  SymplecticGeometricAlgebra
  symplectic_product(const SymplecticGeometricAlgebra &other) const;
};
} // namespace cts
