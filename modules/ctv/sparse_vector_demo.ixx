module;
#include <array>
#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>

export module ctv.sparse_vector_demo;

import ctv.value_set;

export namespace ctv
{
    /// \brief Minimal expression concept for the talk demo.
    template <typename E>
    concept expression =
        requires {
            typename E::is_expression_tag;
            typename E::value_type;
        };

    /// \brief Array-based sparse vector terminal for the demo.
    template <typename IndexSet, typename T>
    struct sparse_vector
    {
        using is_expression_tag = void;
        using index_set = IndexSet;
        using value_type = T;

        std::array<T, IndexSet::size> values{};

        constexpr sparse_vector() = default;

        constexpr explicit sparse_vector(std::array<T, IndexSet::size> init)
            : values(init)
        {
        }

        constexpr T const& operator[](std::size_t pos) const
        {
            return values[pos];
        }

        constexpr T& operator[](std::size_t pos)
        {
            return values[pos];
        }
    };

    //--------------------------------------------------------------------------
    // NOTE ON VALUE SEMANTICS
    //
    // Expression nodes in this tutorial/demo store operands by value.
    // This is intentional:
    //   - temporaries are always safe
    //   - the core idea stays easy to explain
    //   - common lifetime mistakes are avoided
    //
    // A production implementation would usually use a closure strategy:
    //   - lvalues by reference (e.g. reference_wrapper)
    //   - rvalues by value (move)
    //
    // That optimization layer is orthogonal to the sparse evaluation idea.
    //--------------------------------------------------------------------------

    /// \brief Sum expression node.
    template <expression L, expression R>
    struct add_expr
    {
        using is_expression_tag = void;
        using value_type = std::common_type_t<typename L::value_type, typename R::value_type>;

        L l;
        R r;
    };

    /// \brief Difference expression node.
    template <expression L, expression R>
    struct sub_expr
    {
        using is_expression_tag = void;
        using value_type = std::common_type_t<typename L::value_type, typename R::value_type>;

        L l;
        R r;
    };

    template <expression L, expression R>
    constexpr auto operator+(L l, R r)
    {
        return add_expr<L, R>{l, r};
    }

    template <expression L, expression R>
    constexpr auto operator-(L l, R r)
    {
        return sub_expr<L, R>{l, r};
    }

    /// \brief Compute the natural support set of an expression.
    template <typename Expr>
    struct full_index_set;

    template <typename Expr>
    using full_index_set_t = typename full_index_set<Expr>::type;
}

namespace ctv::detail
{
    template <typename T, T... ns, typename F, std::size_t... Is>
    constexpr void for_each_index_impl(value_set<T, ns...>, F&& f, std::index_sequence<Is...>)
    {
        (f(std::integral_constant<T, ns>{}, std::integral_constant<std::size_t, Is>{}), ...);
    }

    template <typename Set, typename F>
    constexpr void for_each_index(F&& f)
    {
        for_each_index_impl(Set{}, std::forward<F>(f), std::make_index_sequence<Set::size>{});
    }
}

export namespace ctv
{
    template <typename IndexSet, typename T>
    struct full_index_set<sparse_vector<IndexSet, T>>
    {
        using type = IndexSet;
    };

    template <expression L, expression R>
    struct full_index_set<add_expr<L, R>>
    {
        using type = ctv::set_union_t<full_index_set_t<L>, full_index_set_t<R>>;
    };

    template <expression L, expression R>
    struct full_index_set<sub_expr<L, R>>
    {
        using type = ctv::set_union_t<full_index_set_t<L>, full_index_set_t<R>>;
    };

    /// \brief Evaluate one logical index of a sparse vector terminal.
    /// Missing indices are treated as zero.
    template <typename IndexSet, typename T, typename U>
        requires std::convertible_to<U, typename IndexSet::value_type>
    constexpr T eval_at(U i, sparse_vector<IndexSet, T> const& v)
    {
        auto pos = ctv::find_pos<IndexSet>(i);
        return pos >= 0 ? v.values[static_cast<std::size_t>(pos)] : T{};
    }

    /// \brief Evaluate one logical index of a sum expression.
    template <typename U, expression L, expression R>
    constexpr auto eval_at(U i, add_expr<L, R> const& e)
    {
        return eval_at(i, e.l) + eval_at(i, e.r);
    }

    /// \brief Evaluate one logical index of a difference expression.
    template <typename U, expression L, expression R>
    constexpr auto eval_at(U i, sub_expr<L, R> const& e)
    {
        return eval_at(i, e.l) - eval_at(i, e.r);
    }

    /// \brief Materialize an expression on an explicitly requested target support set.
    template <typename IndexSet, expression Expr>
    constexpr auto evaluate(Expr const& e)
    {
        using T = typename Expr::value_type;
        sparse_vector<IndexSet, T> out{};

        detail::for_each_index<IndexSet>([&](auto index_c, auto pos_c) {
            constexpr auto index = decltype(index_c)::value;
            constexpr auto pos   = decltype(pos_c)::value;
            out.values[pos] = eval_at(index, e);
        });

        return out;
    }

    /// \brief Materialize an expression on its full natural support set.
    template <expression Expr>
    constexpr auto evaluate(Expr const& e)
    {
        return evaluate<full_index_set_t<Expr>>(e);
    }

    /// \brief Projection onto an explicitly requested support set.
    template <typename TargetSet, expression Expr>
    constexpr auto project(Expr const& e)
    {
        return evaluate<TargetSet>(e);
    }
}
