# Common Implementation

## CMake For Single Bench

In `single-bench` xbundle projects, keep reusable benchmark code in a support target and link it into the xbundle module target created by `xbundle-framework`. Do not add multi-bench manifest or registry directories unless the xbundle project already provides them.

```cmake
add_library(<project_slug>_support STATIC
  src/<bench_logic>.cpp
)

target_include_directories(<project_slug>_support
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(<project_slug>_support
  PUBLIC
    libxcpu
    RapidJSON
)

target_link_libraries(<command_name>_icmd
  PRIVATE
    <project_slug>_support
    <project-provided-sbench-target>
)
```

Create `<command_name>_icmd` through the xbundle project/module rules, not in this skill. If the xbundle project provides a CLI wrapper or unit-test target, link that target to `<project_slug>_support` instead of duplicating measurement logic.

Keep tests linked to `<project_slug>_support` when they exercise reusable logic instead of an entrypoint.

A support library is for reusable implementation code. When a `.cpp` file exists
mainly to register a `MicroBench` or suite through static initialization, treat
it as runner/module composition source. Add it to the final runner/module target
directly, or collect it in an object target and add those objects to the final
runner/module target. See `sbench-registration.md` for the xbundle+sbench target
shape.

## CMake For Multi Bench Leaf

Use this only inside a multi-bench suite root. Project helper functions such as `project_add_bench` belong to multi-bench leaf/suite builds, not single-bench projects:

```cmake
add_library(instr_tp_support STATIC
  src/instr_tp.cpp
)

target_include_directories(instr_tp_support
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

project_add_bench(instr_tp
  SOURCES
    src/main.cpp
    src/instr_tp.cpp
)

project_bench_include_directories(instr_tp
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

project_bench_link_libraries(instr_tp
  PRIVATE
    libxcpu
    RapidJSON
)

project_install_bench(instr_tp)
```

`project_add_bench` should use the project's sbench target and build-mode conventions. For exact C++ leaf, suite, nested suite, unified runner, and split/singleton registration APIs, read `sbench/docs/registration.md`, `sbench/docs/api.md`, and `sbench/docs/examples.md`. Use `sbench-registration.md` only for this skill's project-organization policy.

Keep a leaf-local support library for that leaf's reusable implementation. Move code to suite-root `fixtures/`, `support/`, or `common/` only when multiple leaves use it, and let leaves link those shared targets explicitly.

For benchmark data, prefer JSONL output instead of prose; read `bench-io.md` for output schema and sinks.
