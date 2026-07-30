#!/bin/bash

set -euo pipefail

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
ECKIT_SRC_DIR=$(cd -- "$SCRIPT_DIR/../eckit/src" && pwd)

compute_relative_up() {
    local rel_dir="$1"
    if [ -z "$rel_dir" ] || [ "$rel_dir" = "." ]; then
        echo ""
        return
    fi
    local count
    count=$(echo "$rel_dir" | awk -F'/' '{print NF}')
    local up=""
    for ((i=0; i<count; i++)); do
        up="../$up"
    done
    echo "$up"
}

extract_lib_stem() {
    local name
    name=$(basename "$1")
    local stem="${name%%.so*}"
    stem="${stem%%.dylib*}"
    stem="${stem%%.*}"
    stem="${stem%%-*}"
    echo "$stem"
}

find_matching_vendor_lib() {
    local vendor_dir="$1"
    local needed_lib="$2"

    local stem
    stem=$(extract_lib_stem "$needed_lib")

    if [ -z "$stem" ] || [ ! -d "$vendor_dir" ]; then
        return
    fi

    local match
    match=$(find "$vendor_dir" -maxdepth 1 \( -name "${stem}-*" -o -name "${stem}.*" -o -name "${stem}" \) | head -n 1)
    if [ -n "$match" ]; then
        basename "$match"
    fi
}

process_binary_linux() {
    local bin_file="$1"
    local eckit_src_dir="$2"
    local vendor_dir="$3"

    local bin_dir
    bin_dir=$(dirname "$bin_file")
    local rel_dir="${bin_dir#"$eckit_src_dir"}"
    rel_dir="${rel_dir#/}"

    local rel_up
    rel_up=$(compute_relative_up "$rel_dir")
    local vendor_dir_name
    vendor_dir_name=$(basename "$vendor_dir")

    # shellcheck disable=SC2016
    local rpath='$ORIGIN/'"${rel_up}${vendor_dir_name}:"' $ORIGIN/'"${rel_up}eckitlib"
    # Clean up any whitespace introduced above
    rpath=$(echo "$rpath" | tr -d ' ')

    echo "running patchelf --print-needed $bin_file"
    local needed_libs
    needed_libs=$(patchelf --print-needed "$bin_file")

    while read -r needed_lib; do
        [ -z "$needed_lib" ] && continue
        local matched_lib
        matched_lib=$(find_matching_vendor_lib "$vendor_dir" "$needed_lib")
        if [ -n "$matched_lib" ]; then
            echo "running patchelf --replace-needed $needed_lib $matched_lib $bin_file"
            patchelf --replace-needed "$needed_lib" "$matched_lib" "$bin_file"
        fi
    done <<< "$needed_libs"

    echo "running patchelf --add-rpath $rpath $bin_file"
    patchelf --add-rpath "$rpath" "$bin_file"
}

process_binary_macos() {
    local bin_file="$1"
    local eckit_src_dir="$2"
    local vendor_dir="$3"

    local bin_dir
    bin_dir=$(dirname "$bin_file")
    local rel_dir="${bin_dir#"$eckit_src_dir"}"
    rel_dir="${rel_dir#/}"

    local rel_up
    rel_up=$(compute_relative_up "$rel_dir")
    local vendor_dir_name
    vendor_dir_name=$(basename "$vendor_dir")

    local rpath_vendor="@loader_path/${rel_up}${vendor_dir_name}"
    local rpath_eckit="@loader_path/${rel_up}eckitlib"

    echo "running otool -L $bin_file"
    local otool_out
    otool_out=$(otool -L "$bin_file")

    echo "$otool_out" | tail -n +2 | awk '{print $1}' | while read -r dep_path; do
        [ -z "$dep_path" ] && continue
        # Ignore system libraries and existing rpath/loader_path entries
        case "$dep_path" in
            /usr/lib/*|/System/Library/*|@rpath/*|@loader_path/*)
                continue
                ;;
        esac

        local matched_lib
        matched_lib=$(find_matching_vendor_lib "$vendor_dir" "$dep_path")
        if [ -n "$matched_lib" ]; then
            echo "running install_name_tool -change $dep_path @rpath/$matched_lib $bin_file"
            install_name_tool -change "$dep_path" "@rpath/$matched_lib" "$bin_file"
        fi
    done

    echo "running install_name_tool -add_rpath $rpath_vendor $bin_file"
    install_name_tool -add_rpath "$rpath_vendor" "$bin_file" 2>/dev/null || true

    echo "running install_name_tool -add_rpath $rpath_eckit $bin_file"
    install_name_tool -add_rpath "$rpath_eckit" "$bin_file" 2>/dev/null || true
}

fix_another_wheel_linux() {
    local vendor_dir="$1"

    echo "will crawl $ECKIT_SRC_DIR"
    if [ -d "$ECKIT_SRC_DIR" ]; then
        while read -r bin_file; do
            [ -z "$bin_file" ] && continue
            echo "found $bin_file, will process it"
            process_binary_linux "$bin_file" "$ECKIT_SRC_DIR" "$vendor_dir"
        done < <(find "$ECKIT_SRC_DIR" -type f -name "*.so")
    fi
}

fix_another_wheel_macos() {
    local wheel_file="$1"

    echo "inspecting delocated wheel $wheel_file"
    local inspect_dir="/tmp/eckit/delocate_inspect"
    rm -rf "$inspect_dir"
    mkdir -p "$inspect_dir"
    unzip -q "$wheel_file" -d "$inspect_dir"

    local vendor_dir
    vendor_dir=$(find "$inspect_dir" -maxdepth 2 -type d \( -name "*.dylibs" -o -name ".dylibs" -o -name "*.libs" \) | head -n 1)

    if [ -n "$vendor_dir" ] && [ -d "$vendor_dir" ]; then
        echo "found vendor dylibs at $vendor_dir"
    else
        echo "warning: vendor dylibs directory not found in $wheel_file"
        vendor_dir="$inspect_dir"
    fi

    echo "will crawl $ECKIT_SRC_DIR"
    if [ -d "$ECKIT_SRC_DIR" ]; then
        while read -r bin_file; do
            [ -z "$bin_file" ] && continue
            echo "found $bin_file, will process it"
            process_binary_macos "$bin_file" "$ECKIT_SRC_DIR" "$vendor_dir"
        done < <(find "$ECKIT_SRC_DIR" -type f \( -name "*.so" -o -name "*.dylib" \))
    fi

    rm -rf "$inspect_dir"
}

# NOTE in case of problems like we had with eccodes, replace with noop here
if [ "$(uname)" != "Darwin" ] ; then
    rm -rf /tmp/eckit/auditwheel
    auditwheel repair -w /tmp/eckit/auditwheel /tmp/eckit/build/wheel/*whl
    cd /tmp/eckit/auditwheel
    F=$(basename "$(ls ./*.whl)")
    unzip "$F"
    # shellcheck disable=SC2016
    patchelf --add-rpath '$ORIGIN' eckitlib.libs/*

    fix_another_wheel_linux "/tmp/eckit/auditwheel/eckitlib.libs"

    rm "$F"
    zip -r "$F" ./*
    rm /tmp/eckit/build/wheel/*
    mv "/tmp/eckit/auditwheel/$F" /tmp/eckit/build/wheel
    cd -
fi

# NOTE on macos we delocate with impunity, because the findlibs recursive depload
# is disabled anyway
if [ "$(uname)" = "Darwin" ] ; then
    delocate-wheel /tmp/eckit/build/wheel/*whl
    WHEEL_FILE=$(find /tmp/eckit/build/wheel -name "*.whl" | head -n 1)
    if [ -n "$WHEEL_FILE" ]; then
        fix_another_wheel_macos "$WHEEL_FILE"
    fi
fi
