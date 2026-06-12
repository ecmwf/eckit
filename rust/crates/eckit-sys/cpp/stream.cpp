// eckit Stream bridge — implementation.

#include "eckit_exceptions.h"

#include "eckit-sys/src/lib.rs.h"
#include "stream.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/io/TCPSocketHandle.h"
#include "eckit/net/TCPClient.h"

namespace eckit_bridge {

// ==================== Stream (base class) ====================

void StreamWrapper::write_char(uint8_t c) {
    stream() << static_cast<char>(c);
}
void StreamWrapper::write_bool(bool v) {
    stream() << v;
}
void StreamWrapper::write_int(int32_t v) {
    stream() << v;
}
void StreamWrapper::write_long(int64_t v) {
    stream() << static_cast<long long>(v);
}
void StreamWrapper::write_unsigned_long(uint64_t v) {
    stream() << static_cast<unsigned long long>(v);
}
void StreamWrapper::write_double(double v) {
    stream() << v;
}
void StreamWrapper::write_string(rust::Str v) {
    stream() << std::string(v.data(), v.size());
}
void StreamWrapper::write_blob(rust::Slice<const uint8_t> data) {
    stream().writeBlob(data.data(), data.size());
}

uint8_t StreamWrapper::read_char() {
    char c{};
    stream() >> c;
    return static_cast<uint8_t>(c);
}
bool StreamWrapper::read_bool() {
    bool v{};
    stream() >> v;
    return v;
}
int32_t StreamWrapper::read_int() {
    int v{};
    stream() >> v;
    return v;
}
int64_t StreamWrapper::read_long() {
    long long v{};
    stream() >> v;
    return static_cast<int64_t>(v);
}
uint64_t StreamWrapper::read_unsigned_long() {
    unsigned long long v{};
    stream() >> v;
    return static_cast<uint64_t>(v);
}
double StreamWrapper::read_double() {
    double v{};
    stream() >> v;
    return v;
}
rust::String StreamWrapper::read_string() {
    std::string v;
    stream() >> v;
    return rust::String(v);
}

int64_t StreamWrapper::read_bytes(rust::Slice<uint8_t> buf) {
    throw eckit::SeriousBug("read_bytes not supported on this stream type");
}

rust::Slice<const uint8_t> StreamWrapper::buffer() {
    throw eckit::SeriousBug("buffer() not supported on this stream type");
}

int64_t StreamWrapper::bytes_written() const {
    return const_cast<StreamWrapper*>(this)->stream().bytesWritten();
}

std::unique_ptr<DataHandleWrapper> StreamWrapper::into_data_handle() {
    throw eckit::NotImplemented("StreamWrapper::into_data_handle is only supported on TCP streams", Here());
}

// ==================== TcpStreamWrapper ====================

TcpStreamWrapper::TcpStreamWrapper(const std::string& host, int port) :
    socket_(eckit::net::TCPClient().connect(host, port)), tcp_(socket_) {}

int64_t TcpStreamWrapper::read_bytes(rust::Slice<uint8_t> buf) {
    // The connection lives on `tcp_.socket()` — `socket_` was emptied by the
    // TCPSocket "copy" ctor in `tcp_(socket_)` (ownership transfer).
    return tcp_.socket().read(buf.data(), static_cast<long>(buf.size()));
}

std::unique_ptr<DataHandleWrapper> TcpStreamWrapper::into_data_handle() {
    // Steal the live connection from `tcp_` and wrap it as an owning
    // `eckit::TCPSocketHandle` DataHandle. The TCPSocketHandle ctor copies
    // (ownership transfer) the socket into its own member, so after this call
    // both `socket_` and `tcp_.socket()` are detached (fd = -1) and only the
    // returned DataHandle holds the connection.
    return std::make_unique<DataHandleWrapper>(new eckit::TCPSocketHandle(tcp_.socket()));
}

// ==================== MemoryWriteStreamWrapper ====================

MemoryWriteStreamWrapper::MemoryWriteStreamWrapper() : buf_(4096), mem_(buf_) {}

rust::Slice<const uint8_t> MemoryWriteStreamWrapper::buffer() {
    return {static_cast<const uint8_t*>(buf_.data()), static_cast<size_t>(mem_.bytesWritten())};
}

// ==================== MemoryReadStreamWrapper ====================

MemoryReadStreamWrapper::MemoryReadStreamWrapper(rust::Slice<const uint8_t> data) :
    buf_(data.data(), data.size()), mem_(buf_) {}

// ==================== Factory functions ====================

std::unique_ptr<StreamWrapper> stream_connect(rust::Str host, int32_t port) {
    return std::make_unique<TcpStreamWrapper>(std::string(host), port);
}

std::unique_ptr<StreamWrapper> stream_memory_write() {
    return std::make_unique<MemoryWriteStreamWrapper>();
}

std::unique_ptr<StreamWrapper> stream_memory_read(rust::Slice<const uint8_t> data) {
    return std::make_unique<MemoryReadStreamWrapper>(data);
}

}  // namespace eckit_bridge
