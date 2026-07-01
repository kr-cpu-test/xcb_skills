# Dependency Discovery

Use this reference only when generated benchmark dependencies are missing or ambiguous, or when the user asks how to obtain benchmark dependencies. Do not load it during normal project creation or implementation flows.

## Policy

- Treat benchmark dependencies as project-managed dependencies, not skill-owned files.
- Do not vendor, submodule, or silently clone external repositories into this skill.
- Prefer the generated project's `.gitmodules`, CMake, CPM, FetchContent, and template-provided bootstrap commands.
- For CPM-based dependencies, prefer `CPM_SOURCE_CACHE=<workspace>/.cpm` under the benchmark project root instead of a global cache unless the user explicitly requests a shared cache.
- Clone or point at adjacent local checkouts only when the user explicitly wants offline work, local dependency development, or gives approval.

## sbench

Purpose: benchmark framework used by generated benchmark projects.

Generated projects should pin `sbench` by revision. Use the generated project's documented bootstrap command. Inspect `.gitmodules` and project config before assuming the submodule URL or revision.

If the user explicitly needs a local checkout or the generated project lacks dependency metadata, use:

```sh
git clone https://github.com/kr-cpu-test/sbench <workspace>/sbench
```

For adjacent local development, pass a local checkout through the generated project's documented override mechanism only when intentionally changing `sbench` and the benchmark project together.

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
