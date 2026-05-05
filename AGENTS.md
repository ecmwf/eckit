# AGENTS.md

Guidance for AI coding agents working in this repository. **Human contributors
should read [`CONTRIBUTING.md`](CONTRIBUTING.md) first** — that document is
the canonical reference for project structure, style, build/test workflow,
and the C++ source-file template. This file only adds the agent-specific
notes that sit on top of it.

---

## Source of truth

When the prose in any document disagrees with the build files, **the
executable CMake/workflow files win**. In order of authority:

1. Root `CMakeLists.txt` and per-module `CMakeLists.txt` files — they define
   what is built, gated, installed, and tested.
2. `tests/<module>/CMakeLists.txt` — authoritative list of tests and their
   environment.
3. `.pre-commit-config.yaml` — authoritative list of lint/format checks.
4. `docs/Makefile`, `.github/workflows/*` — authoritative CI behaviour.
5. `CONTRIBUTING.md` — the human-readable summary; aim to keep it
   consistent with the above when you touch related areas.
6. `AGENTS.md` (this file) and `.github/copilot-instructions.md` — agent
   guidance; subordinate to everything above.

If you spot a real divergence, fix the prose, not the build files (unless
the user has asked for a behaviour change).

---

## Required reading before non-trivial changes

* [`CONTRIBUTING.md`](CONTRIBUTING.md) — full project overview, file
  templates, naming, formatting, testing harness, pre-commit checklist.
* The `CMakeLists.txt` of any module you are about to modify, plus its
  test directory.

---

## House-keeping invariants agents tend to miss

These are the recurring traps. Re-check them before declaring a task done.

* **Tool target ↔ installed binary.** `src/tools/CMakeLists.txt` maps
  underscored CMake targets to hyphenated installed names: target
  `eckit_version` → installed binary `eckit-version`. Search for both
  spellings when looking up usage.
* **Feature gates have two halves.** A new optional feature needs *both*
  an `ecbuild_add_option(FEATURE ...)` block in the root `CMakeLists.txt`
  *and* an `if(eckit_HAVE_<FEATURE>)` guard in each consuming
  `CMakeLists.txt` *and* an `eckit_HAVE_<FEATURE>` C++ macro in the
  generated `eckit_config.h`. Touching one without the others produces
  silently broken builds.
* **Generated headers live in the build tree.** `eckit_config.h`,
  `eckit_version.h`, and similar are written under `build/src/...` not the
  source tree. Compile commands and any tooling you spawn must include
  both `src/` and `build/src/` on the include path.
* Avoid unnecessary `eckit::` qualifiers in the code. When inside a
  nested namespace (such as `namespace eckit::test`) redundant
  qualifiers are just noise.
* **`eckit::Mutex` is recursive.** Nested locking is safe, but do not
  assume that of any other mutex type. Recursive calls should be avoided
  wherever possible in any case.

---

## Workflow rules for agents

* **Add tests via `ecbuild_add_test`, never via ad-hoc scripts.** The
  CTest integration is the only contract CI honours.
* **Never edit pre-existing style on lines you are not otherwise
  modifying.** Cleanups creep, reviews balloon. Limit refactors to the
  scope explicitly requested by the user, or open a separate task for
  them.
* **Format with `clang-format` directly, not by hand.** Run
  `./format-sources.sh` (which enforces version 19) on every C/C++ file
  you touched. Do not run it on `CMakeLists.txt`.
* **Verify with the real build, not with reasoning.** After non-trivial
  edits, run `cmake --build build -j<N>` and at minimum the tests in the
  modules you touched. The user's machine has limited cores; if they
  have asked you to use a specific `-j`, respect it. If not specified
  do not exceed -j6.
* **Do not commit unless the user explicitly asks.** The default
  expectation is "show me the diff, I will commit".
* **Do not push to remote unless the user explicitly asks.** Same as
  above.
* **Untracked artefacts** (e.g. `Testing/`, ad-hoc local scratch
  directories) should be left alone unless the user asks to clean them
  up.

---

## Hand-off

When pausing or finishing a piece of work, leave the workspace ready for
either the user or another agent to resume:

* Build is clean (no half-applied edits that fail to compile).
* Tests in the touched module pass; mention any pre-existing failures you
  did not address.
* Summarise what changed, why, and what is left, in a few bullets — not
  a wall of text. The user will read it; brevity matters.
