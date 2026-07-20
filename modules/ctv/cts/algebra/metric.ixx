// metric.ixx
export module cts:metric;

import :utilities;

export namespace cts {
// Metric tensor
template <size_t Dim> class Metric {
  std::array<double, Dim * Dim> components_;

public:
  // Metric operations
  double dot(const std::array<double, Dim> &a,
             const std::array<double, Dim> &b) const;
};
} // namespace cts