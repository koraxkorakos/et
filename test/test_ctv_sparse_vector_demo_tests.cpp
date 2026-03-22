#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

import ctv.value_set;
import ctv.sparse_vector_demo;

#include <type_traits>

TEST_CASE("sparse_vector terminal basics")
{
    using I = ctv::index_set<1, 3, 5>;
    ctv::sparse_vector<I, int> v{{10, 30, 50}};

    CHECK(v.values[0] == 10);
    CHECK(v.values[1] == 30);
    CHECK(v.values[2] == 50);

    CHECK(ctv::eval_at(1, v) == 10);
    CHECK(ctv::eval_at(3, v) == 30);
    CHECK(ctv::eval_at(5, v) == 50);
    CHECK(ctv::eval_at(7, v) == 0);
}

TEST_CASE("addition builds expression and evaluates per index")
{
    ctv::sparse_vector<ctv::index_set<1, 3, 5>, int> a{{10, 30, 50}};
    ctv::sparse_vector<ctv::index_set<3, 4, 5>, int> b{{300, 400, 500}};

    auto expr = a + b;

    using E = decltype(expr);
    CHECK(std::is_same_v<typename E::value_type, int>);

    CHECK(ctv::eval_at(1, expr) == 10);
    CHECK(ctv::eval_at(3, expr) == 330);
    CHECK(ctv::eval_at(4, expr) == 400);
    CHECK(ctv::eval_at(5, expr) == 550);
    CHECK(ctv::eval_at(9, expr) == 0);
}

TEST_CASE("subtraction builds expression and evaluates per index")
{
    ctv::sparse_vector<ctv::index_set<1, 3, 5>, int> a{{10, 30, 50}};
    ctv::sparse_vector<ctv::index_set<3, 4, 5>, int> b{{300, 400, 500}};

    auto expr = a - b;

    CHECK(ctv::eval_at(1, expr) == 10);
    CHECK(ctv::eval_at(3, expr) == -270);
    CHECK(ctv::eval_at(4, expr) == -400);
    CHECK(ctv::eval_at(5, expr) == -450);
    CHECK(ctv::eval_at(9, expr) == 0);
}

TEST_CASE("nested additive group expressions")
{
    ctv::sparse_vector<ctv::index_set<1, 3, 5>, int> a{{10, 30, 50}};
    ctv::sparse_vector<ctv::index_set<3, 4, 5>, int> b{{300, 400, 500}};
    ctv::sparse_vector<ctv::index_set<1, 4>, int> c{{7, 9}};

    auto expr = a + b - c;

    CHECK(ctv::eval_at(1, expr) == 3);
    CHECK(ctv::eval_at(3, expr) == 330);
    CHECK(ctv::eval_at(4, expr) == 391);
    CHECK(ctv::eval_at(5, expr) == 550);
    CHECK(ctv::eval_at(8, expr) == 0);
}

TEST_CASE("full_index_set of terminal")
{
    using V = ctv::sparse_vector<ctv::index_set<1, 3, 5>, int>;
    using F = ctv::full_index_set_t<V>;

    CHECK(std::is_same_v<F, ctv::index_set<1, 3, 5>>);
}

TEST_CASE("full_index_set of addition")
{
    using A = ctv::sparse_vector<ctv::index_set<1, 3, 5>, int>;
    using B = ctv::sparse_vector<ctv::index_set<3, 4, 5>, int>;
    using E = decltype(std::declval<A>() + std::declval<B>());
    using F = ctv::full_index_set_t<E>;

    CHECK(std::is_same_v<F, ctv::index_set<1, 3, 4, 5>>);
}

TEST_CASE("full_index_set of subtraction")
{
    using A = ctv::sparse_vector<ctv::index_set<1, 3, 5>, int>;
    using B = ctv::sparse_vector<ctv::index_set<3, 4, 5>, int>;
    using E = decltype(std::declval<A>() - std::declval<B>());
    using F = ctv::full_index_set_t<E>;

    CHECK(std::is_same_v<F, ctv::index_set<1, 3, 4, 5>>);
}

TEST_CASE("evaluate on explicit target support set")
{
    ctv::sparse_vector<ctv::index_set<1, 3, 5>, int> a{{10, 30, 50}};
    ctv::sparse_vector<ctv::index_set<3, 4, 5>, int> b{{300, 400, 500}};

    auto result = ctv::evaluate<ctv::index_set<1, 4, 5, 9>>(a + b);

    CHECK(result.values[0] == 10);
    CHECK(result.values[1] == 400);
    CHECK(result.values[2] == 550);
    CHECK(result.values[3] == 0);

    using R = decltype(result);
    CHECK(std::is_same_v<R, ctv::sparse_vector<ctv::index_set<1, 4, 5, 9>, int>>);
}

TEST_CASE("evaluate on full natural support")
{
    ctv::sparse_vector<ctv::index_set<1, 3, 5>, int> a{{10, 30, 50}};
    ctv::sparse_vector<ctv::index_set<3, 4, 5>, int> b{{300, 400, 500}};

    auto result = ctv::evaluate(a + b);

    using R = decltype(result);
    CHECK(std::is_same_v<R, ctv::sparse_vector<ctv::index_set<1, 3, 4, 5>, int>>);

    CHECK(result.values[0] == 10);
    CHECK(result.values[1] == 330);
    CHECK(result.values[2] == 400);
    CHECK(result.values[3] == 550);
}

TEST_CASE("project is alias for explicit evaluation")
{
    ctv::sparse_vector<ctv::index_set<1, 3, 5>, int> a{{10, 30, 50}};

    auto p = ctv::project<ctv::index_set<3, 5, 8>>(a);

    using P = decltype(p);
    CHECK(std::is_same_v<P, ctv::sparse_vector<ctv::index_set<3, 5, 8>, int>>);

    CHECK(p.values[0] == 30);
    CHECK(p.values[1] == 50);
    CHECK(p.values[2] == 0);
}

TEST_CASE("mixed support nested expression explicit projection")
{
    ctv::sparse_vector<ctv::index_set<1, 3, 5>, int> a{{10, 30, 50}};
    ctv::sparse_vector<ctv::index_set<2, 3, 8>, int> b{{20, 300, 800}};
    ctv::sparse_vector<ctv::index_set<1, 8>, int> c{{7, 9}};

    auto expr = a + b - c;
    auto result = ctv::project<ctv::index_set<1, 2, 3, 5, 8, 9>>(expr);

    CHECK(result.values[0] == 3);    // 10 + 0 - 7
    CHECK(result.values[1] == 20);   // 0 + 20 - 0
    CHECK(result.values[2] == 330);  // 30 + 300 - 0
    CHECK(result.values[3] == 50);   // 50 + 0 - 0
    CHECK(result.values[4] == 791);  // 0 + 800 - 9
    CHECK(result.values[5] == 0);    // absent
}

TEST_CASE("default constructed sparse_vector behaves as zero terminal")
{
    ctv::sparse_vector<ctv::index_set<2, 4, 7>, int> z{};

    CHECK(ctv::eval_at(2, z) == 0);
    CHECK(ctv::eval_at(4, z) == 0);
    CHECK(ctv::eval_at(7, z) == 0);
    CHECK(ctv::eval_at(9, z) == 0);
}