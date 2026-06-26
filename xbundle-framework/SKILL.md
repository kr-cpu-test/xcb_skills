---
name: xbundle-framework
description: Create, build, package, run, and extend xbundle-template based cross-platform C/C++ command module projects. Use when Codex needs to discover and use an xbundle-template checkout or command, create project/module/case scaffolding, register cases as _icmd modules with xbundle_add_module, package xbundle directories, expose lib/*_icmd.dylib or lib/*_icmd.so through a host loader, route build/run work to xbundle-template generated README and scripts/build.py or scripts/ios_idbg.py docs, handle Android LD_LIBRARY_PATH host execution caveats, use iOS enable_ios_jit.py or --jit-helper flows, or add XBUNDLE_INFO and XBUNDLE_MAIN module entrypoints.
---

# Xbundle Framework

Use this skill to create xbundle-style command module projects and register runnable cases. Start from `xbundle-template` when it is available; keep implementation logic reusable so each case can be tested directly and exported as an xbundle command module.

## Workflow

1. Discover the template interface before creating anything:
   - Look for a project-relative `xbundle-template/` checkout first, then a package command, README, or CLI help.
   - If a required external xbundle project is missing or ambiguous, read `references/dependency-discovery.md`; do not silently clone or vendor dependencies into the skill.
2. Read only the relevant references:
   - `references/template-workflow.md` for generator discovery, project creation, and template-driven case creation.
   - `references/project-composition.md` for layout, three-layer case registration, packaging, and project hygiene.
   - `references/module-runtime.md` for `XBUNDLE_INFO`, `XBUNDLE_MAIN`, `xbundle_add_module`, and module command entrypoints.
   - `references/build-and-run.md` for build/run routing, generated-template documentation priority, `_icmd` execution boundaries, Android `LD_LIBRARY_PATH` caveats, and iOS helper caveats.
   - `references/dependency-discovery.md` only when `xbundle-template`, `xbundle_runtime`, or `xcb_idbg` is missing or the user asks how to obtain dependencies.
3. Create the project, module, or case skeleton through `xbundle-template` when the template provides a command for it. Use project-relative paths in instructions and generated docs.
4. Register each case through all required layers:
   - Template layer: generated project/module/case files.
   - Build layer: `xbundle_add_module(NAME <case_name> ...)` emits `<case_name>_icmd`.
   - Loader layer: package the module under `lib/` so the host loader can expose it as a command.
5. Build, package, and run through the generated project's README, documented presets, generated scripts, host loader, harness, or client. Do not run `_icmd` shared modules directly.
6. Keep the module entrypoint thin. Put reusable case logic in normal C/C++ sources and call it from tests, optional CLIs, and the xbundle module entrypoint.
7. Avoid editing vendored or copied dependencies. Prefer project configuration or an explicit local override when the user is intentionally developing adjacent projects together.

## Capabilities

- Discover and use `xbundle-template` without assuming unavailable CLI commands.
- Create xbundle projects, modules, and case skeletons from template output.
- Register a case as an `_icmd` module with `xbundle_add_module`.
- Package command modules into xbundle directories with `lib/*_icmd.dylib` or `lib/*_icmd.so`.
- Route configure, build, package, and validate work to template-generated project documentation and scripts.
- Connect packaged modules to a host loader when the project already provides one.
- Run xbundle commands through host loaders, harnesses, generated wrappers, or clients, using generated docs first and applying Android `LD_LIBRARY_PATH` and iOS helper caveats only when relevant.
- Compose xbundle modules with xcbench-style benchmark projects without forcing benchmark logic into the module boundary.

## Resources

- `references/template-workflow.md`: how to discover and use `xbundle-template` to create projects, modules, and case skeletons.
- `references/project-composition.md`: recommended layouts, three-layer case registration, packaging, and AI/scratch directory guidance.
- `references/module-runtime.md`: how a case becomes an xbundle command module with `XBUNDLE_INFO`, `XBUNDLE_MAIN`, and `xbundle_add_module`.
- `references/build-and-run.md`: build/run routing to xbundle-template docs and generated project scripts, plus non-template caveats for `_icmd`, Android host loading, and iOS helper/device-id behavior.
- `references/dependency-discovery.md`: fallback links and clone guidance for missing xbundle external projects; do not read during normal generated-project flows.
