# Multi-bench leaf example. The suite root provides project_add_bench and
# any shared fixtures/support/common targets that this leaf links explicitly.
add_library(hello_bench_support STATIC
  src/hello_bench.cpp
)

target_include_directories(hello_bench_support
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

project_add_bench(hello_bench
  SOURCES
    src/main.cpp
    src/hello_bench.cpp
)

project_bench_include_directories(hello_bench
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

project_install_bench(hello_bench)

if(BUILD_TESTING)
  add_executable(hello_bench_test_smoke
    tests/test_smoke.cpp
  )
  target_link_libraries(hello_bench_test_smoke PRIVATE hello_bench_support)
  add_test(NAME hello_bench_test_smoke COMMAND hello_bench_test_smoke)
endif()
