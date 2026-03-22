# ctv.value_set — minimal CMake setup

A small C++23 compile-time library using C++ modules and doctest.

---

## Requirements

- **CMake ≥ 3.28** (required for C++ modules support)
- A compiler with working **C++20/23 modules support**
  - GCC ≥ 13 (experimental)
  - Clang ≥ 16 (experimental)
  - MSVC ≥ 19.36 (best support currently)
- **Ninja** recommended (but not strictly required)

---

# ctv.value_set — minimal CMake setup

A small C++23 compile-time library using C++ modules and doctest.

---

## Requirements

- **CMake ≥ 3.28** (required for C++ modules support)
- A compiler with working **C++20/23 modules support**
  - GCC ≥ 13 (experimental)
  - Clang ≥ 16 (experimental)
  - MSVC ≥ 19.36 (best support currently)
- **Ninja** recommended (but not strictly required)

---

## Project structure

├── CMakeLists.txt
├── ctv.value_set.ixx
└── test_ctv_value_set.cpp


## Build & run tests

```bash
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure
```

## Disable Tests

```bash
cmake -S . -B build -G Ninja -DCTV_BUILD_TESTS=OFF
```

## Notes

- The module interface unit is: 

  `ctv.value_set.ixx`

- The library target is:

  `ctv.value_set`

- Namespaced alias:

  `ctv.value_set`

Tests use doctest, fetched automatically via CMake FetchContent.

## Tips (important for modules)

- Prefer Ninja generator → best dependency handling

- If you see strange build order errors:

  `rm -rf build`

  and reconfigure

- Modules are still evolving — MSVC currently has the smoothest experience

## Next steps (optional)

You can extend this structure with:

- `ctv.set_algorithms`

- `ctv.clifford`

- `ctv.tensor`