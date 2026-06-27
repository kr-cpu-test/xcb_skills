# Project Composition

Use this reference when creating an xbundle-style project, registering cases, packaging modules, or combining xbundle with another scaffold.

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
├── tools/
└── xbundle/
```

- Put reusable logic in `src/<project_name>.cpp` and headers under `include/<project_name>/`.
- Use `src/module_main.cpp` for `XBUNDLE_INFO`, `XBUNDLE_MAIN`, lifecycle hooks, and xbundle-specific I/O/path adaptation.
- Use `src/cli_main.cpp` only when a local standalone command is useful for smoke testing or developer ergonomics.
- Use `xbundle/` for package metadata, host-facing bundle files, or template-generated config.

If the project is only a small command module, omit the CLI and keep `tests/` focused on reusable case logic.

## Three-Layer Case Registration

Register each runnable case through the layers the project uses:

1. Template layer: use `xbundle-template` to generate the project, module, or case skeleton when the template provides that command.
2. Build layer: register the case target with `xbundle_add_module(NAME <case_name> ...)`.
3. Loader layer: package the built module where the generated loader discovers `*_icmd` commands.

Use the same `<case_name>` consistently across the template metadata, CMake target, module metadata, package path, and loader-visible command name unless the project template explicitly separates those names.

## Xbundle Package Shape

Follow generated project or host loader docs for exact package layout. This skill only requires command modules to land where the loader discovers `*_icmd` shared modules; optional directories, private libraries, signing, and copy behavior belong to template or host docs.

## Composition With Benchmarks

When combining with sbench-based benchmarks, keep this skill responsible for xbundle scaffolding, `_icmd` registration, and loader structure. Keep benchmark logic reusable underneath the module entrypoint, and read the benchmark skill for result schema, JSONL, scheduler, or benchmark I/O rules.

## AI And Scratch Directories

When recommending a project structure, include optional project-root directories for AI-oriented documentation and local process state:

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

Do not ignore `.ai/`; it is project documentation.

For validation commands, use `build-and-run.md`; from this reference, only check that reusable logic stays outside the module entrypoint and each case is represented in template, build, and loader layers.
