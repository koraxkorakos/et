cmake_minimum_required(VERSION 3.28)

project(ctv_value_set
    VERSION 0.1.0
    DESCRIPTION "C++ Expression Template Tutorial"
    HOMEPAGE_URL https://github.com/koraxkorakos/et
    LANGUAGES CXX
)

include(CTest)

option(CTV_BUILD_TESTS "Build unit tests" ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(ctv_value_set)
add_library(ctv::value_set ALIAS ctv_value_set)

target_sources(ctv_value_set
    PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/modules/ctv/value_set.ixx
)

target_compile_features(ctv_value_set PUBLIC cxx_std_23)

add_library(ctv_sparse_vector_demo)
add_library(ctv::sparse_vector_demo ALIAS ctv_sparse_vector_demo)

target_sources(ctv_sparse_vector_demo
    PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/modules/ctv/sparse_vector_demo.ixx
)

target_compile_features(ctv_sparse_vector_demo PUBLIC cxx_std_23)

target_link_libraries(ctv_sparse_vector_demo
    PUBLIC
        ctv_value_set
)

if(CTV_BUILD_TESTS)
    include(FetchContent)

    FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG v2.4.12
    )

    FetchContent_MakeAvailable(doctest)

    add_executable(ctv_value_set_tests
        test/test_ctv_value_set_tests.cpp
    )

    add_executable(ctv_sparse_vector_demo_tests
        test/test_ctv_sparse_vector_demo_tests.cpp
    )

    target_link_libraries(ctv_value_set_tests
        PRIVATE
            ctv_value_set
            doctest::doctest
    )

    target_link_libraries(ctv_sparse_vector_demo_tests
        PRIVATE
            ctv_sparse_vector_demo
            doctest::doctest
    )

    target_compile_features(ctv_value_set_tests PRIVATE cxx_std_23)
    target_compile_features(ctv_sparse_vector_demo_tests PRIVATE cxx_std_23)

    add_test(NAME ctv_value_set_tests COMMAND ctv_value_set_tests)
    add_test(NAME ctv_sparse_vector_demo_tests COMMAND ctv_sparse_vector_demo_tests)
endif()