// grassmann.ixx
export module cts:grassmann;

import :vector_space;
import :special_values;
import :utilities;

export namespace cts {

/// even bit numbered indices reverse their sign
template <CTS_Value Arg, auto index> auto get(reverse_expr<Arg> const &arg) {
  return bitcount(index) % 2 == 0 ? get<index>(arg.lhs) : -get<index>(arg.lhs);
}

// todo: note we only need 1 multiplication, the outerproduct is just a cast /
// Projection of the product into the outer product space, so we can just
// compute the product and then project it into the outer product space, which
// is just a cast. So we can do this with only 1 multiplication instead of 2.

} // namespace cts