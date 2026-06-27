---
name: cross-platform-bench
description: Create or extend sbench-based cross-platform C/C++ benchmark programs. Use when Codex needs to scaffold a single benchmark CLI, build a multi-bench CLI suite, add a benchmark leaf, implement flat JSONL benchmark results, add scheduler-control options, use sbench, xcthread, xcmem, xcjit, xcperf, RapidJSON, or write portable benchmark tests for macOS, iOS, Linux, Android, or OpenHarmony. For xbundle command modules, host/runtime integration, xbundle-template scaffolding, or xbundle_runtime wiring, use xbundle-framework with this skill.
---

# Cross Platform Bench

## Workflow

1. Identify whether the user wants one measurement, multiple independent measurements, or an xbundle command module.
   - Use `single-bench-cli` for one standalone measurement or a parameter sweep of the same measurement.
   - Use `multi-bench-cli` whenever the request contains multiple relatively independent measurements, even if the user does not say "suite".
   - In multi-bench work, split different setup, CLI options, measurement loops, or result schemas into separate bench leaves.
   - If the request emphasizes xbundle, host/app integration, reusable command modules, device runtime, or `xbundle_runtime`, use `xbundle-framework` for the module/runtime structure and keep this skill focused on benchmark logic.
2. Read `references/project-creation.md` before creating a benchmark project.
3. Read `references/file-organization.md` before adding or moving benchmark files.
4. Read only the benchmark implementation references needed for the task:
   - `references/common-implementation.md` for `MicroBench`, `ForkableBench`, CLI options, and tests.
   - `references/scheduler-control.md` for `-t/--threads`, QoS, priority, CPU affinity, cluster parameters, and platform scheduler control.
   - `references/bench-io.md` for benchmark I/O, JSONL result rows, output sinks, and xbundle runtime path handling.
   - `references/jit-and-libxcpu.md` for `xcmem`, `xcjit`, `xcperf`, and AArch64 JIT.
   - `references/dependency-discovery.md` only when `sbench`, `libxcpu`, or generated benchmark dependencies are missing or the user asks how to obtain dependencies.
5. Prefer the project generator over handwritten scaffolding when one is present. Add a bench through the generated project tool or mirror `assets/sbench-multi-bench-leaf`.
6. When a benchmark is exposed as an xbundle case or module, keep this skill responsible for the benchmark JSONL schema and reusable measurement logic; use `xbundle-framework` for module registration, loader exposure, and xbundle runtime I/O/path adaptation.
7. Use assets as implementation-time examples only when mirroring a template; they are not required reading for every task.

## Project Choice

Choose single bench when the prompt names one measurement, one smoke/repro/hello program, or many cases that are only parameter combinations of the same measurement. Size sweeps, thread-count sweeps, stride sweeps, and unroll sweeps belong in one parameterized bench when they share the same setup, CLI shape, inner loop, and result schema.

When the prompt says "module", "host", "runtime", "app integration", "device runtime", "bundle", or "xbundle", do not design the module boundary from this skill alone. Use `xbundle-framework` for that layer and keep benchmark measurement code reusable underneath it.

Choose multi bench whenever the request contains multiple relatively independent measurements. Treat different setup, CLI options, inner loops, or result schemas as separate bench leaves. If the same users will naturally run, compare, or maintain the tests together, put them under one multi-bench runner. If the tests only share the word "benchmark" but target different objects, dependencies, or runtime environments, use separate projects.

In multi-bench mode, use the generated manifest and registration support. In single-bench mode, do not add multi-bench registry structure unless the generated project already provides it.

## Resources

- `assets/sbench-single-bench/`: optional minimal single-bench example files.
- `assets/sbench-multi-bench-leaf/`: optional bench leaf example for an existing multi-bench project.
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
