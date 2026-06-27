# Project Creation

Use the `xcbench-template` generator from the project checkout. If the package is installed, prefer the `xcb-template` console script; otherwise run `./xcbench-template/xcb_template.py` from a checkout that contains `xcbench-template`.

For xbundle-oriented cross-platform projects, use this skill for the benchmark scaffold and use `xbundle-framework` for xbundle-template, module ABI, host/runtime, and `xbundle_runtime` wiring.

```bash
xcb-template init-repo <repo-path> [options]
# or:
./xcbench-template/xcb_template.py init-repo <repo-path> [options]
```

## Single Bench

Use for one test program or a small proof of concept:

```bash
xcb-template init-repo ./hello_bench \
  --preset single-bench-cli \
  --target-platform macos \
  --target-platform ios
```

This layout does not need `bench_manifest.cmake` or `bench_registry/`.

## Multi Bench

Use only when the project is a suite with multiple benches:

```bash
xcb-template init-repo ./cpu_bench_suite \
  --preset multi-bench-cli \
  --target-platform macos \
  --target-platform ios \
  --target-platform android \
  --target-platform ohos
```

Then add a bench:

```bash
./tools/add_bench.py . \
  --bench-name instr_tp \
  --bench-class-name InstrTp \
  --bench-description "Instruction throughput benchmark"
```

## Bootstrap And Configure

Bootstrap submodules after creating a project:

```bash
xcb-template bootstrap-submodules .
```

Generate local CMake presets:

```bash
./tools/configure_platform.py . --platform macos
./tools/configure_platform.py . --platform ios
./tools/configure_platform.py . --platform android --android-ndk-root <android-ndk-root>
./tools/configure_platform.py . --platform ohos --ohos-sdk-root <ohos-native-sdk-root>
```

After configure, build and test with the generated presets and the narrow validation commands from `SKILL.md`.

If generated benchmark dependencies are missing or the user asks how to obtain `sbench` or `libxcpu`, read `dependency-discovery.md`. Normal generated projects should use their own `.gitmodules`, CMake, CPM, FetchContent, or template-provided bootstrap.
