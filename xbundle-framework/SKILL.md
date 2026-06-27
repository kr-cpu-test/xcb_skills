---
name: xbundle-framework
description: Create, build, package, run, and extend xbundle-template based cross-platform C/C++ command module projects. Use when Codex needs to discover and use xbundle-template, create project/module/case scaffolding, register _icmd modules with xbundle_add_module, expose lib/*_icmd.dylib or lib/*_icmd.so through a host loader, route build/run work to template-generated docs and scripts, handle Android LD_LIBRARY_PATH or iOS helper caveats, or add XBUNDLE_INFO and XBUNDLE_MAIN entrypoints.
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
4. Register each case through the template, build, and loader layers described in `project-composition.md` and `module-runtime.md`.
5. Build, package, and run through the generated project's README and scripts. Do not run `_icmd` shared modules directly.
6. Keep the module entrypoint thin. Put reusable case logic in normal C/C++ sources and call it from tests, optional CLIs, and the xbundle module entrypoint.
7. Avoid editing vendored or copied dependencies. Prefer project configuration or an explicit local override when the user is intentionally developing adjacent projects together.

## Capabilities

- Discover and use `xbundle-template` without assuming unavailable CLI commands.
- Create xbundle projects, modules, and case skeletons from template output.
- Register a case as an `_icmd` module with `xbundle_add_module`.
- Package and run command modules through template-generated docs, scripts, and loaders.
- Compose xbundle modules with xcbench-style benchmark projects without forcing benchmark logic into the module boundary.

## Resources

- `references/template-workflow.md`: how to discover and use `xbundle-template` to create projects, modules, and case skeletons.
- `references/project-composition.md`: recommended layouts, three-layer case registration, packaging, and AI/scratch directory guidance.
- `references/module-runtime.md`: how a case becomes an xbundle command module with `XBUNDLE_INFO`, `XBUNDLE_MAIN`, and `xbundle_add_module`.
- `references/build-and-run.md`: build/run routing to xbundle-template docs and generated project scripts, plus `_icmd`, Android host loading, and iOS helper/device-id caveats.
- `references/dependency-discovery.md`: fallback links and clone guidance for missing xbundle external projects; do not read during normal generated-project flows.
