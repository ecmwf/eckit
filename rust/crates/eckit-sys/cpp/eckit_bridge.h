// eckit C++ bridge for Rust FFI
#pragma once

// Include auto-generated trycatch handler FIRST — before cxx generates its
// default
#include "eckit_exceptions.h"

#include "eckit/config/LocalConfiguration.h"
#include "eckit/config/YAMLConfiguration.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/io/MultiHandle.h"
#include "eckit/io/PartFileHandle.h"
#include "eckit/io/TeeHandle.h"
#include "eckit/log/Log.h"
#include "eckit/log/LogTarget.h"
#include "eckit/message/Message.h"
#include "eckit/message/Reader.h"
#include "eckit/io/TCPSocketHandle.h"
#include "eckit/net/TCPClient.h"
#include "eckit/net/TCPStream.h"
#include "eckit/runtime/Main.h"
#include "eckit/serialisation/MemoryStream.h"
#include "eckit/serialisation/ResizableMemoryStream.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/system/Library.h"
#include "eckit/system/LibraryManager.h"

#include "rust/cxx.h"

#include <memory>
#include <string>
#include <vector>

namespace eckit_bridge {

// Forward declarations — full definitions in cxx-generated code
enum class LogLevel : std::uint8_t;
void rust_log(LogLevel level, rust::Str msg) noexcept;

// ==================== Logging ====================

/// LogTarget that routes all output to Rust's log crate.
/// Accumulates writes until a newline or flush, then emits a single log call.
class RustLogTarget : public eckit::LogTarget {
public:
  explicit RustLogTarget(LogLevel level) : level_(level) {}

  void write(const char *start, const char *end) override;
  void flush() override;

private:
  LogLevel level_;
  std::string buffer_;
};

/// Main subclass that installs `RustLogTarget` on all channels.
/// Every new thread automatically gets `RustLogTarget` via the factory methods.
class RustMain : public eckit::Main {
public:
  RustMain(int argc, char **argv);

  eckit::LogTarget *createInfoLogTarget() const override;
  eckit::LogTarget *createWarningLogTarget() const override;
  eckit::LogTarget *createErrorLogTarget() const override;
  eckit::LogTarget *createDebugLogTarget() const override;
  eckit::LogTarget *createMetricsLogTarget() const override;
};

/// Initialize eckit runtime with Rust log bridge.
void init();

// ==================== Configuration ====================

/// Wraps `eckit::LocalConfiguration` for Rust FFI.
class ConfigWrapper {
  eckit::LocalConfiguration config_;

public:
  ConfigWrapper() = default;
  explicit ConfigWrapper(const eckit::Configuration &other) : config_(other) {}

  // Read
  bool has(rust::Str key) const;
  bool is_list(rust::Str key) const;
  bool is_empty() const;
  rust::String get_string(rust::Str key, rust::Str default_val) const;
  int64_t get_long(rust::Str key, int64_t default_val) const;
  int32_t get_int(rust::Str key, int32_t default_val) const;
  bool get_bool(rust::Str key, bool default_val) const;
  double get_double(rust::Str key, double default_val) const;
  rust::Vec<rust::String>
  get_string_vector(rust::Str key,
                    const rust::Vec<rust::String> &default_val) const;

  // Sub-configurations (by key)
  std::unique_ptr<ConfigWrapper> get_sub(rust::Str key) const;
  size_t sub_count(rust::Str key) const;
  std::unique_ptr<ConfigWrapper> sub_at(rust::Str key, size_t index) const;

  // Sub-configurations (root-level list)
  size_t root_sub_count() const;
  std::unique_ptr<ConfigWrapper> root_sub_at(size_t index) const;

  // Write
  void set_string(rust::Str key, rust::Str value);
  void set_long(rust::Str key, int64_t value);
  void set_int(rust::Str key, int32_t value);
  void set_bool(rust::Str key, bool value);
  void set_double(rust::Str key, double value);
  void remove(rust::Str key);

  // Access underlying for other C++ bridge code
  const eckit::LocalConfiguration &inner() const { return config_; }
  eckit::LocalConfiguration &inner() { return config_; }
};

// Factory functions — names match cxx bridge declarations
std::unique_ptr<ConfigWrapper> create();
std::unique_ptr<ConfigWrapper> from_path(rust::Str path);
std::unique_ptr<ConfigWrapper> from_yaml(rust::Str yaml);
std::unique_ptr<ConfigWrapper> clone(const ConfigWrapper &src);

// ==================== DataHandle ====================

/// Wraps `eckit::DataHandle*` for Rust FFI. Takes ownership.
class DataHandleWrapper {
  std::unique_ptr<eckit::DataHandle> handle_;

public:
  /// Takes ownership of a raw DataHandle pointer.
  explicit DataHandleWrapper(eckit::DataHandle *h) : handle_(h) {}

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
  int64_t save_into(DataHandleWrapper &target);

  /// Access underlying for other C++ bridge code.
  eckit::DataHandle &inner() { return *handle_; }
  const eckit::DataHandle &inner() const { return *handle_; }

  /// Release ownership — caller takes responsibility.
  eckit::DataHandle *release() { return handle_.release(); }
};

// ==================== Message + Reader ====================

/// Wraps `eckit::message::Message` for Rust FFI.
/// Message is a value type with internal reference counting.
class MessageWrapper {
  eckit::message::Message msg_;

public:
  MessageWrapper() = default;
  explicit MessageWrapper(eckit::message::Message m) : msg_(std::move(m)) {}

  bool is_valid() const;
  size_t length() const;
  /// Byte offset of this message within the source data handle. Mirrors
  /// `eckit::message::Message::offset()` — populated by the Reader when
  /// scanning a file/stream.
  int64_t offset() const;
  rust::String get_string(rust::Str key) const;
  int64_t get_long(rust::Str key) const;
  double get_double(rust::Str key) const;
  rust::Slice<const uint8_t> data() const;
  void write_to(DataHandleWrapper &handle) const;

  /// Clone (Message is ref-counted internally).
  std::unique_ptr<MessageWrapper> clone() const;

  /// Access underlying for other C++ bridge code.
  const eckit::message::Message &inner() const { return msg_; }
  eckit::message::Message &inner() { return msg_; }
};

/// Wraps `eckit::message::Reader` for Rust FFI.
/// Reads messages from a `DataHandle`.
class ReaderWrapper {
  std::unique_ptr<eckit::message::Reader> reader_;

public:
  explicit ReaderWrapper(DataHandleWrapper &handle);

  /// Returns next message, or an invalid message when exhausted.
  std::unique_ptr<MessageWrapper> next();
};

// Factory
std::unique_ptr<ReaderWrapper> new_reader(DataHandleWrapper &handle);

/// Open a file as a DataHandle for reading.
std::unique_ptr<DataHandleWrapper> data_handle_from_file(rust::Str path);

/// Open a byte range of a file as a DataHandle.
std::unique_ptr<DataHandleWrapper>
data_handle_from_part(rust::Str path, int64_t offset, int64_t length);

/// Create a DataHandle from an in-memory buffer (copies the data).
std::unique_ptr<DataHandleWrapper>
data_handle_from_buffer(rust::Slice<const uint8_t> data);

/// Create a MultiHandle from multiple file paths.
std::unique_ptr<DataHandleWrapper>
data_handle_from_multi(rust::Slice<const rust::String> paths);

/// Create a TeeHandle from multiple file paths — writes all targets in
/// parallel.
std::unique_ptr<DataHandleWrapper>
data_handle_tee(rust::Slice<const rust::String> paths);

// ==================== Library registration ====================

// Forward declaration — defined on the Rust side, cxx generates the type.
struct LibraryBox;

/// eckit::system::Library subclass that delegates to a Rust `dyn Library`.
class RustLibrary : public eckit::system::Library {
  rust::Box<LibraryBox> rust_;

public:
  explicit RustLibrary(rust::Box<LibraryBox> lib);

  std::string version() const override;
  std::string gitsha1(unsigned int count) const override;

protected:
  std::string home() const override;
  std::string libraryHome() const override;
  std::string prefixDirectory() const override;
  std::string expandPath(const std::string &path) const override;
  bool debug() const override;
  const void *addr() const override;
};

/// Register a Rust library with eckit's LibraryManager.
void register_library(rust::Box<LibraryBox> lib);

/// Get configuration for a registered library by name.
std::unique_ptr<ConfigWrapper> library_configuration(rust::Str name);

// `LibraryVersion` is a shared struct defined by cxx in this same namespace
// (see the bridge in lib.rs). Forward-declare it here so the function
// signature below compiles when this header is included before the cxx
// definition.
struct LibraryVersion;

/// Snapshot of every ECMWF library currently registered with
/// `eckit::system::LibraryManager`. Mirrors C++ `Environment::library_versions`.
rust::Vec<LibraryVersion> library_versions();

// ==================== Stream ====================

/// Base wrapper for `eckit::Stream`. Subclasses own the transport-specific
/// resources (socket, buffer, etc.). All read/write methods delegate to the
/// `eckit::Stream*` set by the subclass.
class StreamWrapper {
protected:
  eckit::Stream *stream_ = nullptr;

public:
  virtual ~StreamWrapper() = default;

  StreamWrapper(const StreamWrapper &) = delete;
  StreamWrapper &operator=(const StreamWrapper &) = delete;

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

  /// Access underlying stream for other bridge code.
  eckit::Stream &inner() { return *stream_; }
  const eckit::Stream &inner() const { return *stream_; }

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

protected:
  StreamWrapper() = default;
};

/// TCP stream — connects to host:port via `eckit::net::TCPClient`.
class TcpStreamWrapper : public StreamWrapper {
  eckit::net::TCPSocket socket_;
  eckit::net::TCPStream tcp_;

public:
  TcpStreamWrapper(const std::string &host, int port);
  int64_t read_bytes(rust::Slice<uint8_t> buf) override;
  std::unique_ptr<DataHandleWrapper> into_data_handle() override;
};

/// Resizable memory stream — for writing, buffer grows as needed.
class MemoryWriteStreamWrapper : public StreamWrapper {
  eckit::Buffer buf_;
  eckit::ResizableMemoryStream mem_;

public:
  MemoryWriteStreamWrapper();
  rust::Slice<const uint8_t> buffer() override;
};

/// Fixed memory stream — for reading from existing data.
class MemoryReadStreamWrapper : public StreamWrapper {
  eckit::Buffer buf_;
  eckit::MemoryStream mem_;

public:
  MemoryReadStreamWrapper(rust::Slice<const uint8_t> data);
};

// Factory functions
std::unique_ptr<StreamWrapper> stream_connect(rust::Str host, int32_t port);
std::unique_ptr<StreamWrapper> stream_memory_write();
std::unique_ptr<StreamWrapper>
stream_memory_read(rust::Slice<const uint8_t> data);

} // namespace eckit_bridge
