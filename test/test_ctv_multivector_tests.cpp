#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

import ctv.multivector;
import ctv.value_set;

#include <string>
#include <tuple>
#include <type_traits>

namespace
{
    enum class axis
    {
        scalar = 0,
        e1 = 1,
        e2 = 2
    };

    struct nullary_expression
    {
        using index_set = ctv::value_set<axis, axis::scalar, axis::e2>;

        int scalar;
        float e2;
    };

    template <axis Index>
    constexpr decltype(auto) get(nullary_expression const& expression)
    {
        if constexpr (Index == axis::scalar)
            return expression.scalar;
        else
            return expression.e2;
    }

    struct unary_expression
    {
        using index_set = nullary_expression::index_set;
        nullary_expression operand;
    };

    template <axis Index>
    constexpr auto get(unary_expression const& expression)
    {
        return -get<Index>(expression.operand);
    }

    struct binary_expression
    {
        using index_set = nullary_expression::index_set;
        nullary_expression lhs;
        unary_expression rhs;
    };

    template <axis Index>
    constexpr auto get(binary_expression const& expression)
    {
        return get<Index>(expression.lhs) + get<Index>(expression.rhs);
    }
}

TEST_CASE("multivector CTAD materializes nullary, unary, and binary expressions")
{
    nullary_expression terminal{4, 2.5F};
    ctv::multivector nullary{terminal};
    ctv::multivector unary{unary_expression{terminal}};
    ctv::multivector binary{
        binary_expression{terminal, unary_expression{terminal}}};

    using expected = ctv::value_set<axis, axis::scalar, axis::e2>;
    static_assert(std::same_as<typename decltype(nullary)::index_set, expected>);
    static_assert(std::same_as<std::tuple_element_t<0, decltype(nullary)>, int>);
    static_assert(std::same_as<std::tuple_element_t<1, decltype(nullary)>, float>);
    static_assert(std::same_as<std::tuple_element_t<0, decltype(unary)>, int>);
    static_assert(std::same_as<std::tuple_element_t<1, decltype(binary)>, float>);

    CHECK(ctv::get<axis::scalar>(nullary) == 4);
    CHECK(ctv::get<axis::e2>(unary) == doctest::Approx(-2.5F));
    CHECK(ctv::get<axis::scalar>(binary) == 0);
}

TEST_CASE("multivector stores heterogeneous elements under custom indices")
{
    using mv_t = ctv::multivector<
        ctv::value_set<axis, axis::scalar, axis::e1, axis::e2>,
        int, double, std::string>;

    static_assert(mv_t::size == 3);
    static_assert(std::is_same_v<typename mv_t::index_type, axis>);
    static_assert(std::is_same_v<
        typename mv_t::index_set,
        ctv::value_set<axis, axis::scalar, axis::e1, axis::e2>>);

    mv_t mv{1, 2.5, "xy"};

    CHECK(ctv::get<axis::scalar>(mv) == 1);
    CHECK(ctv::get<axis::e1>(mv) == doctest::Approx(2.5));
    CHECK(ctv::get<axis::e2>(mv) == "xy");
}

TEST_CASE("multivector supports default and tuple-based construction")
{
    using mv_t = ctv::multivector<
        ctv::value_set<axis, axis::scalar, axis::e1, axis::e2>,
        int, double, std::string>;

    mv_t zero{};
    CHECK(ctv::get<axis::scalar>(zero) == 0);
    CHECK(ctv::get<axis::e1>(zero) == doctest::Approx(0.0));
    CHECK(ctv::get<axis::e2>(zero).empty());

    typename mv_t::storage_type data{4, 7.5, "basis"};
    mv_t from_tuple{data};

    CHECK(ctv::get<axis::scalar>(from_tuple) == 4);
    CHECK(ctv::get<axis::e1>(from_tuple) == doctest::Approx(7.5));
    CHECK(ctv::get<axis::e2>(from_tuple) == "basis");
}

TEST_CASE("multivector get returns references preserving mutability")
{
    using mv_t = ctv::multivector<
        ctv::value_set<int, -1, 2>,
        int, double>;

    mv_t mv{7, 3.5};

    static_assert(std::is_same_v<decltype(ctv::get<-1>(mv)), int&>);
    static_assert(std::is_same_v<decltype(ctv::get<2>(mv)), double&>);

    ctv::get<-1>(mv) = 11;
    ctv::get<2>(mv) = 9.25;

    CHECK(ctv::get<-1>(mv) == 11);
    CHECK(ctv::get<2>(mv) == doctest::Approx(9.25));
}

TEST_CASE("multivector const access and tuple traits work")
{
    using mv_t = ctv::multivector<
        ctv::value_set<axis, axis::scalar, axis::e1>,
        long, float>;

    mv_t const mv{4L, 1.25F};

    static_assert(std::tuple_size_v<mv_t> == 2);
    static_assert(std::is_same_v<std::tuple_element_t<0, mv_t>, long>);
    static_assert(std::is_same_v<std::tuple_element_t<1, mv_t>, float>);
    static_assert(std::is_same_v<decltype(ctv::get<axis::scalar>(mv)), long const&>);

    CHECK(ctv::get<axis::scalar>(mv) == 4L);
    CHECK(ctv::get<axis::e1>(mv) == doctest::Approx(1.25F));
}

TEST_CASE("multivector rvalue get preserves value categories")
{
    using mv_t = ctv::multivector<
        ctv::value_set<axis, axis::scalar, axis::e1>,
        std::string, double>;

    static_assert(std::is_same_v<
        decltype(ctv::get<axis::scalar>(std::declval<mv_t&&>())),
        std::string&&>);
    static_assert(std::is_same_v<
        decltype(ctv::get<axis::scalar>(std::declval<mv_t const&&>())),
        std::string const&&>);

    mv_t mv{"mv", 2.0};
    auto moved = ctv::get<axis::scalar>(std::move(mv));

    CHECK(moved == "mv");
}
