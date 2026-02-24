#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

REQUIRED_VERSION=19

usage() {
    cat <<'EOF'
Usage: format-sources.sh [OPTIONS] [FILE ...]

Apply clang-format to eckit source files.

The formatter binary is resolved in this order:

  1. $CLANG_FORMAT environment variable (must be an executable path)
  2. clang-format-19 on $PATH
  3. clang-format   on $PATH

Unless --any-version is given, the binary must report major version 19.

When no files are given, recursively formats all *.h and *.cc files
relative to the script's location (the repository root), excluding any
contrib/ directories.  Formatting runs in parallel using all available
cores.  The script can be invoked from any working directory.

Options:
  -h, --help        Show this help message and exit
  --any-version     Skip the version-19 check (prints a warning)

Environment:
  CLANG_FORMAT      Explicit path to the clang-format binary to use

Examples:
  ./format-sources.sh                                        # format all
  ./format-sources.sh src/eckit/foo.cc                       # one file
  CLANG_FORMAT=/usr/bin/clang-format-19 ./format-sources.sh  # explicit binary
  ./format-sources.sh --any-version src/eckit/bar.h          # any version
EOF
}

# Check that the resolved binary is clang-format $REQUIRED_VERSION.
# Usage: check_version <binary>
check_version() {
    local binary="$1"
    local version_output major

    version_output=$("$binary" --version 2>&1) || {
        echo "Error: failed to run '$binary --version'" >&2
        exit 1
    }

    # Typical output: "clang-format version 19.1.7 (...)"
    major=$(echo "$version_output" | sed -n 's/.*version \([0-9]\+\).*/\1/p')

    if [[ -z "$major" ]]; then
        echo "Error: unable to parse version from '$binary --version' output:" >&2
        echo "  $version_output" >&2
        exit 1
    fi

    if [[ "$major" -ne "$REQUIRED_VERSION" ]]; then
        echo "Error: clang-format version $major found, version $REQUIRED_VERSION required" >&2
        echo "  binary: $binary" >&2
        echo "  Set CLANG_FORMAT to the correct binary or use --any-version to skip this check." >&2
        exit 1
    fi
}

any_version=false

while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--help)
            usage
            exit 0
            ;;
        --any-version)
            any_version=true
            shift
            ;;
        --)
            shift
            break
            ;;
        -*)
            echo "Error: unknown option '$1'" >&2
            usage >&2
            exit 1
            ;;
        *)
            break
            ;;
    esac
done

# Resolve clang-format binary
if [[ -n "${CLANG_FORMAT:-}" ]]; then
    if [[ ! -x "$CLANG_FORMAT" ]]; then
        echo "Error: CLANG_FORMAT is set to '$CLANG_FORMAT' but it is not executable" >&2
        exit 1
    fi
elif command -v clang-format-19 &>/dev/null; then
    CLANG_FORMAT="$(command -v clang-format-19)"
elif command -v clang-format &>/dev/null; then
    CLANG_FORMAT="$(command -v clang-format)"
else
    echo "Error: no clang-format found" >&2
    echo "  Install clang-format-19, add it to \$PATH, or set the CLANG_FORMAT variable." >&2
    exit 1
fi

if [[ "$any_version" == true ]]; then
    echo "Warning: --any-version used; skipping version check for $CLANG_FORMAT" >&2
else
    check_version "$CLANG_FORMAT"
fi

# Collect files: positional args, or default to all *.h / *.cc in the repo
if [[ $# -gt 0 ]]; then
    files=("$@")
else
    mapfile -t files < <(find "$SCRIPT_DIR" \( -name '*.h' -o -name '*.cc' \) \
        -not -path '*/contrib/*' -type f)
    if [[ ${#files[@]} -eq 0 ]]; then
        echo "No source files found" >&2
        exit 1
    fi
fi

printf '%s\0' "${files[@]}" \
    | xargs -0 -P0 -n1 "$CLANG_FORMAT" -i -style=file -fallback-style=none
