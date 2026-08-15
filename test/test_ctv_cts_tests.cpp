#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

import ctv.orthogonal_ga;

#include <array>
#include <cstddef>
#include <type_traits>

namespace {

using terminal =
    ctv::cts_array<ctv::GrassmannContext, ctv::index_set<1, 2>, int>;

struct euclidean_metric {
  constexpr int operator()(std::size_t) const noexcept { return 1; }
};

static_assert(ctv::CTS_Variable<terminal>);
static_assert(std::same_as<decltype(get<0>(terminal{})),
                           ctv::zero_type const>);

} // namespace

TEST_CASE("sink context lowers syntax and CTAD deduces structural support") {
  terminal a{std::array{2, 3}};
  terminal b{std::array{5, 7}};

  auto syntax = ctv::outer_product(a, b) + a;
  ctv::grassmann_array result{syntax};

  using expected_support = ctv::index_set<1, 2, 3>;
  CHECK((std::same_as<typename decltype(result)::indices, expected_support>));
  CHECK(get<1>(result) == 2);
  CHECK(get<2>(result) == 3);
  CHECK(get<3>(result) == -1);
}

TEST_CASE("a value projection may widen support with ordinary numeric zeros") {
  terminal source{std::array{3, 4}};
  auto wide = ctv::project<ctv::index_set<0, 1, 2, 3>>(source);

  CHECK((std::same_as<typename decltype(wide)::indices,
                      ctv::index_set<0, 1, 2, 3>>));
  CHECK((std::same_as<decltype(get<0>(wide)), int>));
  CHECK(get<0>(wide) == 0);
  CHECK(get<1>(wide) == 3);
  CHECK(get<2>(wide) == 4);
  CHECK(get<3>(wide) == 0);

  ctv::cts_array<ctv::GrassmannContext, ctv::index_set<0, 1, 2, 3>, int>
      external_layout{wide};
  CHECK(get<0>(external_layout) == 0);
  CHECK(get<3>(external_layout) == 0);
}

TEST_CASE("vector-space and geometric contexts interpret multiplication") {
  terminal vector{std::array{2, 3}};
  using scalar =
      ctv::cts_array<ctv::VectorSpaceContext, ctv::index_set<0>, int>;
  scalar scale{std::array{4}};

  auto scaled = ctv::VectorSpaceContext{}(scale * vector);
  CHECK(get<1>(scaled) == 8);
  CHECK(get<2>(scaled) == 12);

  auto geometric =
      ctv::OrthogonalGAContext<euclidean_metric>{}(vector * vector);
  CHECK(get<0>(geometric) == 13);
  CHECK(get<3>(geometric) == 0);
}
