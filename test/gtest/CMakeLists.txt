find_package(GTest REQUIRED)

add_executable(ugarit_gtests)
target_sources(ugarit_gtests 
               PRIVATE  
               main.cpp)
target_link_libraries(ugarit_gtests ugarit_ GTest::GTest GTest::Main ugarit_testutils)
gtest_discover_tests(ugarit_gtests)