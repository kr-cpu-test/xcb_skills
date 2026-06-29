# Sbench Registration

Use this reference when writing C++ code that exposes benchmark commands through sbench. Prefer the `SBENCH_*` macros from `sbench/register.hpp`.

Contents: macro roles, leaf bench, domain suite, multi-bench capabilities, nested suite, build modes, and linkage rules.

## Macro Roles

- `SBENCH_BENCH_FACTORY(name)`: define one `MicroBenchFactory` accessor in exactly one translation unit.
- `SBENCH_BENCH_REGISTER_INTO(BenchClass, factory_name)`: register a `MicroBench` or `ForkableBench` class into a named factory.
- `SBENCH_BENCH_REGISTER(BenchClass)`: register directly into the default sbench suite factory; use this rarely because domain-local factories usually give cleaner grouping.
- `SBENCH_SUITE_ENTRY(SuiteClass)`: expose a suite according to the target build mode.
- `SBENCH_SUITE_MAIN(SuiteClass)`: emit only a split-executable `main()` when the build defines `SBENCH_BUILD_SPLIT_EXE`.
- `SBENCH_SUITE_REGISTER(SuiteClass)`: register directly into the default sbench suite factory; prefer `SBENCH_SUITE_ENTRY` unless unconditional registration is intentional.

Do not reimplement these macros in benchmark code. Include `sbench/register.hpp` in translation units that define factories, leaf registrations, suite registrations, or suite entrypoints.

## Leaf Bench

Use `MicroBench` for one runnable measurement command. Keep CLI options, mutable state, setup, measurement loop, result schema, and validation local to the leaf unless multiple leaves genuinely share a suite-root fixture/support/common capability.

```cpp
#include "sbench/register.hpp"

#include <cstddef>

class CacheProbeBench : public MicroBench {
public:
  CacheProbeBench() : MicroBench("cache_probe") {
    app->description("Cache probe benchmark");
    app->add_option("--bytes", bytes_, "Working set size in bytes");
    app->callback([this]() { run(); });
  }

private:
  std::size_t bytes_ = 32768;
  void run();
};

SBENCH_BENCH_REGISTER_INTO(CacheProbeBench, domain_bench_factory);
```

The registration key is the C++ class name; the CLI command name is controlled by the `MicroBench` constructor or `app->name(...)`.

## Domain Suite

This is the preferred pattern when multiple benchmark leaves belong to the same benchmark domain and should be hidden behind one runner command. The unified runner sees one suite. The leaf commands stay in a domain-local factory and appear under that suite.

```cpp
#include "sbench/register.hpp"

#include <memory>

SBENCH_BENCH_FACTORY(domain_bench_factory);

class DomainSuite : public ForkableBench {
public:
  DomainSuite() : ForkableBench(domain_bench_factory_get_instance()) {
    app->name("domain");
    app->description("Benchmark domain suite");
  }

protected:
  std::unique_ptr<ForkableBench> create_fork_instance() const override {
    return std::make_unique<DomainSuite>();
  }
};

SBENCH_SUITE_ENTRY(DomainSuite);
```

Use a domain suite when users naturally run and compare those leaves together. Do not flatten every leaf into the default suite factory unless each leaf is meant to be a separate top-level runner command.

## Multi-Bench Capabilities

sbench multi-bench support is centered on `BenchSuite` and `ForkableBench`:

- `MicroBench` represents one runnable leaf command with its own CLI options and callback.
- `MicroBenchFactory` collects leaf adapters through static registration.
- `BenchSuite(MicroBenchFactory&)` turns the factory contents into CLI subcommands.
- `ForkableBench` adds suite-level fork execution while still exposing the leaf subcommands.
- `set_logger()` propagates the suite logger to child benches, so a host adapter can redirect stdout, stderr, and log domains once at the suite boundary.
- `MicroBench::main(std::string, bool)` allows host adapters to pass shell-like argument strings without rebuilding CLI parsing.

For multi-bench projects, default to one domain factory, one `ForkableBench` domain suite, and many registered leaf adapters. Shared fixture/support/common code lives at the suite root and is linked by leaves that need it. Keep benchmark core logic outside the sbench classes; register the sbench adapters, not the reusable measurement core.

For xbundle exposure, default to one xbundle command module per benchmark or multi-bench domain. The module should create the domain suite, attach an xbundle-backed logger/output adapter when needed, and call `suite.main(argc, argv)`. Do not generate one `XBUNDLE_MAIN` per leaf bench unless the leaves are truly separate host-discovered commands with different dependency, platform, or lifecycle requirements.

## Nested Suite

Use a nested suite when a domain contains a subdomain that should be grouped as a subcommand. Define one factory for the subdomain, register leaves into it, then register the subdomain suite into the parent domain factory.

```cpp
#include "sbench/register.hpp"

#include <memory>

SBENCH_BENCH_FACTORY(frontend_bench_factory);

class FrontendSuite : public ForkableBench {
public:
  FrontendSuite() : ForkableBench(frontend_bench_factory_get_instance()) {
    app->name("frontend");
    app->description("Frontend pipeline benchmark suite");
  }

protected:
  std::unique_ptr<ForkableBench> create_fork_instance() const override {
    return std::make_unique<FrontendSuite>();
  }
};

SBENCH_BENCH_REGISTER_INTO(FrontendSuite, domain_bench_factory);
```

Do not also call `SBENCH_SUITE_ENTRY(FrontendSuite)` unless the subdomain should appear both as a nested command and as its own top-level suite.

## Build Modes

`SBENCH_SUITE_ENTRY(SuiteClass)` is intentionally build-controlled:

- With `SBENCH_BUILD_SPLIT_EXE`, it emits `main()` for a standalone executable of that suite.
- With `SBENCH_BUILD_SINGLETON`, it registers the suite into the default sbench suite factory for a unified runner.
- If neither mode is defined, it does not expose the suite; use that only for pure support/library targets.

A multi-domain runner can link several suite translation units built with `SBENCH_BUILD_SINGLETON`; each `SBENCH_SUITE_ENTRY` registers one domain suite into the default suite factory.

## Linkage Rules

Static registration runs only when the object file is linked into the final target. Prefer generated project helpers that compile all leaf registration translation units into the bench target, or use an object library/link-whole strategy when registrations live behind a static library.

Keep each `SBENCH_BENCH_FACTORY(factory_name)` in one translation unit. Leaf translation units may use `SBENCH_BENCH_REGISTER_INTO(..., factory_name)`; the macro declares the factory accessor and registers during static initialization.
