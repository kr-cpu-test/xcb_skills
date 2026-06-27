# Build And Run

Use this reference when configuring, building, packaging, or running an xbundle command module. For `xbundle-template` projects, the template README, generated project README, generated script help, and project config are authoritative. This file only keeps routing rules and caveats that should not be duplicated from template docs.

## Source Priority

Use this order before writing commands:

1. `xbundle-template` checkout README and CLI help.
2. Generated project README.
3. Generated script help and project config files.
4. Host loader, harness, client, or platform docs referenced by the generated project.

Do not copy a build/run recipe from this skill when the generated project already documents the command. Do not invent `xbundle-template` commands or hardcode local SDK, device, bundle, app, or helper paths.

Let generated scripts and config resolve `xbundle_runtime`. If template, runtime, or idbg sources are missing, use `dependency-discovery.md`. Do not manually clone unrelated code libraries for the xbundle layer.

## Module Execution Boundary

`_icmd` outputs are shared modules, not standalone executables. Do not run `<case_name>_icmd.dylib` or `<case_name>_icmd.so` directly as a process.

Execute a command module through project-provided entrypoints such as generated scripts, host loaders, harnesses, clients, or CLI wrappers.

After build/package, verify the loader-expected structure rather than assuming a fixed path. Conventional command module packages usually expose `*_icmd` shared modules under `lib/`; exact placement belongs to template or host docs.

## Validation

Validate in this order:

1. Run reusable logic tests with the project test runner.
2. Run local CLI `--help` and one smoke invocation only when a CLI exists.
3. Run an xbundle command smoke through the host loader, harness, or client.

The xbundle smoke should prove the module exports `xbundle_info` and `xbundle_main`, loads through the host, receives arguments, and writes through runtime I/O.

## Android Caveat

Use generated docs for Android build/package commands. Do not add `adb push` or device paths unless those docs define the layout.

For Android native CLI hosts, launch with an explicit loader path so the host can find the runtime provider and dynamic command libraries:

```sh
LD_LIBRARY_PATH=<xbundle-name>/dynamic_commands <host-or-harness> <host-args>
```

Keep the pushed directory self-contained according to host docs. If Android uses `.dylib` command filenames, keep the template's naming; the loader path is what matters.

## iOS Caveats

iOS xbundle commands run inside a signed host app. Use generated docs and wrappers; do not assemble parallel `xcodebuild`, `devicectl`, or idbg command lines when wrappers exist.

Do not run `_icmd` modules directly on iOS. Load the xbundle through the host's package/load mechanism, then execute the command through the host client or loader.

`--enable-jit` only covers the host/helper execution path. If the command module itself emits executable code, use the iOS xbundle JIT implementation pattern in `cross-platform-bench/references/jit-and-libxcpu.md` or the host's JIT docs.

`enable_ios_jit.py` is the LLDB helper script. Use host or generated wrapper docs for helper details; use `--no-iproxy` only when an external tunnel is already running and managed outside the command.

Device identifiers can differ by tool. If install succeeds but tunnel/client connection fails with the same `--device` value, discover the identifier required by that layer and retry. Do not guess or hardcode device IDs in reusable docs.

For iOS validation, run at least one physical-device smoke test when changing signing, packaging, loader paths, JIT behavior, or command dependencies.
