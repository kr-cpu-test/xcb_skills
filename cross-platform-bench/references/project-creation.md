# Project Creation

For a new cross-platform benchmark project, use `xbundle-framework` to create the `xbundle-template` project, module, and case skeleton first. This skill then supplies the benchmark measurement code, sbench shape, JSONL result rows, scheduler options, and JIT/libxcpu guidance inside that xbundle project.

Do not create a separate ad-hoc CMake benchmark project when the task is to create a new benchmark project. New project scaffolding belongs to `xbundle-framework`; benchmark implementation details belong here.

When extending an existing benchmark project, follow that project's README, project-provided add-bench tool, and current layout. If no add-bench tool exists, mirror this skill's assets only inside the existing project style.

## Single Bench

Default to this path for one test program, proof of concept, repro, smoke command, or parameter sweep of one measurement:

Use this as the default benchmark shape inside the xbundle project/module/case or inside an existing single-bench project.

This layout does not need multi-bench manifests or registry directories.

## Multi Bench

Use only for an explicit suite/domain workflow: unified runner, grouped domains, long-lived benchmark collection, or repeated future leaf additions.

Use this only inside an xbundle project that is intentionally structured as a suite, or inside an existing multi-bench suite root.

Add a leaf through the project-provided tool when one exists. If no tool exists, mirror `assets/sbench-multi-bench-leaf` as a leaf inside the suite root and keep shared fixtures/support/common in the suite root.

## Bootstrap And Configure

Bootstrap dependencies and local build presets through the xbundle project's README, scripts, or CMake presets. Do not assume a fixed bootstrap or configure script name unless the project contains it.

After configure, build and test with the project presets and the narrow validation commands from `SKILL.md`.

Use `xbundle-framework` validation for package, module, host, and loader smoke tests. Use this skill's validation for the reusable benchmark logic and any CLI or unit test surface the xbundle project provides.

If benchmark dependencies are missing or the user asks how to obtain `sbench` or `libxcpu`, read `dependency-discovery.md`. Normal xbundle-template projects should use their own `.gitmodules`, CMake, CPM, FetchContent, or template-provided bootstrap.
