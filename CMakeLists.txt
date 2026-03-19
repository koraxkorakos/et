cmake_minimum_required(VERSION 3.30...3.30.20240621-gb83fabd)
project(et_project
        VERSION 0.0.0.1
        DESCRIPTION "C++ Expression Template Tutorial"
        HOMEPAGE_URL https://github.com/koraxkorakos/et
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)     
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)
# -----------------------------------------------------------------------------
option(ET_DOCTESTS "Build doctest unit tests" On)
option(ET_GTESTS "Build googletest unit tests" On)
option(ET_DOXYDOC "Build source documentation" Off)
option(ET_BUILD_THE_MANUAL "Build the manual" On)

set(ET_NUM_BASES "64" CACHE STRING "Number of of bases vectors")

add_definitions("-D${ET_NUM_BASES}")

#find_package(Boost, REQUIRED program_options system filesystem exception)
#find_package(Eigen3 REQUIRED NO_MODULE)

if(${ET_DOCTESTS})
    set (ET_UNITTESTS On)
elseif(${et_GTESTS})
    set (ET_UNITTESTS On)
else()
    set (ET_UNITTESTS Off)
endif()

include(FetchContent)
include(ExternalProject)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        main
)

FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG        master
)

if(${ET_GTESTS})
    FetchContent_MakeAvailable(googletest)
endif()

if(${ET_DOCTESTS})
    FetchContent_MakeAvailable(doctest)
endif()

# -----------------------------------------------------------------------------
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Ensure the header unit is built before the main target

add_library(et STATIC)

target_sources(et
               PUBLIC 
               src/et.cpp)
           
target_compile_options(et
               PUBLIC
)

target_include_directories(et
                           SYSTEM
                           PUBLIC
                           include
)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
add_executable(et_main src/main.cpp)
target_link_libraries(et_main  et)              

if (${ET_UNITTESTS})     
    enable_testing()
    add_subdirectory(test)
    message("Building unit tests")
endif()

# -----------------------------------------------------------------------------
if(${ET_DOXYDOC})
    find_package(Doxygen
                 REQUIRED dot
                 OPTIONAL_COMPONENTS mscgen dia)

    doxygen_add_docs(etdox)
endif()

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if (ET_BUILD_THE_MANUAL)
#    add_subdirectory(doc)
#endif()
