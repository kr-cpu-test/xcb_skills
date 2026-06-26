# Build And Run

Use this reference when configuring, building, packaging, or running an xbundle command module. For projects created by `xbundle-template`, treat the template checkout README, generated project README, generated `scripts/build.py --help`, and generated `scripts/ios_idbg.py --help` as the authority. This file only records routing rules and caveats that should not be duplicated from the template docs.

## Source Priority

Use this order before writing commands:

1. `xbundle-template` checkout README and CLI help.
2. Generated project README.
3. Generated `scripts/build.py --help`, `scripts/ios_idbg.py --help`, and project config files.
4. Host loader, harness, client, or platform docs.
5. Raw CMake only when the project has no generated or documented runner.

Do not copy a build/run recipe from this skill when the generated project already documents the command. Do not invent `xbundle-template` commands or hardcode local SDK, device, bundle, app, or helper paths.

For generated projects, let `scripts/build.py` and `tools/fetch_xbundle_runtime.py` resolve `xbundle_runtime`. If runtime, idbg host, or template sources are missing, use `dependency-discovery.md`; do not manually clone unrelated code libraries for the xbundle layer.

## Template-Generated Projects

The template repository and generated project README already document:

- project creation and target selection
- `scripts/build.py`
- generated CMake presets and `xbundle/cmake.json`
- target semantics such as `macos:cli`, `android:cli`, `ohos:cli`, `ios:idbg`, and `ios:app`
- package output directories
- iOS app build, install, connect, and run through `scripts/ios_idbg.py`
- bundle id, provisioning, `ios_app.allow_provisioning_updates`, and `ios_app.development_team`
- `--enable-jit` as an idbg helper flow

When those docs exist, reference them instead of restating their commands in skill output. Use this skill only to enforce the xbundle-specific execution boundaries below.

## Module Execution Boundary

`_icmd` outputs are shared modules, not standalone executables. Do not run `<case_name>_icmd.dylib` or `<case_name>_icmd.so` directly as a process.

Execute a command module through one of these project-provided entrypoints:

- generated `scripts/ios_idbg.py`
- host loader
- harness
- client
- generated CLI wrapper

After build/package, verify the loader-expected structure rather than assuming a fixed path. For a conventional command module package, expect a loadable module under `lib/` such as `lib/<case_name>_icmd.dylib` or `lib/<case_name>_icmd.so`.

## Validation Order

Validate in this order:

1. Run reusable logic tests with the project test runner.
2. Run local CLI `--help` and one smoke invocation only when a CLI exists.
3. Run an xbundle command smoke through the host loader, harness, or client.

The xbundle smoke should prove that the packaged module exports `xbundle_info` and `xbundle_main`, loads through the host, receives arguments, and writes through the runtime I/O path.

## Android Caveat

Use the generated project README and Android target docs for build and package commands. Do not add `adb push` or device paths unless the generated project or host docs define the layout.

For Android native CLI hosts, launch with an explicit loader path so the host can find the runtime provider and dynamic command libraries:

```sh
LD_LIBRARY_PATH=<xbundle-name>/dynamic_commands <host-or-harness> <host-args>
```

Keep the pushed directory self-contained: host executable, runtime provider, command modules, and private dependencies must stay in the package layout expected by the loader. If an Android host accepts command modules with `.dylib` filenames, keep the template's naming. The filename extension is a loader convention; the runtime loader path is the important part.

## iOS Caveats

iOS xbundle commands run inside a signed host app. Use generated project docs and `scripts/ios_idbg.py` when present; do not assemble a parallel `xcodebuild`, `devicectl`, or idbg command line unless the project lacks generated wrappers.

Do not run `_icmd` modules directly on iOS. Load the xbundle through the host's package/load mechanism, then execute the command through the host client or loader.

`--enable-jit` only covers the host/helper execution path. If the command module itself emits executable code, use the iOS xbundle JIT implementation pattern in `cross-platform-bench/references/jit-and-libxcpu.md` or the host's JIT docs.

`enable_ios_jit.py` is the LLDB helper script. `--enable-jit` launches the host app suspended when needed, attaches LLDB, imports the helper py, and continues the process through the host workflow. Use `--no-iproxy` only when an external tunnel is already running and managed outside the command.

Device identifiers can differ by tool. `devicectl` may accept a CoreDevice identifier or device name, while tunnel/client layers such as `iproxy` may require the hardware UDID. If install succeeds but the client cannot create a tunnel or connect using the same `--device` value, discover the hardware UDID through project tooling, Xcode, or device inventory commands and retry the client with that identifier. Do not guess or hardcode device IDs in reusable docs.

For iOS validation, run at least one physical-device smoke test when changing signing, packaging, loader paths, JIT behavior, or command dependencies. Verify every loadable Mach-O required by the xbundle is signed according to the host app's rules.

## Non-Template Projects

For non-template projects, prefer project-provided presets and package targets. Use generic CMake and host-loader rules only when the project has no platform-specific runner. Mention platform-specific tools such as `hdc`, `adb`, or direct host setup only when the project documents those steps.

## Validation Checklist

- Project configures with the selected preset or documented CMake options.
- The module build target succeeds.
- The packaged xbundle contains the expected `lib/*_icmd.dylib`, `lib/*_icmd.so`, or template-defined command module filename.
- Required runtime providers and private dependencies are in the package layout.
- Reusable tests pass.
- Optional CLI smoke passes when a CLI exists.
- Host/loader smoke can execute the command by name and pass arguments.
- Android native CLI host runs with `LD_LIBRARY_PATH=<xbundle-name>/dynamic_commands` when that layout is used.
- iOS host smoke runs through the helper/client flow, not by executing the module directly.
- iOS install and client/tunnel steps use a device identifier accepted by that specific tool; CoreDevice IDs and hardware UDIDs are not always interchangeable.
