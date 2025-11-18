# Repository Guidelines

## Project Structure & Module Organization
Root-level directories `01-basics/` through `08-advanced-patterns/` mirror the tutorial outline; keep new lessons in the matching chapter folder and mirror the naming used by the preceding chapters. Runtime code lives in two orchestrated trees: `examples/` (with `basic/`, `intermediate/`, `advanced/` subfolders) for self-contained demos, and `exercises/` for student tasks plus reference implementations under `exercises/solutions/`. Top-level `CMakeLists.txt` registers both trees and emits every executable into `build/bin/`, so avoid checked-in binaries; treat the existing `build/` directory as disposable.

## Build, Test, and Development Commands
- Configure once with `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug` to enable C++20, strict warnings, and an out-of-source build.
- Compile everything via `cmake --build build`, or target a single artifact such as `cmake --build build --target 04_higher_order_functions_exercises` when iterating quickly.
- Run any binary directly from `build/bin/` (e.g., `./build/bin/01_basics_exercises` or `./build/bin/06_algebraic_types_solutions`) to smoke-test the associated chapter.
- Use `cmake --build build --target clean` before pushing if you generated custom executables.

## Coding Style & Naming Conventions
Write modern C++20 that favors `<algorithm>`, `<numeric>`, and lambdas over manual loops, and prefer `const` data plus pure functions to reinforce the functional programming goals of the repo. Follow the established conventions visible in the exercises: four-space indentation, no tabs, braces on the same line as the declaration, `snake_case` for functions/files, and PascalCase for types such as `Person`. Keep helper lambdas small and capture state explicitly; if mutability is unavoidable, isolate it inside obvious utility functions and document the reasoning inline.

## Testing Guidelines
There is deliberately no external test harness; every `*_exercises.cpp` already embeds diagnostic code that prints the expected results. After building, run the relevant executable from `build/bin/` and verify the sample inputs match the expected output text before committing. When fixing or extending solutions, keep the existing expectations intact and add new scenarios by extending the local `test_exercise_*` helpers so future learners can still compare their answers without additional tooling.

## Commit & Pull Request Guidelines
With no commit history yet, establish a consistent pattern now: use short, imperative messages scoped by chapter (for example, `exercises: finalize sum_of_even_squares`). Every pull request should link the motivating issue or lesson, describe observable changes, and include the exact commands you ran (e.g., `cmake --build build && ./build/bin/03_pure_functions_exercises`). If the change affects console output, add before/after snippets or screenshots so reviewers can verify behavior without re-running the entire suite.
