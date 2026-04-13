#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

import ctv.special_values;

TEST_CASE("special value constants convert to arithmetic values")
{
    CHECK(static_cast<int>(ctv::zero) == 0);
    CHECK(static_cast<int>(ctv::one) == 1);
    CHECK(static_cast<int>(ctv::minus_one) == -1);
}
