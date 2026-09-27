module;
#include <concepts>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

export module ctv.multivector;

import ctv.value_set;

namespace ctv::detail
{
    template <typename Storage, typename E>
    constexpr Storage evaluate_multivector_expression(E const& expression);

    template <typename Set, auto Index>
    struct index_position;

    template <typename T, T Index>
    struct index_position<ctv::value_set<T>, Index>
    {
        static constexpr std::size_t value = static_cast<std::size_t>(-1);
    };

    template <typename T, T Head, T... Tail, T Index>
    struct index_position<ctv::value_set<T, Head, Tail...>, Index>
    {
    private:
        static constexpr std::size_t tail_value =
            index_position<ctv::value_set<T, Tail...>, Index>::value;

    public:
        static constexpr std::size_t value =
            Index == Head ? 0 :
            tail_value == static_cast<std::size_t>(-1) ? tail_value :
            1 + tail_value;
    };
}

export namespace ctv
{
    template <typename IndexSet, typename... Ts>
    struct multivector;

    template <typename T, T... Indices, typename... Ts>
        requires (sizeof...(Indices) == sizeof...(Ts))
    struct multivector<value_set<T, Indices...>, Ts...>
    {
        using index_set = value_set<T, Indices...>;
        using index_type = T;
        using storage_type = std::tuple<Ts...>;

        static constexpr std::size_t size = sizeof...(Ts);

        storage_type values{};

        constexpr multivector() = default;

        constexpr explicit multivector(Ts... init)
            : values(std::move(init)...)
        {
        }

        constexpr explicit multivector(storage_type init)
            : values(std::move(init))
        {
        }

        /// Materialize an expression with the same populated indices.  The
        /// coefficient types are part of this specialization and are deduced
        /// by the deduction guide below from the (unevaluated) get calls.
        template <typename E>
            requires (!std::same_as<std::remove_cvref_t<E>, multivector>) &&
                     requires { typename std::remove_cvref_t<E>::index_set; } &&
                     std::same_as<
                         typename std::remove_cvref_t<E>::index_set,
                         index_set>
        constexpr explicit multivector(E const& expression)
            : values(detail::evaluate_multivector_expression<storage_type>(expression))
        {
        }
    };

    template <auto Index, typename T, T... Indices, typename... Ts>
    constexpr decltype(auto) get(multivector<value_set<T, Indices...>, Ts...>& v)
    {
        static_assert(std::same_as<std::remove_cv_t<decltype(Index)>, T>,
            "ctv::get<Index>(multivector): index type does not match multivector::index_type");

        constexpr std::size_t pos =
            detail::index_position<value_set<T, Indices...>, Index>::value;

        static_assert(pos != static_cast<std::size_t>(-1),
            "ctv::get<Index>(multivector): index is not present in the multivector");

        return std::get<pos>(v.values);
    }

    template <auto Index, typename T, T... Indices, typename... Ts>
    constexpr decltype(auto) get(multivector<value_set<T, Indices...>, Ts...> const& v)
    {
        static_assert(std::same_as<std::remove_cv_t<decltype(Index)>, T>,
            "ctv::get<Index>(multivector): index type does not match multivector::index_type");

        constexpr std::size_t pos =
            detail::index_position<value_set<T, Indices...>, Index>::value;

        static_assert(pos != static_cast<std::size_t>(-1),
            "ctv::get<Index>(multivector): index is not present in the multivector");

        return std::get<pos>(v.values);
    }

    template <auto Index, typename T, T... Indices, typename... Ts>
    constexpr decltype(auto) get(multivector<value_set<T, Indices...>, Ts...>&& v)
    {
        static_assert(std::same_as<std::remove_cv_t<decltype(Index)>, T>,
            "ctv::get<Index>(multivector): index type does not match multivector::index_type");

        constexpr std::size_t pos =
            detail::index_position<value_set<T, Indices...>, Index>::value;

        static_assert(pos != static_cast<std::size_t>(-1),
            "ctv::get<Index>(multivector): index is not present in the multivector");

        return std::get<pos>(std::move(v.values));
    }

    template <auto Index, typename T, T... Indices, typename... Ts>
    constexpr decltype(auto) get(multivector<value_set<T, Indices...>, Ts...> const&& v)
    {
        static_assert(std::same_as<std::remove_cv_t<decltype(Index)>, T>,
            "ctv::get<Index>(multivector): index type does not match multivector::index_type");

        constexpr std::size_t pos =
            detail::index_position<value_set<T, Indices...>, Index>::value;

        static_assert(pos != static_cast<std::size_t>(-1),
            "ctv::get<Index>(multivector): index is not present in the multivector");

        return std::get<pos>(std::move(v.values));
    }
}

namespace ctv::detail
{
    template <typename E, auto Index>
    using evaluated_coefficient_t =
        std::remove_cvref_t<decltype(get<Index>(std::declval<E const&>()))>;

    template <typename E, typename T, T... Indices>
    constexpr auto evaluate_multivector_expression_impl(
        E const& expression,
        ctv::value_set<T, Indices...>)
    {
        return std::tuple<evaluated_coefficient_t<E, Indices>...>{
            get<Indices>(expression)...};
    }

    template <typename Storage, typename E>
    constexpr Storage evaluate_multivector_expression(E const& expression)
    {
        using index_set = typename std::remove_cvref_t<E>::index_set;
        return evaluate_multivector_expression_impl(expression, index_set{});
    }

    template <typename E, typename IndexSet>
    struct expression_coefficients;

    template <typename E, typename T, T... Indices>
    struct expression_coefficients<E, ctv::value_set<T, Indices...>>
    {
        using multivector_type = ctv::multivector<
            ctv::value_set<T, Indices...>,
            evaluated_coefficient_t<E, Indices>...>;
    };
}

export namespace ctv
{
    /// CTAD uses one internal descriptor because C++ deduction-guide return
    /// types cannot expand a pack computed by a metafunction.  Inherit the
    /// ordinary, fully expanded specialization so the public interface and
    /// storage behavior remain identical.
    template <typename T, T... Indices, typename E>
    struct multivector<
        value_set<T, Indices...>,
        detail::expression_coefficients<E, value_set<T, Indices...>>>
        : detail::expression_coefficients<
              E, value_set<T, Indices...>>::multivector_type
    {
        using base_type = typename detail::expression_coefficients<
            E, value_set<T, Indices...>>::multivector_type;
        using base_type::base_type;
    };

    template <auto Index, typename T, T... Indices, typename E>
    constexpr decltype(auto) get(multivector<
        value_set<T, Indices...>,
        detail::expression_coefficients<E, value_set<T, Indices...>>>& v)
    {
        using multivector_type = std::remove_reference_t<decltype(v)>;
        return get<Index>(static_cast<typename multivector_type::base_type&>(v));
    }

    template <auto Index, typename T, T... Indices, typename E>
    constexpr decltype(auto) get(multivector<
        value_set<T, Indices...>,
        detail::expression_coefficients<E, value_set<T, Indices...>>> const& v)
    {
        using multivector_type = std::remove_reference_t<decltype(v)>;
        return get<Index>(static_cast<typename multivector_type::base_type const&>(v));
    }

    template <auto Index, typename T, T... Indices, typename E>
    constexpr decltype(auto) get(multivector<
        value_set<T, Indices...>,
        detail::expression_coefficients<E, value_set<T, Indices...>>>&& v)
    {
        using multivector_type = std::remove_reference_t<decltype(v)>;
        return get<Index>(static_cast<typename multivector_type::base_type&&>(v));
    }

    template <auto Index, typename T, T... Indices, typename E>
    constexpr decltype(auto) get(multivector<
        value_set<T, Indices...>,
        detail::expression_coefficients<E, value_set<T, Indices...>>> const&& v)
    {
        using multivector_type = std::remove_reference_t<decltype(v)>;
        return get<Index>(static_cast<typename multivector_type::base_type const&&>(v));
    }

    /// Deduce both structural support and each stored coefficient type without
    /// evaluating the expression.  get<I>(expression) is used only in
    /// decltype here; actual evaluation happens in the converting constructor.
    template <typename E>
        requires requires { typename std::remove_cvref_t<E>::index_set; }
    multivector(E const&) -> multivector<
        typename std::remove_cvref_t<E>::index_set,
        detail::expression_coefficients<
            std::remove_cvref_t<E>,
            typename std::remove_cvref_t<E>::index_set>>;
}

namespace std
{
    template <typename T, T... Indices, typename... Ts>
    struct tuple_size<ctv::multivector<ctv::value_set<T, Indices...>, Ts...>>
        : integral_constant<std::size_t, sizeof...(Ts)>
    {
    };

    template <std::size_t I, typename T, T... Indices, typename... Ts>
    struct tuple_element<I, ctv::multivector<ctv::value_set<T, Indices...>, Ts...>>
    {
        using type = tuple_element_t<I, tuple<Ts...>>;
    };

    template <typename T, T... Indices, typename E>
    struct tuple_size<ctv::multivector<
        ctv::value_set<T, Indices...>,
        ctv::detail::expression_coefficients<
            E, ctv::value_set<T, Indices...>>>>
        : tuple_size<typename ctv::multivector<
              ctv::value_set<T, Indices...>,
              ctv::detail::expression_coefficients<
                  E, ctv::value_set<T, Indices...>>>::base_type>
    {
    };

    template <std::size_t I, typename T, T... Indices, typename E>
    struct tuple_element<I, ctv::multivector<
        ctv::value_set<T, Indices...>,
        ctv::detail::expression_coefficients<
            E, ctv::value_set<T, Indices...>>>>
        : tuple_element<I, typename ctv::multivector<
              ctv::value_set<T, Indices...>,
              ctv::detail::expression_coefficients<
                  E, ctv::value_set<T, Indices...>>>::base_type>
    {
    };
}
