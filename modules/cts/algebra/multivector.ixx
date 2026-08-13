// multivector.ixx
export module cts:multivector;

import :vector_space;
import :metric;

export namespace cts {
// Multivector implementation
template <size_t Dim>
class Multivector : public VectorSpaceWrapper<Multivector<Dim>, double> {
  Metric<Dim> metric_;
  // Grade components
  std::array<double, 1 << Dim> components_;

public:
  explicit Multivector(const Metric<Dim> &metric) : metric_(metric) {}

  // Geometric product and other operations
  // Contains metric as member
};
} // namespace cts