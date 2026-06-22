// eckit Stream bridge — TCP and in-memory `eckit::Stream` serialization.
#pragma once

#include "DataHandleWrapper.h"
#include "eckit/io/Buffer.h"
#include "eckit/net/TCPSocket.h"
#include "eckit/net/TCPStream.h"
#include "eckit/serialisation/MemoryStream.h"
#include "eckit/serialisation/ResizableMemoryStream.h"
#include "eckit/serialisation/Stream.h"

#include "rust/cxx.h"

#include <cstdint>
#include <memory>
#include <string>

namespace eckit_bridge {

//----------------------------------------------------------------------------------------------------------------------

/// Base wrapper for `eckit::Stream`. Subclasses own the transport-specific
/// resources (socket, buffer, etc.) and expose them via `inner()`; all
/// read/write methods in this base dispatch through that virtual accessor.
class StreamWrapper {
public:

    virtual ~StreamWrapper() = default;

    StreamWrapper(const StreamWrapper&)            = delete;
    StreamWrapper& operator=(const StreamWrapper&) = delete;

    // Write operations (eckit::Stream operator<<)
    void write_char(uint8_t c);
    void write_bool(bool v);
    void write_int(int32_t v);
    void write_long(int64_t v);
    void write_unsigned_long(uint64_t v);
    void write_double(double v);
    void write_string(rust::Str v);
    void write_blob(rust::Slice<const uint8_t> data);

    // Read operations (eckit::Stream operator>>)
    uint8_t read_char();
    bool read_bool();
    int32_t read_int();
    int64_t read_long();
    uint64_t read_unsigned_long();
    double read_double();
    rust::String read_string();

    // Raw byte read (for data transfer after protocol handshake)
    virtual int64_t read_bytes(rust::Slice<uint8_t> buf);

    /// Number of bytes written so far.
    int64_t bytes_written() const;

    /// Get buffer contents (memory write streams only).
    virtual rust::Slice<const uint8_t> buffer();

    /// Hand off the underlying connection as a DataHandle for streaming reads.
    ///
    /// Only meaningful for TCP streams; the default throws `NotImplemented`.
    /// After this call the stream is in an unspecified state and must not be
    /// used; the returned `DataHandleWrapper` owns the connection.
    virtual std::unique_ptr<DataHandleWrapper> into_data_handle();

    /// Subclass-supplied access to the owned `eckit::Stream`. Called by every
    /// read/write method in this base, and by downstream bridge code that
    /// needs to plumb the raw stream into other eckit APIs.
    virtual eckit::Stream& inner() = 0;

protected:

    StreamWrapper() = default;
};

//----------------------------------------------------------------------------------------------------------------------

/// TCP stream — connects to host:port via `eckit::net::TCPClient`.
class TcpStreamWrapper : public StreamWrapper {
    eckit::net::TCPSocket socket_;
    eckit::net::TCPStream tcp_;

public:

    TcpStreamWrapper(const std::string& host, int port);
    int64_t read_bytes(rust::Slice<uint8_t> buf) override;
    std::unique_ptr<DataHandleWrapper> into_data_handle() override;

protected:

    eckit::Stream& inner() override { return tcp_; }
};

//----------------------------------------------------------------------------------------------------------------------

/// Resizable memory stream — for writing, buffer grows as needed.
class MemoryWriteStreamWrapper : public StreamWrapper {
    eckit::Buffer buf_;
    eckit::ResizableMemoryStream mem_;

public:

    MemoryWriteStreamWrapper();
    rust::Slice<const uint8_t> buffer() override;

protected:

    eckit::Stream& inner() override { return mem_; }
};

//----------------------------------------------------------------------------------------------------------------------

/// Fixed memory stream — for reading from existing data.
class MemoryReadStreamWrapper : public StreamWrapper {
    eckit::Buffer buf_;
    eckit::MemoryStream mem_;

public:

    MemoryReadStreamWrapper(rust::Slice<const uint8_t> data);

protected:

    eckit::Stream& inner() override { return mem_; }
};

//----------------------------------------------------------------------------------------------------------------------

// Factory functions
std::unique_ptr<StreamWrapper> stream_connect(rust::Str host, int32_t port);
std::unique_ptr<StreamWrapper> stream_memory_write();
std::unique_ptr<StreamWrapper> stream_memory_read(rust::Slice<const uint8_t> data);

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit_bridge
