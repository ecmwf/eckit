# Contributing to eckit

This document is the practical reference for how to add or modify code in this
repository, written so that both new contributors and AI coding agents can
produce changes that match the existing house style on the first try.

For deeper architectural context, see [`AGENTS.md`](AGENTS.md) and
[`docs/`](docs/).

---

## Project structure

`eckit` is a C++17 toolkit built with [`ecbuild`](https://github.com/ecmwf/ecbuild)
on top of CMake. The repository is organised as follows:

| Path | Purpose |
| --- | --- |
| `src/eckit/`              | Public sources of the main `eckit` shared library. Each subdirectory is a logical module (`config`, `container`, `filesystem`, `io`, `log`, `runtime`, `system`, `thread`, `utils`, ...). |
| `src/eckit/<feature>/`    | Optional libraries gated by `ecbuild_add_option(FEATURE ...)` in the root `CMakeLists.txt`: `eckit_mpi`, `eckit_linalg`, `eckit_geometry`, `eckit_maths`, `eckit_web`, `eckit_cmd`, `eckit_codec`, `eckit_spec`, `eckit_geo`, `eckit_sql`. Each has its own `CMakeLists.txt`. |
| `src/eckit/testing/`      | Header-only testing harness (`Test.h` — `CASE`, `EXPECT`, `EXPECT_EQUAL`, `EXPECT_THROWS_AS`, `SetEnv`, `run_tests`). |
| `src/tools/`              | Source of installed CLI tools. CMake target `eckit_foo` produces installed binary `eckit-foo`. |
| `tests/<module>/`         | One CMakeLists per module. New tests are declared there with `ecbuild_add_test(TARGET eckit_test_<module>_<name> ...)`. |
| `docs/`                   | Sphinx documentation. `make -j html` from `docs/`; warnings are errors. |
| `python/eckit/`           | Cython wrapper. Builds against an already-built C++ eckit. |
| `bamboo/`, `tools/`       | CI helpers. |
| `cmake/`                  | Project-specific CMake modules. |

Generated headers like `eckit_config.h` and `eckit_version.h` are written into
the build tree. Compilation needs both `src/` and `build/src/` on the include
path; `ecbuild` handles this for in-tree targets.

### Where to put new code

* **New class in an existing module** — add `Foo.h` and `Foo.cc` next to its
  siblings under `src/eckit/<module>/` and append to that module's
  `CMakeLists.txt`.
* **New module** — create `src/eckit/<module>/` with its own `CMakeLists.txt`,
  add an `ecbuild_add_option` entry in the root `CMakeLists.txt`, and
  define an `eckit_HAVE_<MODULE>` config macro.
* **New test** — add the source under `tests/<module>/` and register it in
  that directory's `CMakeLists.txt`.

---

## Build, test, format

`ecbuild` must be on `PATH` (or discoverable through `ecbuild_ROOT` /
`CMAKE_PREFIX_PATH` if you call CMake directly).

```sh
mkdir -p build
(cd build && ecbuild --prefix=<install-prefix> -- ..)
cmake --build build -j
(cd build && ctest -j --output-on-failure)
```

Run a single test by target name:

```sh
(cd build && ctest -R '^eckit_test_types_uuid$' --output-on-failure)
```

For debugging, run the test binary directly from the build dir, e.g.
`./bin/eckit_test_types_uuid`.

Test targets are declared in `tests/<module>/CMakeLists.txt` with
`ecbuild_add_test(TARGET eckit_test_<module>_<name> ...)`. Add new tests
there rather than via ad-hoc scripts.

Format C/C++ sources with the project script (clang-format major version 19):

```sh
./format-sources.sh path/to/file.cc path/to/file.h
```

`pre-commit` enforces clang-format 19 on C/C++, plus whitespace/LF, isort,
ruff for Python, yamllint, JSON formatting, and cython-lint.

### Tool target naming

`src/tools/CMakeLists.txt` maps internal CMake targets that use underscores
to installed binaries that use hyphens. For instance, target `eckit_version`
produces the installed tool `eckit-version`. Keep this in mind when looking
up which CMake target builds a particular CLI tool.

### Generated headers

Configured headers like `eckit_config.h` and `eckit_version.h` are written
into the build tree, so include paths must contain both `src/` and
`build/src/`. `ecbuild` wires this up for in-tree targets; external consumers
get it via the installed CMake package config.

---

## Python wrapper

The `python/eckit` wrapper is Cython-based and expects an already-built
eckit:

```sh
ECKIT_SOURCE_DIR=<repo> ECKIT_BUILD_DIR=<build> python3 setup.py build_ext -i
```

`python/eckit/setup.py` links against `eckit` and `eckit_geo`. Override
paths with `ECKIT_LIB_DIR` and `ECKIT_INCLUDE_DIRS` when the default build
layout does not match your setup.

`python/eckit/build_chain.sh` assumes `uv`, `PYVERSION`, and the
wheelmaker / setup_utils environment; its pytest step is currently commented
out and only `twine check` runs.

---

## File naming and ownership

* **One class per file**, named after the class. `class FooBar` lives in
  `FooBar.h` and `FooBar.cc`. Keep small helper structs and free functions
  beside the class they support if they are an implementation detail.
* **Header extension is `.h`**, source extension is `.cc`. Do not use `.hpp`,
  `.cpp`, `.cxx`.
* **Test files** are named `test_<module>_<name>.cc` and produce a CTest
  target `eckit_test_<module>_<name>`.
* **Header-only test helpers** live next to the tests that use them, e.g.
  `tests/system/plugin_test_helpers.h`.

---

## C++ source file template

### Common header (`.h` and `.cc`)

Every source file starts with the ECMWF Apache 2.0 boilerplate, exactly:

```cpp
/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
```

A blank line follows, then for **header files** an author/date block:

```cpp
/// @author Simon Smart
/// @date   May 2026
```

The author/date block is conventional on headers and omitted from `.cc` files.
Authorship identifies individuals who are conceptually responsible for and
aware of the relevant code.
Use the month and year you create the file. Multiple `@author` lines are
allowed when several people are responsible.

### Header file (`.h`)

```cpp
/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Simon Smart
/// @date   May 2026

#pragma once

#include <string>
#include <vector>

#include "eckit/system/Library.h"

namespace eckit::system {

class LibraryManager;  // forward declarations as needed

//----------------------------------------------------------------------------------------------------------------------

/// @brief One-line summary of what this class is.
///
/// Multi-line description if the contract is non-trivial: lifecycle, threading,
/// preconditions, ownership. Useful to spell out invariants that are not obvious
/// from the signatures.
class Foo {
public:

    /// @param [in] name  what the parameter means
    explicit Foo(const std::string& name);

    ~Foo();

    /// @brief Short doxygen on each public method that is not self-evident.
    const std::string& name() const { return name_; }

private:

    friend class LibraryManager;  // friend declarations at the top of private:

    std::string name_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::system
```

Key points:

* **Include guard**: prefer `#pragma once` for new files. Older files
  use Older files use old-style include guards - when significant modifications
  are made to those files switch them to use `#pragma once`, but otherwise
  leave them alone.
* **Includes** are grouped: standard library first (`<...>`), then
  third-party, then project (`"eckit/..."`). A blank line separates groups.
  The corresponding `.h` of a `.cc` file comes first in that `.cc`.
* **Namespaces** start at column 0 with no indentation of their contents.
  Use the C++17 nested form `namespace eckit::system {` rather than nested
  blocks. Close with `}  // namespace eckit::system` (two spaces before the
  comment).
* **Separator lines** of exactly 116 dashes (`//----...`) bracket each major
  section: one after the opening `namespace` declaration and one before the
  closing brace, plus one between top-level declarations when grouping helps
  readability. Match the existing files; clang-format does not generate them.
* **`using namespace` is forbidden in headers.** Use the fully qualified name
  or a narrow `using eckit::Foo;` after the namespace open.
* **Doxygen** uses `///` (triple slash) and the `@brief`/`@param`/`@return`/
  `@throws` tags. Document non-obvious behaviour, especially threading,
  ownership, and exception guarantees.
* **`friend` declarations** go at the top of the `private:` section.
* **Class layout order** is `public:` then `protected:` then `private:`, with
  methods before data members in each.

### Source file (`.cc`)

```cpp
/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/system/Foo.h"

#include <algorithm>
#include <fstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"

namespace eckit::system {

//----------------------------------------------------------------------------------------------------------------------

Foo::Foo(const std::string& name) : name_(name) {}

Foo::~Foo() = default;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::system
```

Key differences from the header:

* **No author/date block** — the convention is to keep that on the header
  only.
* **First include is the matching header**, on its own line, separated by a
  blank line from the rest of the includes. This forces the header to be
  self-contained.
* **No declaration body in the header is duplicated here.** Default and
  trivial inline definitions stay in the header.
* **File-local helpers** (free functions, `static` variables, internal
  classes) go inside an anonymous namespace at the top of the `.cc`:

  ```cpp
  namespace {

  int helper() { return 42; }

  }  // namespace
  ```

### Header-vs-source rule of thumb

| Put in `.h` | Put in `.cc` |
| --- | --- |
| Class declaration, public/protected API, friend declarations | Out-of-line method definitions |
| Inline trivial accessors (one-liners returning a member) | Anything that would otherwise pull a heavy include into the `.h` |
| Templates and constexpr definitions | Free functions and statics that are implementation-only |
| Doxygen describing the contract | Comments explaining *why* a non-obvious implementation choice was made |
| Forward declarations of types only used by reference / pointer | `#include` of those types' headers |

If you find yourself adding a heavyweight `#include` in a header just to make
something compile, consider whether a forward declaration in the header plus
a full include in the `.cc` would do.

---

## Coding conventions

### Language level

C++17. Prefer modern idioms (`auto`, range-for, structured bindings,
`std::optional`, `if constexpr`) where they aid clarity. Do not introduce
C++20 features.

### Formatting

clang-format 19 with the project `.clang-format`. Do **not** hand-format;
run `./format-sources.sh` after editing. Notable settings the formatter
enforces:

* 4-space indent, no tabs.
* Pointer/reference attaches to type: `int* p`, not `int *p`.
* Braces on the same line for functions, classes, and control flow
  (`if (x) {`).
* `class Foo : public Bar {` on one line.
* Member initialiser lists wrap with the colon on a new line and one
  initialiser per line when long.

### Naming

* Types: `UpperCamelCase` (`PluginManifest`, `LibraryManager`).
* Functions and variables: `lowerCamelCase` (`loadedPlugins`, `forLibrary`).
* Private and protected data members: trailing underscore (`name_`,
  `handle_`).
* Macros and preprocessor constants: `SCREAMING_SNAKE_CASE`
  (`REGISTER_LIBRARY`, `eckit_HAVE_ECKIT_MPI`).
* File names match the primary type they declare.

### Errors

Throw `eckit` exceptions from `eckit/exception/Exceptions.h`
(`BadValue`, `SeriousBug`, `AssertionFailed`, ...) rather than standard
ones. Use the `Here()` helper to attach source location where to helps
diagnostics and wherever reasonable. `ASSERT(...)` for invariants,
`EXPECT(...)` only inside test `CASE` bodies.

### Threading

Use `std::mutex` and related `std` types where appropriate. There is
a plethora of existing use of `eckit::Mutex` / `eckit::AutoLock<...>` /
`eckit::StaticMutex` in the code base - it is not necessary to use those
for new code and old code is being gradually migrated.
Document threading expectations in doxygen.

### Includes

* Use `"eckit/<module>/<File>.h"` form (with the `eckit/` prefix) for any
  include from this project, even within the same module.
* Sort within each group; clang-format will do this for you.
* Never include `<bits/...>` or other libstdc++ internals.

### Feature gates

Feature-gated code uses both a CMake `if(eckit_HAVE_<FEATURE>)` to compile
in/out the relevant translation units, and a generated `eckit_HAVE_<FEATURE>`
macro from `eckit_config.h` for runtime conditional logic. New features get
both halves wired up.

---

## Tests

* New tests live in `tests/<module>/test_<module>_<name>.cc` and are
  registered with `ecbuild_add_test(TARGET eckit_test_<module>_<name> ...)`.
* Tests use the harness in `eckit/testing/Test.h`. A typical test file:

  ```cpp
  #include "eckit/testing/Test.h"

  using namespace eckit::testing;

  namespace eckit::test {
  
  //----------------------------------------------------------------------------------------------------------------------

  CASE("descriptive sentence") {
      EXPECT(condition);
      EXPECT_EQUAL(actual, expected);
      EXPECT_THROWS_AS(badCall(), BadValue);
  }
  
  //----------------------------------------------------------------------------------------------------------------------

  }  // namespace eckit::test

  int main(int argc, char** argv) {
      return run_tests(argc, argv);
  }
  ```

* When a test must observe state both before and after a global
  initialisation (e.g. before/after `Main` construction), put plain
  conditional aborts in `main()` around the construction — `EXPECT` only
  works reliably inside `CASE`. 
* Do not silently rely on environment defaults. Make every relevant env var
  explicit with `SetEnv` so the test is hermetic.

---

## Documentation

* User-facing docs live under `docs/content/` as reStructuredText files
  consumed by Sphinx with the Doxygen-generated API reference. Add new
  pages to the appropriate `toctree` in `docs/index.rst` or a
  parent page.
* Doxygen comments in headers are extracted into the API docs; keep them
  current when changing signatures or contracts.
* The Sphinx build runs with `-W` (warnings as errors). A broken cross-
  reference will fail CI.
* Docs CI uses Python 3.13, installs `docs/requirements.txt`, prepares
  Doxygen 1.14.0, and then runs `cd docs && make -j html`. Reproduce
  locally with the same Python version when chasing CI-only failures.

---

## Pre-commit checklist

Before opening a PR:

1. `./format-sources.sh` on every changed C/C++ file.
2. `cmake --build build -j` clean.
3. `ctest -j --output-on-failure` from the build dir, or at minimum the
   tests in the modules you touched.
4. If you added Sphinx content, `cd docs && make -j html` clean.
5. Headers you created carry the licence header *and* `/// @author` /
   `/// @date` lines.
6. No `using namespace` in headers; no stray `#include`s; no commented-out
   code.
