# JIT And libxcpu

Use `libxcpu` when the bench needs its capabilities.

## Useful Libraries

- `xcthread`: thread creation, QoS, CPU binding policy.
- `xcmem`: executable and data memory mapping, protection changes, cache flushing.
- `xcjit`: asmjit-backed quick compile helpers.
- `xcperf`: timing helpers such as `TimeMetrics`.
- `xcplat`: platform capability helpers such as page size.
- `xcpath`: platform path helpers.

## Timing

Use steady-clock based timing or `xcperf::TimeMetrics` style APIs. Keep warmup separate from measured loops.

For counted instruction loops, compute operation count from explicit inputs. Example fields for a frequency-style add-chain bench: `iteration`, `chain_length`, `measured_ops`, `time_ns`, `ns_per_op`, and `estimated_hz`.

## JIT Memory Pattern

For AArch64 JIT benches:

1. Allocate writable code memory with `xcmem`.
2. Emit code with `xcjit` / asmjit.
3. Flush instruction cache.
4. Protect code memory as read+execute before calling it.
5. Keep data memory separate from code memory unless the test intentionally couples them.

Prefer the `CTMemoryChunk` overload of `XCJit::quick_compile()` so split JIT mappings use `get_ptr()` for writes and `get_rx_ptr()` for relocation/execution. Use `CTM_MAP_JIT` when the target runtime needs a JIT mapping.

## iOS xbundle JIT Pattern

Use this only when implementing an iOS JIT benchmark command that runs as an xbundle `_icmd` through an idbg-style host with `--enable-jit`. For ordinary CLI benches, use the normal `xcmem`/`xcjit` pattern above.

On current iOS debug/JIT flows, the reliable command-module pattern is RX original plus RW alias:

1. Create the original mapping with `mmap(..., PROT_READ | PROT_EXEC, MAP_PRIVATE | MAP_ANON, ...)`.
2. Create a writable alias with `vm_remap(copy=false)` from the RX original mapping.
3. Export a visible `NOTIFY_DEBUGGER_ABOUT_RX_PAGES(void *base, size_t size)` function.
4. Call `NOTIFY_DEBUGGER_ABOUT_RX_PAGES(rx_base, size)` before writing code.
5. Change the alias to `PROT_READ | PROT_WRITE`.
6. Write generated code through the RW alias.
7. Flush the instruction cache for the RX entry range.
8. Execute the entry point from the original RX mapping, not from the RW alias.

Do not create a RW original and then derive RX from it for iOS JIT command modules. That direction can appear to map/protect correctly but still fail when fetching instructions on newer iOS versions.

Minimal pieces:

```cpp
#if defined(__APPLE__)
#include <mach/mach.h>
#include <sys/mman.h>
#include <unistd.h>
#include <libkern/OSCacheControl.h>
#endif

extern "C" __attribute__((noinline, used, visibility("default")))
void NOTIFY_DEBUGGER_ABOUT_RX_PAGES(void *base, size_t size) {
  asm volatile("" : : "r"(base), "r"(size) : "memory");
}

struct IosJitMap {
  void *rw = nullptr;
  void *rx = nullptr;
  size_t size = 0;
};

bool map_ios_jit(size_t size, IosJitMap *out) {
  void *rx = mmap(nullptr, size, PROT_READ | PROT_EXEC,
                  MAP_PRIVATE | MAP_ANON, -1, 0);
  if (rx == MAP_FAILED) {
    return false;
  }

  vm_address_t rw_addr = 0;
  vm_prot_t cur = 0;
  vm_prot_t max = 0;
  kern_return_t kr = vm_remap(mach_task_self(), &rw_addr, size, 0,
                              VM_FLAGS_ANYWHERE, mach_task_self(),
                              reinterpret_cast<vm_address_t>(rx), false,
                              &cur, &max, VM_INHERIT_NONE);
  if (kr != KERN_SUCCESS) {
    munmap(rx, size);
    return false;
  }

  NOTIFY_DEBUGGER_ABOUT_RX_PAGES(rx, size);

  if (mprotect(reinterpret_cast<void *>(rw_addr), size,
               PROT_READ | PROT_WRITE) != 0) {
    vm_deallocate(mach_task_self(), rw_addr, size);
    munmap(rx, size);
    return false;
  }

  *out = {reinterpret_cast<void *>(rw_addr), rx, size};
  return true;
}
```

When the command is run without the helper, fail with a stderr diagnostic such as `rerun through --enable-jit` and return non-zero.

## Minimal AArch64 Callable Skeleton

Use this shape for small generated functions.

```cpp
#if !defined(__aarch64__)
#error "AArch64 JIT benchmark requires __aarch64__"
#endif

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <asmjit/arm/a64assembler.h>
#include "xcjit/jit.hpp"
#include "xcmem/alloc.hpp"
#include "xcperf/time_metrics.hpp"

using JitFn = uint64_t (*)(uint64_t iteration, uint64_t seed);

struct JitProgram {
  std::shared_ptr<CTMem::CTMemoryChunk> code;
  JitFn entry = nullptr;
  size_t code_size = 0;
};

JitProgram build_add_chain_program(size_t chain_length) {
  if (chain_length == 0) {
    throw std::invalid_argument("chain_length must be greater than zero");
  }

  CTMem::CTMapAttr map_attr{};
  map_attr.flags = CTMem::CTMapFlag::CTM_MAP_JIT;

  auto code = CTMem::CTMemoryAllocator::ctm_mmap(
      0, 4096,
      CTMem::CTMemProt::CTM_PROT_READ | CTMem::CTMemProt::CTM_PROT_WRITE,
      &map_attr);

  size_t emitted_size = 0;
  XCJit::quick_compile(
      *code, &emitted_size, [chain_length](asmjit::a64::Assembler &as) {
        using namespace asmjit::a64;
        auto loop = as.newLabel();
        auto done = as.newLabel();

        as.mov(x2, x1);       // accumulator = seed
        as.cbz(x0, done);     // iteration count in x0
        as.bind(loop);
        for (size_t i = 0; i < chain_length; ++i) {
          as.add(x2, x2, 1);  // dependency chain
        }
        as.subs(x0, x0, 1);
        as.b_ne(loop);
        as.bind(done);
        as.mov(x0, x2);       // return accumulator
        as.ret(x30);
        return true;
      });

  code->mem_protect(CTMem::CTMemProt::CTM_PROT_READ |
                    CTMem::CTMemProt::CTM_PROT_EXEC);

  return {code, reinterpret_cast<JitFn>(code->get_rx_ptr()), emitted_size};
}

uint64_t run_timed_ns(const JitProgram &program, uint64_t iteration) {
  if (program.entry == nullptr) {
    throw std::invalid_argument("JIT program entry is null");
  }

  (void)program.entry(1000, 0); // warmup outside measured region

  TimeMetrics tm;
  tm.start_count();
  volatile uint64_t sink = program.entry(iteration, 0);
  tm.stop_count();
  (void)sink;
  return TimeMetrics::to_ns(tm.count());
}
```

For an add dependency chain, `measured_ops = iteration * chain_length` when overflow checks pass. `estimated_hz = measured_ops * 1e9 / time_ns` is only meaningful when the benchmark intentionally treats one dependent add as the unit being timed.

## Failure Handling

Use `bench-io.md` failure rules. Treat invalid parameters, non-AArch64 builds, failed JIT map/write/flush/protect, failed function call setup, or other setup/runtime failures as stderr plus non-zero/abort with no JSONL row.

Prefer catching exceptions at the CLI command boundary so lower-level JIT helpers can throw naturally while the command still reports a clear error.

## CMake

Link only what is needed:

```cmake
project_bench_link_libraries(my_bench
  PRIVATE
    libxcpu
    RapidJSON
)
```

Add `capstone` only if the bench dumps or disassembles generated code.

For local `libxcpu` development, configure the generated project with `-DSBENCH_LIBXCPU_SOURCE_DIR=<libxcpu-checkout>` so `sbench` uses that checkout.
