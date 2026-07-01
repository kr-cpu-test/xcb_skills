# Project Composition

Use this reference when creating an xbundle-style project, registering outer commands, packaging modules, or combining xbundle with another scaffold.

## Default Layout

For `xbundle-template` projects, keep the generated layout as the source of truth. Use this shape only when the generated project leaves room for custom reusable logic:

```text
.
├── CMakeLists.txt
├── include/<project_name>/
├── src/
│   ├── <project_name>.cpp
│   ├── cli_main.cpp
│   └── module_main.cpp
├── tests/
└── xbundle/
    ├── scripts/
    └── tools/
```

- Put reusable command logic in `src/<project_name>.cpp` and headers under `include/<project_name>/`.
- Use `src/module_main.cpp` for `XBUNDLE_INFO`, `XBUNDLE_MAIN`, lifecycle hooks, and xbundle-specific I/O/path adaptation.
- Use `src/cli_main.cpp` only when a local standalone command is useful for smoke testing or developer ergonomics.
- Use `xbundle/` for package metadata, host-facing bundle files, generated scripts, tools, and config.

If the project is only a small command module, omit the CLI and keep `tests/` focused on reusable command logic.

## Three-Layer Command Registration

Register each host-visible command/program through the layers the project uses:

1. Template layer: use `xbundle-template` to generate the project or command skeleton when the template provides that command.
2. Build layer: use the generated project wrapper, commonly `xbundle_add_program(...)`, for the outer command/program. That wrapper may call lower-level `xbundle_add_module(...)` when module output is enabled.
3. Loader layer: package the built module where the generated loader discovers `*_icmd` commands.

Use the same command name consistently across the template metadata, CMake target, module metadata, package path, and loader-visible command name unless the project template explicitly separates those names.

## Xbundle Package Shape

Follow generated project or host loader docs for exact package layout. This skill only requires command modules to land where the loader discovers `*_icmd` shared modules; optional directories, private libraries, signing, and copy behavior belong to template or host docs.

## Composition With Benchmarks

When combining with sbench-based benchmarks, keep this skill responsible for xbundle scaffolding, `_icmd` registration, and loader structure. Keep benchmark logic reusable underneath the module entrypoint, and read the benchmark skill for result schema, JSONL, scheduler, or benchmark I/O rules.

For one benchmark or one multi-bench domain, default to one xbundle command/program and one `XBUNDLE_MAIN`. That command should be wired by `xbundle_add_program(...)` or the generated project's equivalent wrapper. The resulting module should run the sbench domain suite through a thin xbundle-sbench adapter, so leaf benchmarks remain sbench subcommands instead of separate `_icmd` modules.

For xbundle + sbench suites, keep the topology simple:

- one xbundle command/program per benchmark suite or real benchmark domain;
- one thin `XBUNDLE_MAIN` entrypoint that constructs the suite and calls
  `suite.main(argc, argv)`;
- leaf benchmarks remain sbench subcommands;
- reusable benchmark logic stays in support libraries below the module boundary.

Use `cross-platform-bench/references/sbench-registration.md` for benchmark suite
shape and domain grouping policy. Use `sbench/docs/registration.md` for exact
sbench registration, linking, and shared-runtime packaging facts.

Split into multiple xbundle command programs/modules only at a real command/domain boundary: independent benchmark domains, substantially different dependencies, incompatible platforms, separate host discovery needs, or distinct lifecycle/permission requirements. Do not split merely because a multi-bench suite has multiple leaf measurements.

## AI And Scratch Directories

When recommending a project structure, keep AI-facing project knowledge under `.ai/` and local scratch under `.dev/`:

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

- `.ai/README.md`: committed AI index for the project. Link to architecture notes, build/test guidance, xbundle targets, loader notes, plans, status, decisions, and generated-template notes.
- `.ai/plan/`: committed plans for multi-step project work, migrations, module additions, platform bring-up, or xbundle integration.
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

For validation commands, use `build-and-run.md`; from this reference, only check that reusable logic stays outside the module entrypoint and each host-visible command is represented in template, build, and loader layers.
