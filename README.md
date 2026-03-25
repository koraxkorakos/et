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


├── bin
├── doc
|   ├── presentation.tex
|   └── presentation_with_notes.tex
├── lib
├── modules
|   ├── sparse_vector_demo.ixx
|   └── value_set.cpp
├── test
|   ├── test_ctv_sparse_vector_demo_tests.cpp
|   └── test_ctv_value_set_tests.cpp
├── .gitignore
├── CMakeLists.txt
├── LICENSE
└── Readme.md


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

## Building the Presentation Slides

From the `build directory` issue: 

```bash
ninja presentation
ninja presentation_with_notes
```

This will build the presentation slides in the `build` directory. 
It requires that you have `pdflatex` installed along with the LaTeX packages used. 

## Notes

- The module interface unit are: 

  `ctv.value_set.ixx`
  `sparse_vector_demo.ixx`

- The library target are:

  `ctv.value_set`
  `ctv.sparse_vector_demo`

- Namespaced alias:

  `ctv.value_set`
  `ctv.sparse_vector_demo`

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