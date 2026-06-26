# Dependency Discovery

Use this reference only when generated benchmark dependencies are missing or ambiguous, or when the user asks how to obtain benchmark dependencies. Do not load it during normal project creation or implementation flows.

## Policy

- Treat benchmark dependencies as project-managed dependencies, not skill-owned files.
- Do not vendor, submodule, or silently clone external repositories into this skill.
- Prefer the generated project's `.gitmodules`, `.xcb_template.json`, CMake, CPM, FetchContent, and template-provided bootstrap commands.
- Clone or point at adjacent local checkouts only when the user explicitly wants offline work, local dependency development, or gives approval.

## sbench

Purpose: benchmark framework used by generated xcbench projects.

Generated projects pin `sbench` by revision. Prefer:

```sh
xcb-template bootstrap-submodules .
```

or the generated project's documented bootstrap command. Inspect `.gitmodules` and `.xcb_template.json` before assuming the submodule URL or revision.

If the user explicitly needs a local checkout or the generated project lacks dependency metadata, use:

```sh
git clone https://github.com/kr-cpu-test/sbench <workspace>/sbench
```

For adjacent local development, pass a local checkout through the generator options only when intentionally changing `sbench` and the benchmark project together:

```sh
--sbench-repo-url <workspace>/sbench
--sbench-revision <known-good-sbench-commit>
```

Use an empty `--sbench-revision ""` only for intentional floating local development.

## libxcpu

Purpose: CPU feature/topology/performance helper library used through `sbench` or selected benchmark code.

Do not make `cross-platform-bench` manually clone `libxcpu` during normal project creation. `sbench` pins `libxcpu` through its CMake configuration, and first configure may use CPM or FetchContent to obtain it.

If the user is intentionally testing a local `libxcpu` checkout, configure the generated project with:

```sh
-DSBENCH_LIBXCPU_SOURCE_DIR=<workspace>/libxcpu
```

If a local checkout is explicitly requested:

```sh
git clone https://github.com/kr-cpu-test/libxcpu <workspace>/libxcpu
```

## Other Benchmark Libraries

For `xcmem`, `xcjit`, `xcperf`, RapidJSON, or other code dependencies, follow the generated project and dependency CMake rules first. Use CPM, FetchContent, package-manager config, or project-provided options instead of adding new repository clone rules to this skill.
