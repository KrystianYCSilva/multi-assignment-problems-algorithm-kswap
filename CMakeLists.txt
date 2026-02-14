cmake_minimum_required(VERSION 3.16)
project(mapa_kswap C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

set(LIB_SOURCES
    src/core/schedule.c
    src/core/cost.c
    src/solvers/lap_solver.c
    src/heuristics/constructive.c
    src/heuristics/pcr.c
    src/heuristics/kswap.c
    src/heuristics/vns.c
    src/heuristics/vnd.c
    src/heuristics/shift_relocate.c
    src/heuristics/swap_exchange.c
    src/heuristics/two_opt.c
    src/heuristics/operator_registry.c
    src/io/nsp_reader.c
    src/io/result_writer.c
    src/benchmark/benchmark.c
)

add_library(mapa_lib STATIC ${LIB_SOURCES})
target_include_directories(mapa_lib PUBLIC ${CMAKE_SOURCE_DIR}/src)
if(UNIX)
    target_link_libraries(mapa_lib PUBLIC m)
endif()

add_executable(mapa_kswap src/main.c)
target_link_libraries(mapa_kswap PRIVATE mapa_lib)

enable_testing()

add_executable(test_schedule tests/test_schedule.c)
target_include_directories(test_schedule PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/tests)
target_link_libraries(test_schedule PRIVATE mapa_lib)
add_test(NAME schedule COMMAND test_schedule)

add_executable(test_solvers tests/test_solvers.c)
target_include_directories(test_solvers PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/tests)
target_link_libraries(test_solvers PRIVATE mapa_lib)
add_test(NAME solvers COMMAND test_solvers)

add_executable(test_integration tests/test_integration.c)
target_include_directories(test_integration PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/tests)
target_link_libraries(test_integration PRIVATE mapa_lib)
add_test(NAME integration COMMAND test_integration)
