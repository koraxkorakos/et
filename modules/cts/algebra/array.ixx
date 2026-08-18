module;
#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>
export module ctv.array;
export import ctv.cts_base;

namespace ctv::detail {
template <class Sink, class Value, class T, T... I>
constexpr void assign_indices(Sink &sink, Value const &value, value_set<T, I...>) {
  ((get<I>(sink) = get<I>(value)), ...);
}
} // namespace ctv::detail

export namespace ctv {
struct ArrayContext {
  template <CTS_Value E> constexpr auto operator()(E &&e) const { return std::forward<E>(e); }

  template <class T>
  constexpr auto operator()(constant_expression<T> const &value) const {
    return lower_scalar(value);
  }

  template <class T>
  constexpr auto operator()(reference_expression<T> const &value) const {
    return lower_scalar(value);
  }

  template <CTS_Sink Sink, CTS_Value Value>
  constexpr void assign(Sink &sink, Value const &value) const {
    static_assert(
        std::same_as<typename Sink::indices, typename Value::indices>,
        "CTS assignment requires identical structural support; use project "
        "explicitly when truncation is intended");
    detail::assign_indices(sink, value, typename Sink::indices{});
  }
};

/// Homogeneous sparse storage. Context is part of the type and therefore
/// controls how a generic expression is lowered on assignment/construction.
template <class Context, ValueSet Indices, class T>
struct cts_array : cts_variable_mixin<cts_array<Context, Indices, T>, Indices>,
                   expression<cts_array<Context, Indices, T>> {
  using context_type = Context;
  using indices = Indices;
  using index_type = typename indices::value_type;
  using value_type = T;
  std::array<T, indices::size> values{};

  constexpr cts_array() = default;
  constexpr explicit cts_array(std::array<T, indices::size> init) : values(std::move(init)) {}

  template <Expression E> constexpr explicit cts_array(E const &syntax) {
    auto semantic = Context{}(syntax);
    static_assert(std::same_as<typename decltype(semantic)::indices, indices>,
                  "explicit cts_array support does not match expression support");
    Context{}.assign(*this, semantic);
  }

  /// Assignment chooses semantics from this sink's Context. Because this sink
  /// already has a fixed structural support, the lowered expression must have
  /// exactly the same support.
  template <Expression E> constexpr cts_array &operator=(E const &syntax) {
    auto semantic = Context{}(syntax);
    Context{}.assign(*this, semantic);
    return *this;
  }

private:
  template <index_type I, class Self> constexpr decltype(auto) get_impl(this Self &&self) {
    constexpr auto position = find_pos<indices>(I);
    static_assert(position >= 0);
    return std::forward<Self>(self).values[static_cast<std::size_t>(position)];
  }
  friend cts_variable_mixin<cts_array, indices>;
};

template <class Context, Expression E>
[[nodiscard]] constexpr auto make_cts_array(E const &syntax) {
  auto semantic = Context{}(syntax);
  using S = decltype(semantic);
  cts_array<Context, typename S::indices, typename S::value_type> result;
  Context{}.assign(result, semantic);
  return result;
}
} // namespace ctv
