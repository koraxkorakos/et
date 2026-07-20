// grassmann.ixx
export module cts:grassmann;

import :multivector;

export namespace cts {
// Grassmann algebra (exterior algebra)
template <size_t Dim> class GrassmannAlgebra : public Multivector<Dim> {
public:
  using Multivector<Dim>::Multivector;

  // Wedge product operations
  GrassmannAlgebra wedge(const GrassmannAlgebra &other) const;

  // Grade projection
  GrassmannAlgebra grade(size_t k) const;
};
} // namespace cts