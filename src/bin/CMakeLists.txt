# Configure playground
add_executable(
    hyrisePlayground

    playground.cpp
        ../lib/operators/get_table.cpp)
target_link_libraries(
    hyrisePlayground
    hyrise
)
