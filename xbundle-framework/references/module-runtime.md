# Module Runtime

Use this reference when turning one generated case into an xbundle command module.

## Case Entry

Each command case needs a thin module entrypoint that exports metadata plus a main-like function:

```c
#include <xbundle/xbundle.h>
#include <xbundle/xbundle_io.h>

static int case_main(int argc, char **argv) {
  xbundle_printf("{\"case\":\"smoke\",\"status\":\"ok\"}\n");
  return 0;
}

XBUNDLE_INFO("smoke", "0.1.0")
XBUNDLE_MAIN(case_main)
```

Keep `case_main` thin. Parse module arguments there only when needed, then call reusable C/C++ case logic.

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

Use ordinary platform APIs in reusable case code only when that code is intentionally independent of host-mediated behavior.

If the module calls xbundle I/O or path helpers, make the module target link against the runtime provider through the generated project or `xbundle_add_module`. Do not bypass runtime linking and then compensate with direct `printf`, `fprintf`, or platform path calls in the module entrypoint.

## Register The Module Target

Register the case as an `_icmd` module with `xbundle_add_module`:

```cmake
find_package(xbundle_runtime REQUIRED)

xbundle_add_module(
  NAME smoke
  SOURCES
    src/smoke_module.cpp
    src/smoke.cpp
)
```

`xbundle_add_module(NAME smoke ...)` emits the command module using the generated project's naming convention, commonly:

```text
smoke_icmd.dylib
smoke_icmd.so
```

Pass case-specific `LINK_LIBRARIES`, `INCLUDE_DIRS`, and `COMPILE_DEFINITIONS` through `xbundle_add_module` instead of bypassing the helper.

Runtime packaging is target-specific. Follow generated target docs and verify the host can resolve runtime symbols.

## Validation

Use `build-and-run.md` for build/run commands. From this reference, verify only the module contract:

- the module exports `xbundle_info` through `XBUNDLE_INFO`
- the module exports `xbundle_main` through `XBUNDLE_MAIN`
- runtime output and path access use `xbundle_runtime` helpers inside the module entrypoint
- case-specific libraries, include paths, and definitions are passed through `xbundle_add_module`
