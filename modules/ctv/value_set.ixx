module;
#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <type_traits>

export module ctv.value_set;

namespace ctv::detail
{
    template <typename T, T... ns>
    constexpr bool is_strictly_sorted_values()
    {
        if constexpr (sizeof...(ns) < 2)
            return true;

        std::array<T, sizeof...(ns)> arr{ns...};
        return std::adjacent_find(
                   arr.begin(), arr.end(),
                   [](const T& a, const T& b) { return !(a < b); }) == arr.end();
    }

    template <typename T, std::size_t N>
    struct value_pack_base
    {
        using value_type = T;
        static constexpr std::size_t size = N;
    };

    template <bool>
    struct cond;

    template <>
    struct cond<true>
    {
        template <typename T, typename>
        using f = T;
    };

    template <>
    struct cond<false>
    {
        template <typename, typename T>
        using f = T;
    };

    template <bool B, typename T, typename F>
    using cond_t = typename cond<B>::template f<T, F>;
}

export namespace ctv
{
    /// \brief Raw homogeneous compile-time list.
    template <typename T, T... ns>
    struct value_list : detail::value_pack_base<T, sizeof...(ns)>
    {
    };

    /// \brief Canonical sorted unique compile-time set.
    template <typename T, T... ns>
        requires (detail::is_strictly_sorted_values<T, ns...>())
    struct value_set : detail::value_pack_base<T, sizeof...(ns)>
    {
    };

    /// \brief Convenience alias for a compile-time list of indices.
    template <std::size_t... ns>
    using index_list = value_list<std::size_t, ns...>;

    /// \brief Convenience alias for a compile-time set of indices.
    template <std::size_t... ns>
    using index_set = value_set<std::size_t, ns...>;
}

namespace ctv::detail
{
    template <typename Set, auto x>
    struct prepend_;

    template <typename T, T... ns, T x>
    struct prepend_<ctv::value_set<T, ns...>, x>
    {
        using type = ctv::value_set<T, x, ns...>;
    };

    template <typename A, typename B>
    struct union_;

    template <typename A, typename B>
    struct intersection_;

    template <typename A, typename B>
    struct difference_;

    template <typename A, typename B, auto x>
    struct prepend_union_
    {
        using type = typename prepend_<typename union_<A, B>::type, x>::type;
    };

    template <typename A, typename B, auto x>
    struct prepend_intersection_
    {
        using type = typename prepend_<typename intersection_<A, B>::type, x>::type;
    };

    template <typename A, typename B, auto x>
    struct prepend_difference_
    {
        using type = typename prepend_<typename difference_<A, B>::type, x>::type;
    };
}

export namespace ctv
{
    /// \brief Set-theoretic union of two canonical value_set types.
    template <typename A, typename B>
    struct set_union;

    /// \brief Alias for the union of two canonical value_set types.
    template <typename A, typename B>
    using set_union_t = typename set_union<A, B>::type;

    /// \brief Set-theoretic intersection of two canonical value_set types.
    template <typename A, typename B>
    struct set_intersection;

    /// \brief Alias for the intersection of two canonical value_set types.
    template <typename A, typename B>
    using set_intersection_t = typename set_intersection<A, B>::type;

    /// \brief Set-theoretic difference A \ B of two canonical value_set types.
    template <typename A, typename B>
    struct set_difference;

    /// \brief Alias for the difference A \ B of two canonical value_set types.
    template <typename A, typename B>
    using set_difference_t = typename set_difference<A, B>::type;

    /// \brief Set-theoretic symmetric difference of two canonical value_set types.
    template <typename A, typename B>
    struct set_symmetric_difference
    {
        using type = set_union_t<
            set_difference_t<A, B>,
            set_difference_t<B, A>>;
    };

    /// \brief Alias for the symmetric difference of two canonical value_set types.
    template <typename A, typename B>
    using set_symmetric_difference_t =
        typename set_symmetric_difference<A, B>::type;
}

namespace ctv::detail
{
    template <typename A, typename B>
    struct union_
    {
        using type = typename ctv::set_union<A, B>::type;
    };

    template <typename A, typename B>
    struct intersection_
    {
        using type = typename ctv::set_intersection<A, B>::type;
    };

    template <typename A, typename B>
    struct difference_
    {
        using type = typename ctv::set_difference<A, B>::type;
    };
}

export namespace ctv
{
    template <typename T>
    struct set_union<value_set<T>, value_set<T>>
    {
        using type = value_set<T>;
    };

    template <typename T, T... bs>
    struct set_union<value_set<T>, value_set<T, bs...>>
    {
        using type = value_set<T, bs...>;
    };

    template <typename T, T... as>
    struct set_union<value_set<T, as...>, value_set<T>>
    {
        using type = value_set<T, as...>;
    };

    template <typename T, T a0, T... as, T b0, T... bs>
    struct set_union<value_set<T, a0, as...>, value_set<T, b0, bs...>>
    {
    private:
        using left_tail  = value_set<T, as...>;
        using right_tail = value_set<T, bs...>;
        using left_full  = value_set<T, a0, as...>;
        using right_full = value_set<T, b0, bs...>;

        using selected = detail::cond_t<
            (a0 < b0),
            detail::prepend_union_<left_tail, right_full, a0>,
            detail::cond_t<
                (b0 < a0),
                detail::prepend_union_<left_full, right_tail, b0>,
                detail::prepend_union_<left_tail, right_tail, a0>>>;

    public:
        using type = typename selected::type;
    };

    template <typename T>
    struct set_intersection<value_set<T>, value_set<T>>
    {
        using type = value_set<T>;
    };

    template <typename T, T... bs>
    struct set_intersection<value_set<T>, value_set<T, bs...>>
    {
        using type = value_set<T>;
    };

    template <typename T, T... as>
    struct set_intersection<value_set<T, as...>, value_set<T>>
    {
        using type = value_set<T>;
    };

    template <typename T, T a0, T... as, T b0, T... bs>
    struct set_intersection<value_set<T, a0, as...>, value_set<T, b0, bs...>>
    {
    private:
        using left_tail  = value_set<T, as...>;
        using right_tail = value_set<T, bs...>;
        using left_full  = value_set<T, a0, as...>;
        using right_full = value_set<T, b0, bs...>;

        using selected = detail::cond_t<
            (a0 < b0),
            detail::intersection_<left_tail, right_full>,
            detail::cond_t<
                (b0 < a0),
                detail::intersection_<left_full, right_tail>,
                detail::prepend_intersection_<left_tail, right_tail, a0>>>;

    public:
        using type = typename selected::type;
    };

    template <typename T>
    struct set_difference<value_set<T>, value_set<T>>
    {
        using type = value_set<T>;
    };

    template <typename T, T... bs>
    struct set_difference<value_set<T>, value_set<T, bs...>>
    {
        using type = value_set<T>;
    };

    template <typename T, T... as>
    struct set_difference<value_set<T, as...>, value_set<T>>
    {
        using type = value_set<T, as...>;
    };

    template <typename T, T a0, T... as, T b0, T... bs>
    struct set_difference<value_set<T, a0, as...>, value_set<T, b0, bs...>>
    {
    private:
        using left_tail  = value_set<T, as...>;
        using right_tail = value_set<T, bs...>;
        using left_full  = value_set<T, a0, as...>;
        using right_full = value_set<T, b0, bs...>;

        using selected = detail::cond_t<
            (a0 < b0),
            detail::prepend_difference_<left_tail, right_full, a0>,
            detail::cond_t<
                (b0 < a0),
                detail::difference_<left_full, right_tail>,
                detail::difference_<left_tail, right_tail>>>;

    public:
        using type = typename selected::type;
    };

    /// \brief Find the zero-based position of a value in a canonical value_set, or -1 if absent.
    template <typename T, T... ns, typename U>
        requires std::convertible_to<U, T>
    constexpr std::ptrdiff_t find_pos(value_set<T, ns...>, U value)
    {
        constexpr std::array<T, sizeof...(ns)> arr{ns...};
        T v = static_cast<T>(value);

        auto it = std::lower_bound(arr.begin(), arr.end(), v);
        if (it == arr.end() || *it != v)
            return -1;

        return static_cast<std::ptrdiff_t>(it - arr.begin());
    }

    /// \brief Find the zero-based position of a value in Set, or -1 if absent.
    template <typename Set, typename U>
        requires std::convertible_to<U, typename Set::value_type>
    constexpr std::ptrdiff_t find_pos(U value)
    {
        return find_pos(Set{}, value);
    }

    /// \brief Test whether a value is contained in a canonical value_set.
    template <typename Set, typename U>
        requires std::convertible_to<U, typename Set::value_type>
    constexpr bool is_element_of(U value)
    {
        return find_pos<Set>(value) >= 0;
    }

    /// \brief Normalize a value_list into a canonical value_set.
    template <typename List>
    struct normalize;

    template <typename T>
    struct normalize<value_list<T>>
    {
        using type = value_set<T>;
    };

    template <typename T, T x>
    struct normalize<value_list<T, x>>
    {
        using type = value_set<T, x>;
    };

    template <typename T, T x, T... rest>
    struct normalize<value_list<T, x, rest...>>
    {
    private:
        using rest_set = typename normalize<value_list<T, rest...>>::type;

    public:
        using type = set_union_t<rest_set, value_set<T, x>>;
    };

    /// \brief Alias for the normalized canonical value_set of a value_list.
    template <typename List>
    using normalize_t = typename normalize<List>::type;

    /// \brief Union of two raw value_list types via normalization.
    template <typename A, typename B>
    struct list_union;

    template <typename T, T... as, T... bs>
    struct list_union<value_list<T, as...>, value_list<T, bs...>>
    {
        using type = normalize_t<value_list<T, as..., bs...>>;
    };

    /// \brief Alias for the normalized union of two raw value_list types.
    template <typename A, typename B>
    using list_union_t = typename list_union<A, B>::type;
}