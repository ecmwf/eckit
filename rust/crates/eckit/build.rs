fn main() {
    bindman_utils::reexport_dep_root("ECKIT_SYS");

    // This crate's own test binaries are leaf binaries: the vendored libraries
    // carry `@rpath` install names (macOS) / `$ORIGIN` rpath (Linux), so they
    // need rpath entries of their own to load eckit at test time. Does not
    // reach downstream consumers — they call `emit_rpaths()` themselves.
    bindman_utils::emit_rpaths();
}
