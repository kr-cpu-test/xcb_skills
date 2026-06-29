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
│   ├── suite_main.cpp
│   └── <bench_logic>.cpp
└── tests/
```

Do not add multi-bench manifests or registry directories for a single-bench task unless the xbundle project already provides them.

## Multi Bench Layout

Use this only for an explicit suite/domain runner. The suite root owns manifest, generated registry, shared fixtures/support/common, and tools; each leaf remains independently testable.

```text
src/
├── bench_manifest.cmake
├── suite_main.cpp
├── <generated-registry>/
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

The generated registry support should rely on `sbench/register.hpp` so a bench leaf can build both as a split executable and as part of a singleton suite. When leaves belong to one domain, prefer a domain suite that hides leaf registrations behind a domain-local factory; read `sbench-registration.md` for the C++ pattern.

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

When creating or extending a benchmark project, keep AI-facing project knowledge under `.ai/` and local scratch under `.dev/`:

```text
.
├── .ai/
│   ├── README.md
│   ├── plan/
│   ├── status/
│   ├── decisions/
│   └── notes/
└── .dev/
```

- `.ai/README.md`: committed AI index for the project. Link to architecture notes, build/test guidance, plans, status, decisions, benchmark conventions, and generated-template notes.
- `.ai/plan/`: committed plans for multi-step project work, migrations, benchmark additions, or xbundle integration.
- `.ai/status/`: committed current state and handoff/resume notes that future agents need. Keep it concise; do not use it as a raw log directory.
- `.ai/decisions/`: committed design decisions, tradeoffs, and durable rationale.
- `.ai/notes/`: committed AI-facing research notes or project-specific conventions that do not fit the other folders.
- `.dev/`: ignored local scratch space for temporary scripts, generated artifacts, logs, experiments, downloads, and bulky outputs.

Put AI-generated files that should survive handoff under `.ai/`. Use `.ai/status/` for handoff state, and put disposable files under `.dev/`.

Recommend ignoring local-only scratch:

```gitignore
/.dev/
```

Do not ignore `.ai/`; it is project documentation.
