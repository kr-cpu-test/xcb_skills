# File Organization

## Single Bench Layout

Use this for one benchmark case or module:

```text
.
├── CMakeLists.txt
├── CMakePresets.json
├── include/<bench_name>/
├── src/
│   ├── main.cpp
│   └── <bench_logic>.cpp
└── tests/
```

Do not add multi-bench manifests or registry directories for a single-bench task unless the xbundle project already provides them.

## Multi Bench Layout

Use this only for an explicit suite/domain runner. The suite root owns manifest, registration support, shared fixtures/support/common, and tools; each leaf remains independently testable.

```text
src/
├── bench_manifest.cmake
├── suite_main.cpp
├── <registration-support>/
├── fixtures/
├── support/
├── common/
├── tools/
├── instr_tp/
├── mem_latency/
└── cache_probe/
```

`bench_manifest.cmake` is the source-of-truth list of bench directories:

```cmake
set(PROJECT_BENCHES
  instr_tp
  mem_latency
)
```

Registration support should follow the project-provided add-bench tool or current suite conventions. When leaves belong to one domain, prefer one domain suite that hides leaves behind that suite command. Use `sbench-registration.md` for project policy; use `sbench/docs/registration.md`, `sbench/docs/api.md`, and `sbench/docs/examples.md` for exact C++ APIs and build modes.

## Bench Leaf Layout

Keep reusable logic separate from entrypoint registration:

```text
src/<bench_name>/
├── CMakeLists.txt
├── include/<bench_name>/<bench_name>.hpp
├── src/main.cpp
├── src/<bench_name>.cpp
├── tests/test_smoke.cpp
└── docs/README.md
```

Tests should link the support/static logic library where possible, not an entrypoint.

## Fixture And Support Separation

Use suite-root `fixtures/`, `support/`, or `common/` only for platform, scheduler, affinity, output sink, JSONL writer, setup/teardown, common CMake helpers, and probe helpers that several leaves genuinely share. `tools/` may hold project-provided add-leaf, validation, or maintenance scripts.

Keep each bench leaf's CLI options, setup parameters, measurement loop, result schema, validation rules, and mutable state local to that leaf. A fixture should provide capabilities, not know the business meaning of a specific bench. Bench leaves should not include each other's implementation files.

Treat parameter sweeps of one measurement as one bench. Split into separate leaves only inside an explicit multi-bench suite when tests have different setup, CLI shape, inner loop, or result fields.

## AI And Scratch Directories

For xbundle-template projects, use the project hygiene rules in
`xbundle-framework/references/project-composition.md`.

From this benchmark skill, only add durable benchmark conventions, result
schema notes, validation notes, and measurement decisions under `.ai/`. Put
temporary benchmark outputs, generated probes, logs, and experiments under
`.dev/`.
