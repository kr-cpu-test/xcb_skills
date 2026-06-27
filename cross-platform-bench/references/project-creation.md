# Project Creation

Use the benchmark project generator from the project checkout. If no generator is present, create a small CMake project that depends on `sbench`.

For xbundle-oriented cross-platform projects, use this skill for the benchmark scaffold and use `xbundle-framework` for xbundle-template, module ABI, host/runtime, and `xbundle_runtime` wiring.

Do not require legacy `xcbench-template` or `xcb-template` commands for framework projects.

## Single Bench

Use for one test program or a small proof of concept:

Use the generated single-bench preset when the project provides one. Otherwise mirror `assets/sbench-single-bench`.

This layout does not need multi-bench manifests or registry directories.

## Multi Bench

Use only when the project is a suite with multiple benches:

Use the generated multi-bench preset when the project provides one. Otherwise mirror the multi-bench layout in `file-organization.md`.

Then add a bench through the generated project tool when one exists.

## Bootstrap And Configure

Bootstrap submodules and generate local build presets through the generated project's README, scripts, or CMake presets. Do not assume a fixed bootstrap or configure script name unless the project contains it.

After configure, build and test with the generated presets and the narrow validation commands from `SKILL.md`.

If generated benchmark dependencies are missing or the user asks how to obtain `sbench` or `libxcpu`, read `dependency-discovery.md`. Normal generated projects should use their own `.gitmodules`, CMake, CPM, FetchContent, or template-provided bootstrap.
