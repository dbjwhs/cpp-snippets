# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Repository shape

This is a collection of independent, self-contained C++ snippets. Each snippet lives in its own directory under a topical category (`algorithms/`, `concurrency/`, `data-structures/`, `design-patterns/{creational,structural,behavioral}/`, `language-features/{cpp20,cpp23,core}/`, `networking/`, `programming-paradigms/`, `system-programming/`, `utilities/`, `examples/`) and ships its own `CMakeLists.txt`. There is no top-level CMake — snippets are not linked to one another and are built independently.

Standard snippet layout:
```
snippet-name/
├── CMakeLists.txt
├── README.md
├── include/snippet.hpp   # optional
└── src/snippet.cpp
```

Snippets default to C++20 (`set(CMAKE_CXX_STANDARD 20)`) and `cmake_minimum_required(VERSION 3.30)`. A few snippets pin to C++17 or pull in external packages (e.g. `utilities/dates/` uses `find_package(CURL)` and Howard Hinnant's `date` library).

## Build commands

Build a single snippet:
```bash
cd <category>/<snippet>
mkdir -p build && cd build
cmake ..
make
```

Build everything from the repo root via the helper scripts in `tooling/`:
```bash
./tooling/build_all.sh              # walks every CMake project, builds each in its own build/
./tooling/build_all.sh --dry-run    # -d: report what would build, build nothing
./tooling/build_all.sh --run        # -r: execute each binary after a successful build
./tooling/clean_all.sh              # remove every build/ directory
./tooling/clean_all.sh --dry-run
```

`build_all.sh` excludes `language-features/cpp20/modules` (modules build is fragile). There is no separate test runner — snippets are demo executables, and `--run` is how you exercise them in bulk.

## The shared header (`headers/project_utils.hpp`)

Almost every snippet `#include`s `headers/project_utils.hpp` via a relative path like `"../../../headers/project_utils.hpp"`. This single header provides the project's de facto runtime library — there is no separate utils target to link against. Notable contents:

- `Logger` — thread-safe singleton with file + stdout/stderr sinks, per-level enable/disable, and a `Logger::StderrSuppressionGuard` RAII helper. Access via `Logger::getInstance()` or `getInstancePtr()`. Default log file path is `../custom.log` relative to the binary's working directory, which is why every snippet's build directory grows a `custom.log`.
- `LOG_INFO` / `LOG_WARNING` / `LOG_ERROR` / `LOG_CRITICAL` / `LOG_DEBUG` / `LOG_NORMAL` macros, plus `*_PRINT` variants gated on `__cpp_lib_print` that take `std::format_string`.
- `LogLevel` enum, `RandomGenerator`, `utils::split`, `utils::generate_guid` (UUID v4), `threadIdToString`.
- `DECLARE_NON_COPYABLE` / `DECLARE_NON_MOVEABLE` macros.

When adding a new snippet, follow the existing convention: include this header, log via the macros, and use `StderrSuppressionGuard` around code paths that intentionally produce errors during testing.

## Linux build prerequisite

`std::format` requires GCC 13 or newer. GCC 12 ships an incomplete `<format>` and snippets that use it will fail with `fatal error: format: No such file or directory` from the command line even when CLion succeeds. See `linux-build-notes.md` and the README for the `ppa:ubuntu-toolchain-r/test` upgrade steps.

## Conventions for new snippets

- Place under the appropriate category directory; mirror the `include/` + `src/` + `CMakeLists.txt` + `README.md` layout.
- C++20 unless there's a reason otherwise. Keep the MIT/copyright header in `CMakeLists.txt` and source files (matches existing files; `tooling/update_header_license.sh` enforces this if re-enabled in the pre-commit hook).
- Reach for `Logger` and the `LOG_*` macros instead of raw `std::cout` so output is timestamped and thread-tagged consistently with the rest of the repo.
- After adding a snippet, regenerate the README index with `tooling/generate_snippets_index.py` if updating the index manually is tedious.

## Git hooks

`.githooks/pre-commit` exists but its body is currently commented out (it just `exit 0`s). To enable hooks on a fresh clone: `git config core.hooksPath .githooks`. The two scripts the hook would run are `tooling/new_line.sh` and `tooling/update_header_license.sh`.
