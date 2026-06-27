# File Organization

## Single Bench Layout

Use this for one benchmark program:

```text
.
├── CMakeLists.txt
├── CMakePresets.json
├── include/<bench_name>/
├── src/
│   ├── main.cpp
│   ├── suite_main.cpp
│   └── <bench_logic>.cpp
└── tests/
```

Do not add multi-bench manifests or registry directories for a single-bench task unless the generated project already provides them.

## Multi Bench Layout

Use this for multiple relatively independent measurements that should share one runner:

```text
src/
├── bench_manifest.cmake
├── suite_main.cpp
├── <generated-registry>/
├── fixtures/
├── support/
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

The generated registry support should rely on `sbench/register.hpp` so a bench leaf can build both as a split executable and as part of a singleton suite. Do not invent legacy `xcbench` registry directories unless the generator creates them for compatibility.

## Bench Leaf Layout

Keep reusable logic separate from CLI registration:

```text
src/<bench_name>/
├── CMakeLists.txt
├── include/<bench_name>/<bench_name>.hpp
├── src/main.cpp
├── src/<bench_name>.cpp
├── tests/test_smoke.cpp
└── docs/README.md
```

Tests should link the support/static logic library where possible, not the CLI entrypoint.

## Fixture And Support Separation

Use shared `fixtures/` or `support/` code for platform, scheduler, affinity, output sink, JSONL writer, setup/teardown, and probe helpers that several bench leaves genuinely share.

Keep each bench leaf's CLI options, setup parameters, measurement loop, result schema, validation rules, and mutable state local to that leaf. A fixture should provide capabilities, not know the business meaning of a specific bench. Bench leaves should not include each other's implementation files.

Treat parameter sweeps of one measurement as one bench. Split into separate leaves when tests have different setup, CLI shape, inner loop, or result fields.

## AI And Scratch Directories

When creating or extending a benchmark project, recommend these optional project-root directories:

```text
.
├── .ai/
├── .dev/
└── .status/
```

- `.ai/`: committed AI-facing project notes, architecture summaries, build/test guidance, and conventions that help future AI agents understand the project.
- `.dev/`: local scratch space for temporary scripts, generated artifacts, logs, and experiments.
- `.status/`: local process notes, active task goals, task deltas, handoff state, and resume notes.

Recommend ignoring local-only directories:

```gitignore
/.dev/
/.status/
```

Do not ignore `.ai/`; it is project documentation. Keep transient notes in `.status/` and generated scratch files in `.dev/`.
