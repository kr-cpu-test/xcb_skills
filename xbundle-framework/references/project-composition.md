# Project Composition

Use this reference when creating an xbundle-style project, registering cases, packaging modules, or combining xbundle with another scaffold.

## Default Layout

Prefer this shape for a generated project that needs testable case logic and an xbundle command module:

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
- Use `xbundle/` for package metadata, host-facing bundle files, or generated `xbundle-template` output.

If the project is only a small command module, omit the CLI and keep `tests/` focused on reusable case logic.

## Three-Layer Case Registration

Register each runnable case through the layers the project uses:

1. Template layer: use `xbundle-template` to generate the project, module, or case skeleton when the template provides that command.
2. Build layer: register the case target with `xbundle_add_module(NAME <case_name> ...)`, which emits `<case_name>_icmd`.
3. Loader layer: package the built module under `lib/`; the host loader discovers `lib/*_icmd.dylib` or `lib/*_icmd.so` and exposes it as a command.

Use the same `<case_name>` consistently across the template metadata, CMake target, module metadata, package path, and loader-visible command name unless the project template explicitly separates those names.

## Xbundle Package Shape

Use this package shape when the host loader expects an xbundle directory:

```text
<xbundle-name>/
├── lib/
│   ├── <case_name>_icmd.dylib
│   └── <private_library>.dylib
├── bin/
└── share/
```

- `lib/` is required for command modules.
- `lib/<case_name>_icmd.dylib` or `lib/<case_name>_icmd.so` is a loadable command module.
- `bin/` is optional and should hold shims only when the template or host uses them.
- `share/` is optional read-only data for cases.

When a host loader is present, follow its documented load operation. For idbg-style loaders, loading an xbundle copies the package into the app/runtime command area and exposes every `lib/*_icmd.dylib` command through a virtual `/bin` mount.

## Composition With Benchmarks

When combining with `xcbench-template`, keep responsibilities separated:

- `xcbench-template`: benchmark scaffold, platform presets, `sbench` integration, tests, and benchmark suite structure.
- `xbundle-framework`: template-driven module/case scaffolding, `_icmd` registration, host loading, and package/module structure.
- Benchmark logic: reusable implementation callable from tests, a CLI entrypoint, or an xbundle module entrypoint.

Use a thin xbundle module entrypoint that calls the benchmark core. Keep result rows flat and self-describing when emitting JSONL, for example:

```json
{"bench":"memcpy","size":4096,"time_ns":123456,"bytes":4096000}
```

Do not force JIT, libxcpu, or benchmark-suite scaffolding into a simple module unless the user asks for those capabilities.

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

## Validation

Use `build-and-run.md` for configure, build, package, and platform run commands. From this composition reference, verify only the structural contract:

- reusable logic is separate from the xbundle module entrypoint
- each runnable case is registered through the template, build, and loader layers the project uses
- the package layout contains the host-expected command module under `lib/`
- xbundle-specific runtime and path adaptation stays in the module entrypoint
