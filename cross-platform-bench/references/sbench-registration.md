# Sbench Registration

Use this reference when organizing benchmark projects that expose commands
through sbench. It is a project-usage guide, not the source of truth for sbench
API definitions.

Read sbench docs for API facts and exact macro behavior:

- `sbench/docs/registration.md`: dynamic registration, suite entry build modes,
  registration translation unit ownership, and shared runtime linking.
- `sbench/docs/api.md`: public classes, macros, fixtures, and helper functions.
- `sbench/docs/architecture.md`: runtime structure, execution flow, factory and
  suite relationships, and thread model.
- `sbench/include/sbench/sbench.hpp` and `sbench/include/sbench/register.hpp`:
  exact definitions.

Keep this file for cross-platform-bench decisions: suite shape, domain grouping,
xbundle composition, and reusable measurement-code boundaries.

## Project Roles

Separate benchmark projects into these roles:

- support library: reusable measurement logic, parsers, kernels, fixtures,
  output helpers, platform helpers, and tests;
- sbench adapter translation unit: defines a `MicroBench` or suite class and
  uses `SBENCH_*` registration macros;
- suite entrypoint: creates or exposes a domain suite and runs
  `suite.main(argc, argv)`;
- final runner/module target: the CLI wrapper or xbundle `_icmd` module loaded
  by the host.

Keep benchmark core logic outside sbench adapter classes. The adapter should
parse CLI options, call reusable measurement code, and emit the benchmark result
through the project output sink.

## xbundle + sbench Target Topology

Use this topology when one benchmark or domain is exposed through both sbench
and an xbundle command module:

```text
measurement core
  -> support library
     -> tests
     -> optional CLI runner
     -> xbundle module entrypoint

sbench adapter / registration TU
  -> MicroBench or domain suite adapter
  -> calls measurement core
  -> linked into final runner/module image

optional local CLI runner
  -> links support + sbench adapters + sbench runtime
  -> used for local --help and smoke tests

module_main.cpp
  -> XBUNDLE_INFO / XBUNDLE_MAIN
  -> constructs or invokes the sbench bench/suite
  -> adapts stdout, stderr, and paths through xbundle runtime

final <command_name>_icmd
  -> links support + sbench adapter objects + sbench runtime + xbundle runtime
  -> discovered and executed by the host loader
```

Do not put measurement logic directly in `XBUNDLE_MAIN`, do not create one
xbundle command module per leaf by default, and do not hide sbench registration
translation units in a library that is not linked into the final loaded image.

## Single Bench

For one measurement, proof, smoke command, or parameter sweep, keep one
single-bench shape. Do not add multi-bench registry directories or domain suite
machinery unless the xbundle project already provides them.

Use a `MicroBench` adapter when the project wants sbench CLI behavior. Otherwise
keep the measurement core reusable and let the xbundle module or CLI wrapper
call it directly.

## Multi-Bench Suite

Use multi-bench only for an explicit suite/domain workflow: unified runner,
grouped domains/subdomains, long-lived benchmark collection, or repeated future
leaf additions.

Default to this shape:

- one domain-local registration/factory path;
- many leaf adapters registered into that factory;
- one domain suite exposed to the unified runner or xbundle module;
- shared fixture/support/common code at the suite root, linked by leaves that
  need it.

Use the ordinary sbench suite shape by default. Use `ForkableBench` only when
the project needs suite-level `--fork` execution or already uses that pattern.
For exact `BenchSuite`, `ForkableBench`, factory, and registration APIs, read
`sbench/docs/api.md` and `sbench/docs/registration.md`.

Use a nested suite only when a domain contains a subdomain that should appear as
a grouped subcommand. Register the subdomain suite into the parent domain
factory. Do not also expose that subdomain as a top-level suite unless the user
asks for both command paths.

## xbundle Composition

For xbundle exposure, default to one xbundle command module per benchmark or
multi-bench domain. Individual benchmark leaves normally remain sbench
subcommands, not separate host-discovered modules.

The module entrypoint should:

1. construct the single bench or domain suite;
2. attach an xbundle-backed logger/output adapter when the project uses one;
3. call the sbench `main(argc, argv)` path;
4. leave module registration, loader exposure, path handling, and runtime I/O
   to `xbundle-framework`.

Do not create one `XBUNDLE_MAIN` per leaf bench unless the leaves are truly
separate host-discovered commands with different dependencies, platform
requirements, or lifecycle.

## Linking Policy

Use `sbench/docs/registration.md` for exact registration ownership, suite entry
build modes, object/link-whole choices, shared-runtime packaging, and validation
commands.

For benchmark-project policy, keep only these decisions here:

- project registration adapters must be included in the final runner or xbundle
  `_icmd` module image by the generated project mechanism;
- the generic sbench runtime is a project-provided dependency, not a place for
  project-specific leaf registration;
- prefer generated add-leaf helpers for sbench leaves and the xbundle command
  wrapper for the outer module before hand-writing registration target wiring;
- after packaging, validate loader command discovery and sbench subcommand
  visibility through the project docs.
