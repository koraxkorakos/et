// cts.cppm
export module cts;

// Re-export all partitions
export import :utilities;
export import :expression_template;
export import :valueset;
export import :metric;
export import :cts_array;
export import :additive_group;
export import :vector_space;
export import :multivector;
export import :grassmann;
export import :orthogonal_ga;
export import :symplectic_ga;

// Any common functionality can go here
export namespace cts {
// Version information, common types, etc.
inline constexpr int version_major = 1;
inline constexpr int version_minor = 0;
} // namespace cts