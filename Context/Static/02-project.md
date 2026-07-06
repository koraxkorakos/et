# 02-architecture.md


## Aims

  - strong type-safety
  - casting as projection
  - support C++ modules (if possible with current compilers)
  - support G++ 15.2

## Non-Aims

  - multi threading (but parallel algorithms allowed, SIMD)
  - support for runtime detection of zero elements (not compile time detected)
  - runtime polymorphic containers
  - symplectic Geometric Algebra to be prepared, but out of scope for the moment
  - support for older C++ Standard Version than 23
  - backward compatibility between versions