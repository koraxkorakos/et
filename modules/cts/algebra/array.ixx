// cts_array.ixx
module;

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

export module ctv.array;

import export ctv.expression_template;
import export ctv.value_set;
import export ctv.special_values;
import export ctv.cts_concepts;

namespace ctv {

namespace detail {
template <typename Set, auto Index> struct index_position;

template <typename T, T Index> struct index_position<ctv::value_set<T>, Index> {
  static constexpr std::size_t value = static_cast<std::size_t>(-1);
};

template <typename T, T Head, T... Tail, T Index>
struct index_position<ctv::value_set<T, Head, Tail...>, Index> {
private:
  static constexpr std::size_t tail_value =
      index_position<ctv::value_set<T, Tail...>, Index>::value;

public:
  static constexpr std::size_t value =
      Index == Head                                ? 0
      : tail_value == static_cast<std::size_t>(-1) ? tail_value
                                                   : 1 + tail_value;
};
} // namespace ctv::detail

export namespace ctv {

template <typename IndexSet, typename... Ts> struct multivector;

template <typename T, T... Indices, typename... Ts>
  requires(sizeof...(Indices) == sizeof...(Ts))
struct multivector<value_set<T, Indices...>, Ts...> {
  using indices = value_set<T, Indices...>;
  using index_type = T;
  using storage_type = std::tuple<Ts...>;

  static constexpr std::size_t size = sizeof...(Ts);

  storage_type values{};

  constexpr multivector() = default;

  constexpr explicit multivector(Ts... init) : values(std::move(init)...) {}

  constexpr explicit multivector(storage_type init) : values(std::move(init)) {}
};

template <auto Index, typename T, T... Indices, typename... Ts>
constexpr decltype(auto) get(multivector<value_set<T, Indices...>, Ts...> &v) {
  static_assert(std::same_as<std::remove_cv_t<decltype(Index)>, T>,
                "ctv::get<Index>(multivector): index type does not match "
                "multivector::index_type");

  constexpr std::size_t pos =
      detail::index_position<value_set<T, Indices...>, Index>::value;

  static_assert(
      pos != static_cast<std::size_t>(-1),
      "ctv::get<Index>(multivector): index is not present in the multivector");

  return std::get<pos>(v.values);
}

template <auto Index, typename T, T... Indices, typename... Ts>
constexpr decltype(auto)
get(multivector<value_set<T, Indices...>, Ts...> const &v) {
  static_assert(std::same_as<std::remove_cv_t<decltype(Index)>, T>,
                "ctv::get<Index>(multivector): index type does not match "
                "multivector::index_type");

  constexpr std::size_t pos =
      detail::index_position<value_set<T, Indices...>, Index>::value;

  static_assert(
      pos != static_cast<std::size_t>(-1),
      "ctv::get<Index>(multivector): index is not present in the multivector");

  return std::get<pos>(v.values);
}

template <auto Index, typename T, T... Indices, typename... Ts>
constexpr decltype(auto) get(multivector<value_set<T, Indices...>, Ts...> &&v) {
  static_assert(std::same_as<std::remove_cv_t<decltype(Index)>, T>,
                "ctv::get<Index>(multivector): index type does not match "
                "multivector::index_type");

  constexpr std::size_t pos =
      detail::index_position<value_set<T, Indices...>, Index>::value;

  static_assert(
      pos != static_cast<std::size_t>(-1),
      "ctv::get<Index>(multivector): index is not present in the multivector");

  return std::get<pos>(std::move(v.values));
}

template <auto Index, typename T, T... Indices, typename... Ts>
constexpr decltype(auto)
get(multivector<value_set<T, Indices...>, Ts...> const &&v) {
  static_assert(std::same_as<std::remove_cv_t<decltype(Index)>, T>,
                "ctv::get<Index>(multivector): index type does not match "
                "multivector::index_type");

  constexpr std::size_t pos =
      detail::index_position<value_set<T, Indices...>, Index>::value;

  static_assert(
      pos != static_cast<std::size_t>(-1),
      "ctv::get<Index>(multivector): index is not present in the multivector");

  return std::get<pos>(std::move(v.values));
}

struct ArrayContext {
  template <CTS_sink Sink, CTS_Value Value>
  inline constexpr void assign(Sink &ref, Value const &value) {
    assign_impl(ref, value, typename Sink::indices{});
  }

private:
  template <CTS_sink Sink, CTS_Value Value, typename T, T t, T... ts>
  inline constexpr void assign_impl(Sink &ref, Value const &value,
                                    value_set<T, t, ts...>) {
    get<t>(ref) = get<t>(value);
    if constexpr (sizeof...(ts))
      assign_impl<Sink, Value, Context, Ts...>(ref, value, ctx, ts...);
  }
  template <CTS_sink Sink, CTS_Value Value, typename T>
  inline constexpr void assign_impl(Sink &, Value const &, value_set<T>) {}
};
} // namespace ctv

namespace std {
template <typename T, T... Indices, typename... Ts>
struct tuple_size<ctv::multivector<ctv::value_set<T, Indices...>, Ts...>>
    : integral_constant<std::size_t, sizeof...(Ts)> {};

template <std::size_t I, typename T, T... Indices, typename... Ts>
struct tuple_element<I,
                     ctv::multivector<ctv::value_set<T, Indices...>, Ts...>> {
  using type = tuple_element_t<I, tuple<Ts...>>;
};
} // namespace std
