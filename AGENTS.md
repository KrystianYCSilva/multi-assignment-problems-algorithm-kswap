# AGENTS.md — Coding Agent Instructions

## Project Overview

MAPA NSP Solver: C99 heuristic for the Nurse Scheduling Problem.
Modular architecture with pluggable operators (Shift, Swap, 2-Opt, kSwap, PCR, VND, VNS).

## Build Commands

```bash
# Build (produces build/mapa_kswap.exe)
bash build.sh

# Build + compile and run ALL tests
bash build.sh --tests

# Build only, then run a single test suite
bash build.sh
C:/Users/kryst/scoop/apps/gcc/15.2.0/bin/gcc.exe -std=c99 -Wall -Wextra -O2 -Isrc -Itests \
  tests/test_schedule.c build/*.o -lm -o build/test_schedule.exe
./build/test_schedule.exe
```

Replace `test_schedule` with `test_solvers` or `test_integration` for other suites.

There is no separate lint or typecheck command. The compiler warnings (`-Wall -Wextra`) serve as the linter.
Always verify a clean build with `bash build.sh` after changes.

## Test Framework

Custom minimal framework in `tests/test_framework.h`. No external dependencies.

- **Macros:** `ASSERT_TRUE`, `ASSERT_EQ`, `ASSERT_NEQ`, `ASSERT_LT`, `ASSERT_NOT_NULL`
- **Runner:** `RUN_TEST(fn)` — each test is a `static void fn(void)`
- **Main pattern:**
  ```c
  int main(void) {
      printf("=== Suite Name ===\n");
      RUN_TEST(test_foo);
      RUN_TEST(test_bar);
      TEST_SUMMARY();
      return TEST_EXIT();
  }
  ```
- **Suites:** `test_schedule.c` (99 asserts), `test_solvers.c` (13), `test_integration.c` (64)
- Every test must free all allocated memory (no leaks)

## LSP / clangd Warnings

CLion clangd reports false errors (`'stdlib.h' file not found`, undeclared `calloc`, etc.).
These are sysroot configuration issues — **ignore them**. The code compiles cleanly with gcc.

## Code Style

### Language & Standard
- **C99** (`-std=c99`). No C11/C23 features.
- Use `<stdbool.h>` for `bool`/`true`/`false`.

### Formatting
- **4-space indentation**, no tabs
- **K&R brace style** (opening brace on same line as statement)
- Single blank line between functions, no blank line after opening brace
- Line length: aim for ~80 chars, hard wrap at ~100
- No trailing whitespace

### Naming Conventions
- **Types:** `PascalCase` — `Schedule`, `NspInstance`, `Constraints`, `VndConfig`, `OperatorEntry`
- **Functions:** `snake_case` prefixed by module — `schedule_create`, `cost_evaluate`, `kswap_forward`, `vnd_solve`
- **Variables:** `snake_case` — `n_nurses`, `best_cost`, `day_start`
- **Constants/Macros:** `UPPER_SNAKE_CASE` — `SHIFT_MORNING`, `PENALTY_HARD`, `VND_MAX_NEIGHBOURHOODS`
- **Files:** `snake_case.c/.h` — `shift_relocate.c`, `lap_solver.h`, `operator_registry.c`
- **Static (file-private) functions:** `snake_case` without module prefix — `static void wrap_shift(...)`
- **Function pointer typedefs:** `PascalCase` ending in `Fn` — `LapSolverFn`, `NeighbourhoodFn`

### Include Guards & Includes
- Use `#ifndef MAPA_MODULE_H` / `#define MAPA_MODULE_H` / `#endif` (not `#pragma once`)
- Include order in `.c` files:
  1. Own header (`#include "foo.h"`)
  2. Other project headers (`#include "../core/types.h"`)
  3. Standard library headers (`#include <stdio.h>`)
- Headers include only what they need; `types.h` is the root dependency

### Function Signatures
- Read-only params: `const Type *ptr`
- Output/mutated params: `Type *ptr` (first parameter for the "self" object)
- Standard operator signature: `void op(Schedule *s, const NspInstance *nsp, const Constraints *c, LapSolverFn solver)`
- Return `Type *` for constructors (caller owns), `NULL` on failure
- Cast `calloc` return: `(Type *)calloc((size_t)n, sizeof(Type))`

### Memory Management
- **Allocate with `calloc`**, not `malloc` (zero-initialized)
- Always check for `NULL` after allocation
- Every `*_create` must have a matching `*_free`
- Use `alloc_matrix_int` / `free_matrix_int` from `types.h` for 2D int arrays
- `schedule_copy` performs deep copy; caller must `schedule_free` the result
- Unused parameters: `(void)param;` to suppress warnings

### Error Handling
- Return `NULL` from allocators/constructors on failure
- Return `-1` from int-returning functions on error, `0` on success
- Print errors to `stderr` via `fprintf(stderr, "Error: ...")`
- No `assert()` in production code; assertions only in tests via `ASSERT_*` macros
- No `exit()` except in `main.c` CLI argument parsing

### Comments
- **Do NOT add comments** unless explicitly requested
- No doc comments, no inline comments, no TODO markers
- Code should be self-documenting through clear naming

## Directory Structure

```
src/
  core/           types.h, schedule.h/.c, cost.h/.c
  solvers/        lap_solver.h/.c (Hungarian, APC, BAP)
  heuristics/     constructive, pcr, kswap, shift_relocate, swap_exchange,
                  two_opt, vnd, vns, operator_registry
  benchmark/      benchmark.h/.c
  io/             nsp_reader.h/.c, result_writer.h/.c
  main.c
tests/            test_framework.h, test_schedule.c, test_solvers.c, test_integration.c
files/            NSPLib instances (N25, N30, N50, N60, N75, N100) + constraint files
docs/             Algorithm pseudocode reference
```

## Adding a New Operator

1. Create `src/heuristics/my_op.h` and `src/heuristics/my_op.c`
2. Follow the `NeighbourhoodFn` signature for the core function
3. Implement both `_forward` and `_backward` variants (backward = reverse days, run forward, reverse back)
4. Register in `operator_registry.c` → `registry_load_defaults()`
5. Add wrapper in `vnd.c` if it should be a VND neighbourhood
6. Add `.c` to `LIB_SOURCES` in both `build.sh` and `CMakeLists.txt`
7. Add tests in `test_integration.c`

## Key Types

- `Schedule` — nurse_schedule[nurse][day] = shift (int), plus cost/violations
- `NspInstance` — coverage_matrix + preference_matrix from NSPLib files
- `Constraints` — assignment bounds, consecutive work/shift limits
- `LapSolverFn` — `int (*)(int n, int **cost, int *assignment)` — pluggable LAP solver
- `NeighbourhoodFn` — `void (*)(Schedule*, const NspInstance*, const Constraints*, const LapSolverFn)` — pluggable operator

## Environment

- **OS:** Windows (Git Bash)
- **Compiler:** GCC 15.2.0 at `C:/Users/kryst/scoop/apps/gcc/15.2.0/bin/gcc.exe`
- **No cmake, no make** — use `bash build.sh` only
- **Project root:** `C:\Users\kryst\CLionProjects\mapa_kswap`
