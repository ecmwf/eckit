// eckit DataHandle bridge — wraps `eckit::DataHandle` and its factories.
#pragma once

#include "eckit/io/DataHandle.h"

#include "rust/cxx.h"

#include <cstdint>
#include <memory>

namespace eckit_bridge {

/// Wraps `eckit::DataHandle*` for Rust FFI. Takes ownership.
class DataHandleWrapper {
    std::unique_ptr<eckit::DataHandle> handle_;

public:

    /// Takes ownership of a raw DataHandle pointer.
    explicit DataHandleWrapper(eckit::DataHandle* h) : handle_(h) {}

    int64_t open_for_read();
    void open_for_write(int64_t estimated_length);
    int64_t read(rust::Slice<uint8_t> buf);
    int64_t write(rust::Slice<const uint8_t> buf);
    void close();
    int64_t position() const;
    int64_t seek(int64_t offset);
    bool can_seek() const;
    int64_t estimate() const;

    /// Copy all data from this handle into target (both must be open).
    int64_t save_into(DataHandleWrapper& target);

    /// Access underlying for other C++ bridge code.
    eckit::DataHandle& inner() { return *handle_; }
    const eckit::DataHandle& inner() const { return *handle_; }

    /// Release ownership — caller takes responsibility.
    eckit::DataHandle* release() { return handle_.release(); }
};

/// Open a file as a DataHandle for reading.
std::unique_ptr<DataHandleWrapper> data_handle_from_file(rust::Str path);

/// Open a byte range of a file as a DataHandle.
std::unique_ptr<DataHandleWrapper> data_handle_from_part(rust::Str path, int64_t offset, int64_t length);

/// Create a DataHandle from an in-memory buffer (copies the data).
std::unique_ptr<DataHandleWrapper> data_handle_from_buffer(rust::Slice<const uint8_t> data);

/// Create a MultiHandle from multiple file paths.
std::unique_ptr<DataHandleWrapper> data_handle_from_multi(rust::Slice<const rust::String> paths);

/// Create a TeeHandle from multiple file paths — writes all targets in
/// parallel.
std::unique_ptr<DataHandleWrapper> data_handle_tee(rust::Slice<const rust::String> paths);

// Forward declaration — defined on the Rust side, cxx generates the type.
struct ReaderBox;

/// Create a DataHandle that forwards `read()` calls to a Rust `std::io::Read`
/// source wrapped in a `ReaderBox`. Used to stream bytes from Rust into any
/// C++ API that consumes an `eckit::DataHandle&` (e.g. fdb5 archive, the
/// streaming retrieve API) without staging through a temp file or buffer.
///
/// The returned handle is owned by the Rust side; on drop, the contained
/// `ReaderBox` is dropped, releasing the underlying `Read` source.
std::unique_ptr<DataHandleWrapper> data_handle_from_reader(rust::Box<ReaderBox> reader);

}  // namespace eckit_bridge
