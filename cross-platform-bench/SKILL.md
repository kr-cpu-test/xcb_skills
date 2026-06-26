---
name: cross-platform-bench
description: Create or extend xcbench-template cross-platform C/C++ benchmark programs. Use when Codex needs to scaffold a single benchmark CLI, build a multi-bench CLI suite, add a benchmark leaf under an existing xcbench project, implement flat JSONL benchmark results, add scheduler-control options, use sbench, xcthread, xcmem, xcjit, xcperf, RapidJSON, or write portable benchmark tests for macOS, iOS, Linux, Android, or OpenHarmony. For xbundle command modules, host/runtime integration, xbundle-template scaffolding, or xbundle_runtime wiring, use xbundle-framework with this skill.
---

# Cross Platform Bench

## Workflow

1. Identify whether the user wants one benchmark program, an xbundle command module, or a suite.
   - Use `single-bench-cli` for one standalone test program.
   - Use `multi-bench-cli` only when the user wants multiple benches, categories, or future bench additions.
   - If the request emphasizes xbundle, host/app integration, reusable command modules, device runtime, or `xbundle_runtime`, use `xbundle-framework` for the module/runtime structure and keep this skill focused on benchmark logic.
2. Read `references/project-creation.md` before creating a benchmark project.
3. Read `references/file-organization.md` before adding or moving benchmark files.
4. Read only the benchmark implementation references needed for the task:
   - `references/common-implementation.md` for `MicroBench`, `ForkableBench`, CLI options, and tests.
   - `references/scheduler-control.md` for `-t/--threads`, QoS, priority, CPU affinity, cluster parameters, and platform scheduler control.
   - `references/bench-io.md` for benchmark I/O, JSONL result rows, output sinks, and xbundle runtime path handling.
   - `references/jit-and-libxcpu.md` for `xcmem`, `xcjit`, `xcperf`, and AArch64 JIT.
   - `references/dependency-discovery.md` only when `sbench`, `libxcpu`, or generated benchmark dependencies are missing or the user asks how to obtain dependencies.
5. Prefer the repo generator over handwritten scaffolding:
   - New project: use `xcbench-template/xcb_template.py init-repo`.
   - Add bench to a multi-bench project: use `tools/add_bench.py` or mirror `assets/xcbench-multi-bench-leaf`.
6. When a benchmark is exposed as an xbundle case or module, keep this skill responsible for the benchmark JSONL schema and reusable measurement logic; use `xbundle-framework` for module registration, loader exposure, and xbundle runtime I/O/path adaptation.
7. Use assets as implementation-time examples only when mirroring a template; they are not required reading for every task.

## Project Choice

Choose single bench by default when the prompt says "a test program", "a benchmark", "hello world", or names one measurement.

When the prompt says "module", "host", "runtime", "app integration", "device runtime", "bundle", or "xbundle", do not design the module boundary from this skill alone. Use `xbundle-framework` for that layer and keep benchmark measurement code reusable underneath it.

Choose multi bench only when the prompt says "multiple benches", "suite", "many tests", "categories", or asks to keep adding benches over time. In multi-bench mode, `bench_manifest.cmake` and `bench_registry/` are required; in single-bench mode, they are not part of the intended structure.

## Resources

- `assets/xcbench-single-bench/`: optional minimal single-bench example files.
- `assets/xcbench-multi-bench-leaf/`: optional bench leaf example for an existing multi-bench project.
- `references/project-creation.md`: generator commands and platform setup.
- `references/file-organization.md`: single vs multi layout rules.
- `references/common-implementation.md`: C++ registration and CMake patterns.
- `references/scheduler-control.md`: parameterized scheduler control, thread placement, QoS, priority, CPU affinity, and cluster parameters.
- `references/bench-io.md`: benchmark I/O, JSONL result rows, output sinks, and xbundle runtime path handling.
- `references/jit-and-libxcpu.md`: libxcpu and JIT guidance.
- `references/dependency-discovery.md`: fallback links and local-development guidance for missing benchmark dependencies; do not read during normal generated-project flows.

## Validation

After implementing a bench, run the narrowest available verification:

```bash
cmake --preset default
cmake --build build/default
ctest --test-dir build/default --output-on-failure
```

If the task adds a CLI bench, also run `--help` and one smoke invocation that emits JSONL or expected stdout.

If the task also adds an xbundle module, follow `xbundle-framework` validation for the module target and host/loader smoke tests.
