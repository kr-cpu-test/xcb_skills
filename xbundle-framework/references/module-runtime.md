# Module Runtime

Use this reference when turning one generated outer command/program into an xbundle command module.

## Command Entry

Each xbundle command/program needs a thin module entrypoint that exports metadata plus a main-like function:

```c
#include <xbundle/xbundle.h>
#include <xbundle/xbundle_io.h>

static int command_main(int argc, char **argv) {
  xbundle_printf("{\"case\":\"smoke\",\"status\":\"ok\"}\n");
  return 0;
}

XBUNDLE_INFO("smoke", "0.1.0")
XBUNDLE_MAIN(command_main)
```

Keep `command_main` thin. Parse module arguments there only when needed, then call reusable C/C++ command logic.

For an sbench-backed benchmark module, keep the module entrypoint equally thin: construct the sbench domain suite, attach an xbundle-backed logger or output adapter when needed, and call `suite.main(argc, argv)`. A multi-bench domain should still have one `XBUNDLE_MAIN`; leaf benches remain sbench subcommands under the suite.

Optional lifecycle hooks are exported by name:

```c
XBUNDLE_EXPORT int xbundle_start(const xbundle_context_t *ctx);
XBUNDLE_EXPORT void xbundle_finish(const xbundle_context_t *ctx, int exit_code);
```

Use lifecycle hooks for per-invocation setup or cleanup when the host supports them. Do not treat them as project registration.

## Runtime Services

Inside a module entrypoint, prefer xbundle services for host-mediated output and paths:

- `xbundle_write_stdout()`
- `xbundle_write_stderr()`
- `xbundle_printf()`
- `xbundle_fprintf_stderr()`
- `xbundle_path_info()`
- `xbundle_path_can_read()`
- `xbundle_path_can_write()`
- `xbundle_resolve_path()`
- `xbundle_getcwd()`
- `xbundle_chdir()`

Use ordinary platform APIs in reusable command code only when that code is intentionally independent of host-mediated behavior.

If the module calls xbundle I/O or path helpers, make the module target link against the runtime provider through the generated project wrapper, commonly `xbundle_add_program(...)`. Use direct `xbundle_add_module(...)` only when the generated project docs require lower-level module wiring. Do not bypass runtime linking and then compensate with direct `printf`, `fprintf`, or platform path calls in the module entrypoint.

### Virtual Path Handling

When a module accepts a path argument such as `--output <path>`, treat it as a
host-defined virtual path, not necessarily as a process-local filesystem path.

- Check host policy with `xbundle_path_info()` or `xbundle_path_can_write()`
  before opening a path for write.
- Call `xbundle_resolve_path()` only when a lower-level file API requires a real
  host path. Resolving translates a path; it does not authorize access.
- If a path is not writable, or cannot be resolved when a real path is required,
  write diagnostics through `xbundle_write_stderr()` or
  `xbundle_fprintf_stderr()` and return non-zero.
- Benchmarks should not emit a success result row for failed path setup.

## Program Wrapper And Module Target

Prefer the generated project wrapper for the outer command/program. In current
`xbundle-template` projects, this is commonly `xbundle_add_program(...)` in
`xbundle/project.cmake` or the generated top-level CMake:

```cmake
xbundle_add_program(
  NAME smoke
  COMMAND_NAME "smoke"
  SOURCES
    src/smoke_main.cpp
    src/smoke.cpp
  LINK_LIBRARIES
    smoke_support
)
```

This wrapper can build both the standalone CLI and the `_icmd` module from the
same command sources, depending on target configuration. It calls lower-level
module helpers when module output is enabled.

Use direct `xbundle_add_module(...)` only when the generated project documents
that lower-level path or has no command/program wrapper:

```cmake
find_package(xbundle_runtime REQUIRED)

xbundle_add_module(
  NAME smoke
  SOURCES
    src/smoke_module.cpp
    src/smoke.cpp
)
```

Direct `xbundle_add_module(NAME smoke ...)` emits a module target using the runtime helper. The generated wrapper may instead choose target names such as `smoke_module` while setting the loader-visible output name. Common module outputs are:

```text
smoke_icmd.dylib
smoke_icmd.so
```

Pass command-specific `LINK_LIBRARIES`, `INCLUDE_DIRS`, and `COMPILE_DEFINITIONS` through the generated wrapper first. When using lower-level module wiring, pass them through `xbundle_add_module` instead of bypassing the helper.

Runtime packaging is target-specific. Follow generated target docs and verify the host can resolve runtime symbols.

## Validation

Use `build-and-run.md` for build/run commands. From this reference, verify only the module contract:

- the module exports `xbundle_info` through `XBUNDLE_INFO`
- the module exports `xbundle_main` through `XBUNDLE_MAIN`
- runtime output and path access use `xbundle_runtime` helpers inside the module entrypoint
- command-specific libraries, include paths, and definitions are passed through the generated wrapper, or through `xbundle_add_module` only for lower-level module wiring
