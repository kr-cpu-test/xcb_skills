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
├── tests/
└── tools/configure_platform.py
```

Do not add `bench_manifest.cmake` or `bench_registry/` for a single-bench task.

## Multi Bench Layout

Use this only for a suite:

```text
src/
├── bench_manifest.cmake
├── bench_registry/
├── suite_main.cpp
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

`bench_registry/` provides shared registration macros and a suite factory so a bench leaf can build both as a split executable and as part of a singleton suite. It is a multi-bench concern, not a requirement for single-bench projects.

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
