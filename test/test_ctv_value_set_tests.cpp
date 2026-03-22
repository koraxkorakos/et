#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

import ctv.value_set;

#include <type_traits>
#include <cstddef>

TEST_CASE("value_set basic invariants")
{
    using S = ctv::value_set<int, 1, 3, 5>;
    CHECK(S::size == 3);
    CHECK(std::is_same_v<typename S::value_type, int>);

    using L = ctv::value_list<int, 5, 1, 3, 1>;
    CHECK(L::size == 4);
    CHECK(std::is_same_v<typename L::value_type, int>);
}

TEST_CASE("normalize value_list to value_set")
{
    using L = ctv::value_list<int, 5, 1, 3, 1, 2, 5>;
    using N = ctv::normalize_t<L>;
    CHECK(std::is_same_v<N, ctv::value_set<int, 1, 2, 3, 5>>);
}

TEST_CASE("set_union")
{
    using A = ctv::value_set<int, 1, 3, 5, 9>;
    using B = ctv::value_set<int, 2, 3, 4, 9, 10>;
    using U = ctv::set_union_t<A, B>;
    CHECK(std::is_same_v<U, ctv::value_set<int, 1, 2, 3, 4, 5, 9, 10>>);
}

TEST_CASE("set_intersection")
{
    using A = ctv::value_set<int, 1, 3, 5, 9>;
    using B = ctv::value_set<int, 2, 3, 4, 9, 10>;
    using I = ctv::set_intersection_t<A, B>;
    CHECK(std::is_same_v<I, ctv::value_set<int, 3, 9>>);
}

TEST_CASE("set_difference")
{
    using A = ctv::value_set<int, 1, 3, 5, 9>;
    using B = ctv::value_set<int, 2, 3, 4, 9, 10>;
    using D1 = ctv::set_difference_t<A, B>;
    using D2 = ctv::set_difference_t<B, A>;
    CHECK(std::is_same_v<D1, ctv::value_set<int, 1, 5>>);
    CHECK(std::is_same_v<D2, ctv::value_set<int, 2, 4, 10>>);
}

TEST_CASE("set_symmetric_difference")
{
    using A = ctv::value_set<int, 1, 3, 5, 9>;
    using B = ctv::value_set<int, 2, 3, 4, 9, 10>;
    using SD = ctv::set_symmetric_difference_t<A, B>;
    CHECK(std::is_same_v<SD, ctv::value_set<int, 1, 2, 4, 5, 10>>);
}

TEST_CASE("list_union normalizes concatenated raw lists")
{
    using A = ctv::value_list<int, 5, 1, 7>;
    using B = ctv::value_list<int, 2, 7, 4>;
    using U = ctv::list_union_t<A, B>;
    CHECK(std::is_same_v<U, ctv::value_set<int, 1, 2, 4, 5, 7>>);
}

TEST_CASE("find_pos and is_element_of on index_set")
{
    using S = ctv::index_set<1, 3, 5, 8, 13>;

    CHECK(ctv::find_pos<S>(1u) == 0);
    CHECK(ctv::find_pos<S>(5u) == 2);
    CHECK(ctv::find_pos<S>(13u) == 4);
    CHECK(ctv::find_pos<S>(2u) == -1);

    CHECK(ctv::find_pos(S{}, 8u) == 3);
    CHECK(ctv::find_pos(S{}, 21u) == -1);

    CHECK(ctv::is_element_of<S>(1u));
    CHECK(ctv::is_element_of<S>(8u));
    CHECK_FALSE(ctv::is_element_of<S>(21u));
}

TEST_CASE("find_pos and is_element_of on int set")
{
    using S = ctv::value_set<int, -3, 0, 4, 7>;

    CHECK(ctv::find_pos<S>(-3) == 0);
    CHECK(ctv::find_pos<S>(4) == 2);
    CHECK(ctv::find_pos<S>(6) == -1);

    CHECK(ctv::find_pos(S{}, 7) == 3);
    CHECK(ctv::is_element_of<S>(0));
    CHECK_FALSE(ctv::is_element_of<S>(9));
}

TEST_CASE("empty set operations")
{
    using E = ctv::value_set<int>;
    using A = ctv::value_set<int, 1, 2, 4>;

    CHECK(std::is_same_v<ctv::set_union_t<E, A>, A>);
    CHECK(std::is_same_v<ctv::set_union_t<A, E>, A>);

    CHECK(std::is_same_v<ctv::set_intersection_t<E, A>, E>);
    CHECK(std::is_same_v<ctv::set_intersection_t<A, E>, E>);

    CHECK(std::is_same_v<ctv::set_difference_t<E, A>, E>);
    CHECK(std::is_same_v<ctv::set_difference_t<A, E>, A>);

    CHECK(std::is_same_v<ctv::set_symmetric_difference_t<E, A>, A>);
    CHECK(std::is_same_v<ctv::set_symmetric_difference_t<A, E>, A>);
}

TEST_CASE("single element normalization and lookup")
{
    using L = ctv::value_list<int, 42>;
    using N = ctv::normalize_t<L>;

    CHECK(std::is_same_v<N, ctv::value_set<int, 42>>);
    CHECK(ctv::find_pos<N>(42) == 0);
    CHECK(ctv::find_pos<N>(7) == -1);
    CHECK(ctv::is_element_of<N>(42));
    CHECK_FALSE(ctv::is_element_of<N>(0));
}

TEST_CASE("duplicate-heavy normalization")
{
    using L = ctv::value_list<int, 4, 4, 4, 2, 2, 7, 1, 7, 1, 3>;
    using N = ctv::normalize_t<L>;
    CHECK(std::is_same_v<N, ctv::value_set<int, 1, 2, 3, 4, 7>>);
}

TEST_CASE("set operations with identical operands")
{
    using A = ctv::value_set<int, 1, 3, 5>;

    CHECK(std::is_same_v<ctv::set_union_t<A, A>, A>);
    CHECK(std::is_same_v<ctv::set_intersection_t<A, A>, A>);
    CHECK(std::is_same_v<ctv::set_difference_t<A, A>, ctv::value_set<int>>);
    CHECK(std::is_same_v<ctv::set_symmetric_difference_t<A, A>, ctv::value_set<int>>);
}

TEST_CASE("set operations with disjoint operands")
{
    using A = ctv::value_set<int, 1, 3, 5>;
    using B = ctv::value_set<int, 2, 4, 6>;

    CHECK(std::is_same_v<ctv::set_union_t<A, B>, ctv::value_set<int, 1, 2, 3, 4, 5, 6>>);
    CHECK(std::is_same_v<ctv::set_intersection_t<A, B>, ctv::value_set<int>>);
    CHECK(std::is_same_v<ctv::set_difference_t<A, B>, A>);
    CHECK(std::is_same_v<ctv::set_difference_t<B, A>, B>);
    CHECK(std::is_same_v<ctv::set_symmetric_difference_t<A, B>, ctv::value_set<int, 1, 2, 3, 4, 5, 6>>);
}