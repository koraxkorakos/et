find_package(doctest REQUIRED)

include("${CMAKE_BINARY_DIR}/_deps/doctest-src/scripts/cmake/doctest.cmake")

message("BUILD doctests")
add_executable(ugarit_doctests)
target_compile_definitions(ugarit_doctests PUBLIC META_CONCEPT_CHECK)
target_sources(ugarit_doctests
    PRIVATE
    main.cpp
    test_core.cpp)
target_link_libraries(ugarit_doctests ugarit doctest::doctest ugarit_testutils)
doctest_discover_tests(ugarit_doctests)
