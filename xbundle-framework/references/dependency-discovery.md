# Dependency Discovery

Use this reference only when a required xbundle external project is missing or ambiguous, or when the user asks how to obtain dependencies. Do not load it during normal generated-project build/run flows.

## Policy

- Treat external projects as discoverable dependencies, not skill-owned files.
- Do not vendor, submodule, or silently clone external repositories into this skill.
- Prefer existing workspace/project-local checkouts and generated project configuration.
- Clone only when the user explicitly asks or approves a target directory.
- Keep reusable instructions path-neutral; use placeholders such as `<workspace>` or `<project>`.

Normal C/C++ library dependencies belong to the target project's CMake, CPM, FetchContent, or package-manager setup. Do not add unrelated library repositories to this xbundle-framework dependency list.

## xbundle-template

Purpose: project generator for xbundle command projects.

Discovery order:

1. Look for an existing workspace/project-local checkout such as `<workspace>/xbundle-template/`.
2. Look for a package or console command documented by the project.
3. Read the checkout README and CLI help before using any command.

If missing, report the dependency and provide the upstream source:

```sh
git clone https://github.com/kr-cpu-test/xbundle-template <workspace>/xbundle-template
```

After cloning, use the checkout's README and CLI help as the source of truth. Do not invent `init`, `create`, or `add-case` commands from memory.

## xbundle_runtime

Purpose: ABI/runtime SDK used by generated projects and xbundle command modules.

Do not manually resolve runtime before the generated project needs it. Prefer the generated mechanisms:

- `xbundle/scripts/build.py`
- `xbundle/tools/fetch_xbundle_runtime.py`
- `xbundle/config.json`
- `xbundle/local.json`
- `subprojects/xbundle_runtime`
- `xbundle/cache/xbundle_runtime/`

Generated projects may fetch or cache runtime from their configured `xbundle_runtime.repo_url` and `xbundle_runtime.git_tag`. A local checkout is only an override through `xbundle/local.json` or generated project docs; do not add a separate runtime clone flow to this skill.

## xcb_idbg

Purpose: iOS idbg host/client backend for `ios:app`,
`xbundle/scripts/ios_idbg.py`, and JIT-enabled iOS command runs.

Discovery order:

1. Read `ios_app.xcb_idbg_dir` from `xbundle/config.json` or `xbundle/local.json`.
2. Check `XCB_IDBG_DIR` and `IDBG_XCB_IDBG_DIR`.
3. Check for an existing workspace/project-local `xcb_idbg/` checkout.
4. Verify the checkout contains `tools/idbg_cli.py`.

If missing, report the dependency and provide the upstream source:

```sh
git clone https://github.com/kr-cpu-test/xcb_idbg.git <workspace>/xcb_idbg
```

For JIT runs, use a checkout whose `tools/idbg_cli.py --help` supports `--enable-jit`; generated wrappers should fail early if that capability is missing.

## Platform Toolchains

Android NDK, OpenHarmony SDK, Xcode, `devicectl`, `adb`, Python, CMake, and `uv` are system/toolchain dependencies. Use generated project docs and config for them; do not list clone commands here.
