# Scheduler Control

Use this reference when a benchmark needs parameterized scheduling control. This file explains how to apply user-provided scheduler parameters; it does not discover topology, infer clusters, or define CPU numbering.

Prefer `sbench::BenchThreadFixture` when the generated project has it. This is the default multi-thread implementation path: derive from the fixture or reuse its style, call `RegisterOptions(app)` to expose `-t/--threads`, and run workers with `multi_threads_run_in_configs()` or the closest existing fixture runner.

## Parameters

Default behavior: if `-t/--threads` is omitted, use one default thread config with no CPU binding, no mask binding, and unspecified/default QoS. Passing `-t` replaces that default with parsed key/value thread specs.

Use the `sbench::BenchThreadFixture` `-t` style as the first choice:

```bash
./bench --iteration 100000 -t count=1
./bench --iteration 100000 -t bind_cpu=3,count=1
./bench --iteration 100000 -t bind_mask=0x3,count=2
./bench --iteration 100000 -t qos=1,count=1
```

The current fixture-style parser supports these fields:

- `count`: number of workers for the thread spec.
- `bind_cpu`: one logical CPU.
- `bind_mask`: CPU mask.
- `qos`: platform QoS or project abstraction.

Only use project-extension fields when the benchmark project already provides an adapter for them:

```bash
./bench --iteration 100000 -t priority=high,count=1
./bench --iteration 100000 -t policy=fifo,count=1
./bench --iteration 100000 -t cluster=performance,count=1
```

Extension parameter meanings:

- `priority`: platform priority or project abstraction.
- `policy`: project-defined scheduler policy.
- `cluster`: project-defined cluster parameter. This reference does not map cluster names to CPUs.

Do not pass `bind_cpu` and `bind_mask` in the same thread spec.

## Implementation Rules

- Keep a no-binding default path so every benchmark can run on unsupported platforms.
- Prefer `sbench::BenchThreadFixture` or its `-t` key/value style for ordinary multi-thread benchmarks. Do not reimplement `-t` parsing when the fixture is available.
- If `BenchThreadFixture` is not available, prefer `sbench`, `xcthread`, or `libxcpu` helpers over direct OS calls.
- Apply scheduling config after creating each worker and before entering the timed region.
- If the user explicitly requests an unsupported parameter, fail with `unsupported` or a non-zero exit instead of silently ignoring it.
- Do not infer cluster membership from CPU numbering. Accept `cluster` only when the project already maps that parameter to a supported platform action.
- For hand-managed workers, do not also accept global `-t`. Provide explicit options such as `--producer-thread` and `--consumer-thread`, parse each as a single thread config, and reject global thread configs if they were explicitly set.

## Platform Strategy

### macOS

- `qos`: supported through public pthread QoS APIs or a project helper.
- `priority`: supported only when a public API path or project helper exists.
- `bind_cpu` and `bind_mask`: supported only through an existing verified project helper; otherwise unsupported.
- `cluster`: unsupported by default. Do not infer performance or efficiency cluster from CPU numbering.

### iOS

- `qos`: supported through public pthread QoS APIs or a project helper.
- `priority`: supported only when a public API path or project helper exists.
- `bind_cpu` and `bind_mask`: unsupported by default; use only when the target project has a verified helper for that device/runtime.
- `cluster`: unsupported by default. Do not use private APIs and do not infer cluster membership from CPU numbering.

### Linux

- `bind_cpu`: use `sched_setaffinity` with a single-bit mask or a project helper.
- `bind_mask`: use `sched_setaffinity` with the requested CPU mask or a project helper.
- `priority`: use `nice`, pthread scheduler APIs, or a project helper; permissions may be required.
- `qos`: use only a project-defined mapping.
- `cluster`: use only if the project already maps the cluster parameter to a CPU mask or other supported action.

### Android

- `bind_cpu` and `bind_mask`: use `sched_setaffinity`, cpuset support, or a project helper when permitted.
- `priority`: use `nice`, pthread scheduler APIs, or a project helper; permissions may be required.
- `qos`: use only a project-defined mapping.
- `cluster`: use only if the project already maps the cluster parameter to a CPU mask or other supported action.

### OpenHarmony

- `bind_cpu` and `bind_mask`: use platform affinity APIs or a project helper.
- `priority`, `qos`, and `policy`: use only project-defined mappings or helpers.
- `cluster`: use only if the project already maps the cluster parameter to a CPU mask or other supported action.

## Result Metadata

Include lightweight scheduler metadata in output whenever scheduling affects the measurement. Use `bench-io.md` for the row shape and keep fields optional:

```json
{"thread":{"thread_id":0,"tid":12345,"bind_cpu":3}}
{"thread":{"thread_id":0,"tid":12345,"mask":"0x3"}}
{"thread":{"thread_id":0,"tid":12345,"qos":"user_interactive"}}
```

Use the same field name as the requested scheduling parameter when possible. For unsupported explicit requests, return non-zero and write a diagnostic. Do not emit a successful measurement row that hides the unsupported scheduling request.

## Validation

- Run one no-binding smoke test.
- Run one supported scheduling request for the target platform.
- Run one unsupported explicit request and verify it fails clearly.
- For frequency-sensitive or scheduling-sensitive benchmarks, verify lightweight scheduler metadata is present in the JSONL row.
