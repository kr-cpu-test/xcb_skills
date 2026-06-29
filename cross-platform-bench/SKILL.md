---
name: cross-platform-bench
description: Create or extend sbench-based cross-platform C/C++ benchmark logic for xbundle-template projects and existing benchmark suites. Use when Codex needs benchmark measurement code, single-bench or multi-bench sbench structure, flat JSONL benchmark results, scheduler-control options, xcthread, xcmem, xcjit, xcperf, RapidJSON, libxcpu, or portable benchmark tests for macOS, iOS, Linux, Android, or OpenHarmony. For new benchmark project scaffolding, xbundle command modules, host/runtime integration, xbundle-template usage, or xbundle_runtime wiring, use xbundle-framework with this skill.
---

# Cross Platform Bench

## Workflow

1. Identify whether the user wants one measurement, an explicit suite/domain runner, or an xbundle command module.
   - Use `single-bench` by default for one measurement, one proof/smoke/repro, or parameter sweeps of the same measurement.
   - Use `multi-bench-suite` only when the user asks for a suite, unified runner, domain/subdomain grouping, long-lived benchmark collection, or repeated future leaf additions.
   - In multi-bench work, keep leaves independent and put only genuinely shared fixture/support/common code at the suite root.
   - If the request emphasizes xbundle, host/app integration, reusable command modules, device runtime, or `xbundle_runtime`, use `xbundle-framework` for the module/runtime structure and keep this skill focused on benchmark logic and sbench suite structure.
2. Read `references/project-creation.md` before creating or extending a benchmark project.
3. Read `references/file-organization.md` before adding or moving benchmark files.
4. Read only the benchmark implementation references needed for the task:
   - `references/sbench-registration.md` for `MicroBench`, `ForkableBench`, leaf factories, domain suites, nested suites, and split/singleton registration.
   - `references/common-implementation.md` for CMake target patterns, support library linkage, reusable logic, and tests.
   - `references/scheduler-control.md` for `-t/--threads`, QoS, priority, CPU affinity, cluster parameters, and platform scheduler control.
   - `references/bench-io.md` for benchmark I/O, JSONL result rows, output sinks, and xbundle runtime path handling.
   - `references/jit-and-libxcpu.md` for `xcmem`, `xcjit`, `xcperf`, and AArch64 JIT.
   - `references/dependency-discovery.md` only when `sbench`, `libxcpu`, or benchmark dependencies are missing or the user asks how to obtain dependencies.
5. For new benchmark project scaffolding, route through `xbundle-framework` and `xbundle-template`; this skill supplies the benchmark code, result schema, scheduler options, and sbench shape inside that project.
6. When extending an existing benchmark project, use its README, project-provided add-bench tool, or current layout. Mirror assets only as implementation examples, not as a separate project template.
7. When a benchmark is exposed as an xbundle case or module, keep this skill responsible for the benchmark JSONL schema and reusable measurement logic; use `xbundle-framework` for module registration, loader exposure, and xbundle runtime I/O/path adaptation.
8. Use assets as implementation-time examples only when mirroring a template; they are not required reading for every task.

## Project Choice

Choose single bench when the prompt names one measurement, one smoke/repro/hello program, or many cases that are only parameter combinations of the same measurement. Size sweeps, thread-count sweeps, stride sweeps, and unroll sweeps belong in one parameterized bench when they share the same setup, CLI shape, inner loop, and result schema.

When the prompt says "module", "host", "runtime", "app integration", "device runtime", "bundle", or "xbundle", do not design the module boundary from this skill alone. Use `xbundle-framework` for that layer and keep benchmark measurement code reusable underneath it.

Choose multi bench only for an explicit suite/domain workflow: a unified runner, grouped domains/subdomains, a long-lived benchmark collection, or ongoing leaf additions. Treat different setup, CLI options, inner loops, or result schemas as separate leaves only after multi-bench has been chosen. If the user merely asks for several variants of one measurement, keep one parameterized single bench.

When multiple leaves belong to the same benchmark domain, prefer a domain suite: keep leaf benches registered in a domain-local factory and expose one `ForkableBench` suite to the unified runner. Use nested suites only for subdomains that should appear as grouped subcommands under the domain suite.

When exposing a benchmark or multi-bench domain through xbundle, keep one xbundle command module for that benchmark/domain by default. The module entrypoint should run the sbench domain suite through a thin xbundle-sbench adapter instead of creating one `XBUNDLE_MAIN` per leaf bench.

In multi-bench mode, use the suite manifest and registration support. In single-bench mode, do not add multi-bench registry structure unless the xbundle project already provides it.

## Resources

- `assets/sbench-single-bench/`: optional minimal single-bench example files.
- `assets/sbench-multi-bench-leaf/`: optional leaf example for an existing multi-bench suite root; not a default single-bench template.
- `references/project-creation.md`: new project routing through xbundle-template and existing project extension rules.
- `references/file-organization.md`: single vs multi layout rules.
- `references/sbench-registration.md`: sbench C++ registration patterns, including domain-local hidden registries and unified runner exposure.
- `references/common-implementation.md`: CMake target patterns, support libraries, and reusable implementation guidance.
- `references/scheduler-control.md`: parameterized scheduler control, thread placement, QoS, priority, CPU affinity, and cluster parameters.
- `references/bench-io.md`: benchmark I/O, JSONL result rows, output sinks, and xbundle runtime path handling.
- `references/jit-and-libxcpu.md`: libxcpu and JIT guidance.
- `references/dependency-discovery.md`: fallback links and local-development guidance for missing benchmark dependencies; do not read during normal xbundle-template flows.

## Validation

After implementing a bench, run the narrowest available verification:

```bash
cmake --preset default
cmake --build build/default
ctest --test-dir build/default --output-on-failure
```

If the xbundle project also provides a CLI wrapper, run `--help` and one smoke invocation that emits JSONL or expected stdout.

If the task also adds an xbundle module, follow `xbundle-framework` validation for the module target and host/loader smoke tests.
