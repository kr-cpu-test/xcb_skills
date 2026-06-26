# Bench IO

Benchmark result rows should prefer JSONL: one flat JSON object per line. Bench I/O covers result serialization, stdout/stderr, file output, and xbundle runtime I/O/path adaptation.

JSONL result rows are success-only. For parameter errors, setup failures, unsupported runtime paths, output-open failures, scheduling failures, JIT map/protect/execute failures, or other abort conditions, write diagnostics to stderr and return non-zero or abort. Do not emit `{"status":"unsupported"}` or `{"status":"error"}` JSONL rows.

## Result Row Shape

Do not force one metric schema onto every benchmark. Prefer a flat, self-describing row with stable identity/context fields plus benchmark-specific result fields.

```json
{"bench":"hello","case":"smoke","status":"ok"}
{"bench":"memcpy","size":4096,"align":64,"time_ns":123456,"bytes":4096000}
{"bench":"dispatch","iteration":10000,"thread":{"thread_id":0,"tid":12345,"bind_cpu":3},"time_ns":1234,"measured_ops":10000,"ns_per_op":0.1234}
```

Common top-level fields:

- `bench`: stable benchmark or command name.
- `case`: optional short string label for a subcase or scenario.
- `thread`: object with `thread_id`, `tid`, `bind_cpu`, `mask`, `qos`, `priority`, `policy`, or `cluster` when scheduling affects the measurement.

Use result fields directly at the top level:

- Input and case parameters such as `iteration`, `size`, `align`, `stride`, `mode`, or `trial` can be top-level fields.
- Use nested objects only when a row would otherwise become too long or when fields are naturally grouped.
- `status`, `time_ns`, `cycles`, `instructions`, `bytes`, `pages`, `value`, or domain-specific names for successful direct measurements.
- `measured_ops` and `ns_per_op` only when the row represents a counted operation loop and `time_ns / measured_ops` is meaningful.
- Derived rates such as `bytes_per_s`, `ops_per_s`, or `cycles_per_op` only when the numerator and denominator are clear from the row.

## Output Option

Provide `--output`, defaulting to `stdout`. Treat `-` and `stdout` as stdout; otherwise truncate the output file at the start of a run and append rows.

## Output Sinks

Write result rows through an abstract output sink so reusable benchmark logic does not depend directly on `std::cout`, `FILE*`, or xbundle APIs.

For a regular CLI:

- `--output stdout`, `--output -`, and omitted `--output` write rows to process stdout.
- Any other `--output <path>` opens a normal file path, truncates it at the start of the run, then appends JSONL rows.
- Diagnostics and output-open failures go to stderr and return a non-zero exit code without writing a JSONL row.

For an xbundle module:

- stdout, stderr, and path I/O must be adapted through `xbundle_runtime` helpers.
- stdout rows must use `xbundle_write_stdout()` or `xbundle_printf()`.
- stderr diagnostics must use `xbundle_write_stderr()` or `xbundle_fprintf_stderr()`.
- The benchmark core should still write to the same abstract sink; only the entrypoint adapts the sink to xbundle runtime I/O.

If the same benchmark has both CLI and xbundle entrypoints, keep serialization and measurement code shared and make only the sink construction entrypoint-specific.

## Path Handling Under Xbundle Runtime

When an xbundle module accepts `--output <path>`, treat the path as a host-defined virtual path.

- Before writing a file, check host policy with `xbundle_path_info()` or `xbundle_path_can_write()`.
- Call `xbundle_resolve_path()` only when the lower-level file API requires a real host path.
- Treat successful resolve as path translation only; it does not grant write authorization by itself.
- If the virtual path is not writable or cannot be resolved when a real path is required, write a diagnostic to xbundle stderr and return non-zero.
- Do not emit a fake success JSONL row for output-path failures.

For xbundle benchmark modules, also read `xbundle-framework/references/module-runtime.md` for runtime I/O and path helper usage. Keep xbundle module registration and loader packaging in `xbundle-framework`, not in this benchmark output reference.

## Implementation Guidance

Use RapidJSON if the project already links it. Keep JSON construction structured; do not hand-concatenate JSON strings except for tiny smoke-only examples.

For hello world tasks that are not real measurements, plain text is acceptable. For benchmark rows, emit JSONL.
