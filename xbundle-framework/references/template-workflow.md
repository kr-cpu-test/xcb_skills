# Xbundle Template Workflow

Use this reference when starting from `xbundle-template` or adding an outer command/module to an xbundle-style project.

## Discover The Template

Before creating files, inspect the available template source:

- project-relative checkout named `xbundle-template/`
- package or console command named by the project
- README, examples, or CLI help
- `.gitmodules` or package metadata that points to the template

Use the commands documented by that template. Do not invent `init`, `create`, `add-case`, or `add-command` commands from memory.

When a checkout is present, read its README and then check CLI help for the actual interface. Typical entrypoints are a wrapper script such as `xbundle_template.py`, a Python module command, or a package-installed console command; use whichever the checkout documents.

If no template checkout or command is available, read `dependency-discovery.md` for the upstream link and clone guidance. Do not silently clone; ask the user or use an explicitly provided dependency path.

## Common Template Flow

After reading the template README and CLI help, use the documented generator flow for project init, optional TUI init, platform addition, and template updates. Treat any command names in memory as examples only; the checkout's README/help is authoritative.

The current `xbundle-template` checkout provides project-level commands such as
`init`, `tui`, `add-platform`, `update-template`, and `bootstrap-submodules`.
Generated projects provide a CMake wrapper, commonly `xbundle_add_program(...)`,
for one outer xbundle command/program. Do not assume a separate `add-case` or
`add-command` CLI exists unless the checkout's README/help documents it.

After project creation, the generated project owns the remaining workflow. Use its README, `xbundle/scripts/build.py --help`, `xbundle/scripts/ios_idbg.py --help`, and `xbundle/` config files to decide exact targets and arguments. Do not duplicate generated-project build/run instructions in skill output when those files are present.

## Create Project Or Command Skeletons

Use `xbundle-template` as scaffolding for:

- project roots
- outer command/program directories when the template provides them
- package metadata
- host or loader integration files when the template provides them

After generation, inspect the resulting CMake and README before adding custom logic. Keep generated structure aligned with the template instead of copying layout from an unrelated external project.

If the generated project includes `xbundle/scripts/`, `xbundle/config.json`, or `xbundle/cmake.json`, treat those generated files as the source of truth for targets, package names, and platform runners.

For normal C/C++ library dependencies, follow the generated project's CMake, CPM, FetchContent, or package-manager rules. Do not add unrelated library repos to xbundle-framework dependency discovery.

## Add An Outer Command

Prefer the template's own documented command-addition mechanism when it exists.
In the current template, additional command wiring is usually done in the
generated project style, for example through `xbundle/project.cmake` and
`xbundle_add_program(...)`.

An xbundle command/program is the host-visible command boundary. It is not the
same thing as a sbench leaf benchmark. For sbench projects, one xbundle command
usually hosts one benchmark or domain suite, while sbench leaves remain
subcommands inside that suite.

If the template only creates a project, add the command manually in the generated project's style:

- create reusable source files for command logic
- create a thin module entrypoint file
- add tests for reusable logic when the project has tests
- add package metadata only when the generated layout uses it

Register the command using the module/runtime rules in `module-runtime.md`, then verify the package shape with `project-composition.md`.

## Link And Path Policy

Do not hardcode local machine paths in reusable docs, generated examples, or skill output. Use project-relative paths in code and commands.

When referencing upstream projects, prefer links discovered from:

- `git remote -v`
- `.gitmodules`
- README or package metadata
- user-provided project links

If no trustworthy link is available, refer to the project by name and say the link should be discovered from the checkout.

Do not expose unrelated external project layouts as required skill structure.
