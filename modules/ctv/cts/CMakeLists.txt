# CMakeLists.txt snippet
add_library(cts)
target_sources(cts 
    PRIVATE
    src/cts.cppm
    src/utilities.ixx
    src/expression_template.ixx
    src/indexset.ixx
    src/metric.ixx
    src/cts_array.ixx
    src/additive_group.ixx
    src/vector_space.ixx
    src/multivector.ixx
    src/grassmann.ixx
    src/orthogonal_ga.ixx
    src/symplectic_ga.ixx
)
target_compile_features(cts PUBLIC cxx_std_20)