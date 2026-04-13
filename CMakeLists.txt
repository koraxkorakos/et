cmake_minimum_required(VERSION 3.28)

project(ctv_value_set
  VERSION 0.1.0
  DESCRIPTION "C++ Expression Template Tutorial"
  HOMEPAGE_URL https://github.com/koraxkorakos/et
  LANGUAGES CXX
)

include(CTest)

# ---- Options ----

option(CTV_BUILD_TESTS "Build unit tests" ON)
option(CTV_BUILD_BENCHMARKS "Build benchmarks" OFF)
option(CTV_ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(CTV_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

# ---- Global language settings ----

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---- Helper functions ----

function(ctv_enable_sanitizers target)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    if(CTV_ENABLE_ASAN)
      target_compile_options(${target} PRIVATE
        -fsanitize=address
        -fno-omit-frame-pointer
      )
      target_link_options(${target} PRIVATE
        -fsanitize=address
      )
    endif()

    if(CTV_ENABLE_UBSAN)
      target_compile_options(${target} PRIVATE
        -fsanitize=undefined
      )
      target_link_options(${target} PRIVATE
        -fsanitize=undefined
      )
    endif()
  endif()
endfunction()

# ---- Library: ctv_value_set ----

add_library(ctv_value_set)
add_library(ctv::value_set ALIAS ctv_value_set)

target_sources(ctv_value_set
  PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/modules/ctv/value_set.ixx
)

target_compile_features(ctv_value_set
  PUBLIC
    cxx_std_23
)

ctv_enable_sanitizers(ctv_value_set)

# ---- Library: ctv_multivector ----

add_library(ctv_multivector)
add_library(ctv::multivector ALIAS ctv_multivector)

target_sources(ctv_multivector
  PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/modules/ctv/multivector.ixx
)

target_compile_features(ctv_multivector
  PUBLIC
    cxx_std_23
)

target_link_libraries(ctv_multivector
  PUBLIC
    ctv_value_set
)

ctv_enable_sanitizers(ctv_multivector)

# ---- Library: ctv_sparse_vector_demo ----

add_library(ctv_sparse_vector_demo)
add_library(ctv::sparse_vector_demo ALIAS ctv_sparse_vector_demo)

target_sources(ctv_sparse_vector_demo
  PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/modules/ctv/sparse_vector_demo.ixx
)

target_compile_features(ctv_sparse_vector_demo
  PUBLIC
    cxx_std_23
)

target_link_libraries(ctv_sparse_vector_demo
  PUBLIC
    ctv_value_set
)

ctv_enable_sanitizers(ctv_sparse_vector_demo)

# ---- Library: ctv_special_values ----

add_library(ctv_special_values)
add_library(ctv::special_values ALIAS ctv_special_values)

target_sources(ctv_special_values
  PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/modules/ctv/special_values.ixx
)

target_compile_features(ctv_special_values
  PUBLIC
    cxx_std_23
)

ctv_enable_sanitizers(ctv_special_values)

target_link_libraries(ctv_sparse_vector_demo
  PUBLIC
    ctv_special_values
)

# ---- Tests ----

if(CTV_BUILD_TESTS)
  include(FetchContent)
  enable_testing()

  FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.12
  )

  FetchContent_MakeAvailable(doctest)

  add_executable(ctv_value_set_tests
    test/test_ctv_value_set_tests.cpp
  )

  target_link_libraries(ctv_value_set_tests
    PRIVATE
      ctv_value_set
      doctest::doctest
  )

  target_compile_features(ctv_value_set_tests
    PRIVATE
      cxx_std_23
  )

  ctv_enable_sanitizers(ctv_value_set_tests)

  add_test(
    NAME ctv_value_set_tests
    COMMAND ctv_value_set_tests
  )

  add_executable(ctv_multivector_tests
    test/test_ctv_multivector_tests.cpp
  )

  target_link_libraries(ctv_multivector_tests
    PRIVATE
      ctv_multivector
      doctest::doctest
  )

  target_compile_features(ctv_multivector_tests
    PRIVATE
      cxx_std_23
  )

  ctv_enable_sanitizers(ctv_multivector_tests)

  add_test(
    NAME ctv_multivector_tests
    COMMAND ctv_multivector_tests
  )

  add_executable(ctv_sparse_vector_demo_tests
    test/test_ctv_sparse_vector_demo_tests.cpp
  )

  target_link_libraries(ctv_sparse_vector_demo_tests
    PRIVATE
      ctv_sparse_vector_demo
      doctest::doctest
  )

  target_compile_features(ctv_sparse_vector_demo_tests
    PRIVATE
      cxx_std_23
  )

  ctv_enable_sanitizers(ctv_sparse_vector_demo_tests)

  add_test(
    NAME ctv_sparse_vector_demo_tests
    COMMAND ctv_sparse_vector_demo_tests
  )

  add_executable(ctv_special_values_tests
    test/test_ctv_special_values_tests.cpp
  )

  target_link_libraries(ctv_special_values_tests
    PRIVATE
      ctv_special_values
      doctest::doctest
  )

  target_compile_features(ctv_special_values_tests
    PRIVATE
      cxx_std_23
  )

  ctv_enable_sanitizers(ctv_special_values_tests)

  add_test(
    NAME ctv_special_values_tests
    COMMAND ctv_special_values_tests
  )
endif()

# ---- Benchmarks ----

if(CTV_BUILD_BENCHMARKS)
  include(FetchContent)

  FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.5
  )

  set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
  set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)

  FetchContent_MakeAvailable(benchmark)

  add_executable(ctv_benchmarks
    benchmark/bench_value_set.cpp
  )

  target_link_libraries(ctv_benchmarks
    PRIVATE
      ctv_value_set
      benchmark::benchmark
      benchmark::benchmark_main
  )

  target_compile_features(ctv_benchmarks
    PRIVATE
      cxx_std_23
  )
endif()

# ---- Documentation (LaTeX) ----

find_program(PDFLATEX_EXECUTABLE pdflatex)

if(PDFLATEX_EXECUTABLE)
  set(DOC_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/doc)
  set(DOC_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

  add_custom_command(
    OUTPUT ${DOC_BINARY_DIR}/presentation.pdf
    COMMAND ${PDFLATEX_EXECUTABLE}
      -interaction=nonstopmode
      -halt-on-error
      -output-directory ${DOC_BINARY_DIR}
      ${DOC_SOURCE_DIR}/presentation.tex
    WORKING_DIRECTORY ${DOC_SOURCE_DIR}
    DEPENDS ${DOC_SOURCE_DIR}/presentation.tex
    COMMENT "Building presentation.pdf"
    VERBATIM
  )

  add_custom_target(presentation
    DEPENDS ${DOC_BINARY_DIR}/presentation.pdf
  )

  add_custom_command(
    OUTPUT ${DOC_BINARY_DIR}/presentation_with_notes.pdf
    COMMAND ${PDFLATEX_EXECUTABLE}
      -interaction=nonstopmode
      -halt-on-error
      -output-directory ${DOC_BINARY_DIR}
      ${DOC_SOURCE_DIR}/presentation_with_notes.tex
    WORKING_DIRECTORY ${DOC_SOURCE_DIR}
    DEPENDS ${DOC_SOURCE_DIR}/presentation_with_notes.tex
    COMMENT "Building presentation_with_notes.pdf"
    VERBATIM
  )

  add_custom_target(presentation_with_notes
    DEPENDS ${DOC_BINARY_DIR}/presentation_with_notes.pdf
  )
else()
  message(WARNING "pdflatex not found - presentation targets disabled")
endif()
