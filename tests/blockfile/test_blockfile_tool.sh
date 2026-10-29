#!/usr/bin/env bash
# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

# Exercise the eckit-blockfile tool end to end.
# Usage: test_blockfile_tool.sh <path to eckit-blockfile> <directory holding records.jsonl>

set -euo pipefail
export LC_ALL=C

tool="$1"
data="$2/records.jsonl"
work="blockfile_tool_test"

rm -rf "$work"
mkdir -p "$work"
cd "$work"

fail() {
    echo "FAILED: $*" >&2
    exit 1
}

count() {
    grep -c "$1" || true
}

# convert: 6 records, typed keys, at most 2 records per block
"$tool" convert "$data" --output all.ekbf --records-per-block 2 \
    --keys class=mars.class,stream=mars.stream,param:int=mars.param

# verify / info / stats
"$tool" verify all.ekbf | grep -q "OK (.* 6 records)" || fail "verify all.ekbf"
"$tool" info all.ekbf > info.txt
grep -q "^records: *6$" info.txt || fail "info record count"
grep -q "param(int)" info.txt || fail "info key types"
"$tool" stats all.ekbf > stats.txt
grep -q "enfo .* 3 records" stats.txt || fail "stats enfo count"
grep -q "class=od,stream=enfo,param=130" stats.txt || fail "stats combinations"

# list: CSV header, one line per block, records add up, offsets are consistent
"$tool" list all.ekbf > list.csv
head -1 list.csv | grep -qx "block,size,startOffset,endOffset,records,class,stream,param" || fail "list header"
[ "$(tail -n +2 list.csv | awk -F, '{ n += $5 } END { print n }')" -eq 6 ] || fail "list record total"
tail -n +2 list.csv | awk -F, '$4 != $2 + $3 { exit 1 }' || fail "list endOffset"
[ "$("$tool" list all.ekbf --no-header --filter stream=enfo | awk -F, '{ n += $5 } END { print n }')" -eq 3 ] \
    || fail "list with filter"
"$tool" list all.ekbf --no-header --separator ';' | head -1 | grep -q ';od;' || fail "list separator"

# cat returns every record unchanged
"$tool" cat all.ekbf | sort > cat.txt
sort "$data" | diff - cat.txt || fail "cat differs from the input"

# filters: by string value and by numeric range
[ "$("$tool" cat all.ekbf --filter stream=enfo | count enfo)" -eq 3 ] || fail "filter stream=enfo"
[ "$("$tool" cat all.ekbf --filter param=100/to/150 | count '"param":130')" -eq 3 ] || fail "filter param range"
[ "$("$tool" cat all.ekbf --filter class=rd | wc -l)" -eq 1 ] || fail "filter class=rd"

# select copies blocks; convert merges block files using their labels
"$tool" select all.ekbf --filter stream=enfo --output enfo.ekbf
"$tool" verify enfo.ekbf | grep -q "OK (.* 3 records)" || fail "verify enfo.ekbf"
"$tool" convert all.ekbf enfo.ekbf --output merged.ekbf
"$tool" verify merged.ekbf | grep -q "OK (.* 9 records)" || fail "verify merged.ekbf"

# convert driven by a YAML configuration (object form and short form); --keys overrides it
cat > keys.yaml <<'YAML'
keys:
  - {name: class, path: mars.class}
  - {name: param, path: mars.param, type: int}
records-per-block: 3
YAML
"$tool" convert "$data" --output config.ekbf --config keys.yaml
"$tool" info config.ekbf | grep -q "class(string) param(int)$" || fail "config keys"
"$tool" verify config.ekbf | grep -q "OK (.* 6 records)" || fail "verify config.ekbf"
[ "$("$tool" cat config.ekbf --filter param=167 | wc -l)" -eq 2 ] || fail "filter on config keys"

printf 'keys: [stream=mars.stream]\n' > short.yaml
"$tool" convert "$data" --output short.ekbf --config short.yaml
"$tool" info short.ekbf | grep -q "stream(string)$" || fail "short-form config keys"
"$tool" convert "$data" --output override.ekbf --config short.yaml --keys class=mars.class
"$tool" info override.ekbf | grep -q "class(string)$" || fail "--keys overrides --config"

# a block file is a valid zstd stream
if command -v zstdcat > /dev/null; then
    [ "$(zstdcat all.ekbf | wc -l)" -eq 6 ] || fail "zstdcat all.ekbf"
fi

# errors are reported with a non-zero exit status
size=$(wc -c < all.ekbf)
dd if=all.ekbf of=truncated.ekbf bs=1 count=$((size - 10)) 2> /dev/null
if "$tool" verify truncated.ekbf > /dev/null 2>&1; then fail "truncated file accepted"; fi
if "$tool" cat all.ekbf --filter levtype=pl > /dev/null 2>&1; then fail "unknown filter key accepted"; fi
if "$tool" convert "$data" --output x.ekbf > /dev/null 2>&1; then fail "convert without --keys accepted"; fi

echo "eckit-blockfile tool test passed"
