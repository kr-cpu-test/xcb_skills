# Xbundle Template Workflow

Use this reference when starting from `xbundle-template` or adding a generated module or case to an xbundle-style project.

## Discover The Template

Before creating files, inspect the available template source:

- project-relative checkout named `xbundle-template/`
- package or console command named by the project
- README, examples, or CLI help
- `.gitmodules` or package metadata that points to the template

Use the commands documented by that template. Do not invent `init`, `create`, or `add-case` commands from memory.

When a checkout is present, read its README and then check CLI help for the actual interface. Typical entrypoints are a wrapper script such as `xbundle_template.py`, a Python module command, or a package-installed console command; use whichever the checkout documents.

If no template checkout or command is available, read `dependency-discovery.md` for the upstream link and clone guidance. Do not silently clone; ask the user or use an explicitly provided dependency path.

## Common Template Flow

After reading the template README and CLI help, a typical flow is:

```sh
./xbundle_template.py init <project>
./xbundle_template.py init --tui <project>
./xbundle_template.py add-platform <project> --platform <platform>
./xbundle_template.py update-template <project> --dry-run
./xbundle_template.py update-template <project>
```

Treat these as common entrypoint examples, not a replacement for the checkout's README/help. If the project uses a package-installed command or Python module entrypoint instead of `./xbundle_template.py`, use the documented entrypoint.

After project creation, the generated project usually owns the remaining workflow:

```sh
cd <project>
python3 scripts/build.py
python3 scripts/build.py --target <platform>:<kind>
python3 scripts/ios_idbg.py <build|install|connect|run> ...
```

Use the generated project README and config files to decide exact targets and arguments.

## Create Project Or Module Skeletons

Use `xbundle-template` as scaffolding for:

- project roots
- module directories
- case directories
- package metadata
- host or loader integration files when the template provides them

After generation, inspect the resulting CMake and README before adding custom logic. Keep generated structure aligned with the template instead of copying layout from an unrelated external project.

If the generated project includes `scripts/build.py`, `scripts/ios_idbg.py`, `xbundle/config.json`, or `xbundle/cmake.json`, treat those generated files as the source of truth for targets, package names, and platform runners.

For normal C/C++ library dependencies, follow the generated project's CMake, CPM, FetchContent, or package-manager rules. Do not add unrelated library repos to xbundle-framework dependency discovery.

## Add A Case

Prefer the template's own "add case", "add command", or "add module" command when it exists. If the template only creates a project, add the case manually in the generated project's style:

- create a reusable source file for case logic
- create a thin module entrypoint file
- add tests for reusable logic when the project has tests
- add package metadata only when the generated layout uses it

Register the case in the build with `xbundle_add_module(NAME <case_name> ...)`, then package the output under `lib/` so the loader can expose it.

## Link And Path Policy

Do not hardcode local machine paths in reusable docs, generated examples, or skill output. Use project-relative paths in code and commands.

When referencing upstream projects, prefer links discovered from:

- `git remote -v`
- `.gitmodules`
- README or package metadata
- user-provided project links

If no trustworthy link is available, refer to the project by name and say the link should be discovered from the checkout.

Do not expose unrelated external project layouts as required skill structure.
