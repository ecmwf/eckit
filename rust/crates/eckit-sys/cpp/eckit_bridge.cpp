// eckit C++ bridge implementation
#include "eckit_bridge.h"
#include "eckit-sys/src/lib.rs.h" // cxx-generated — provides LogLevel values

namespace eckit_bridge {

// ==================== Logging ====================

void RustLogTarget::write(const char *start, const char *end) {
  buffer_.append(start, end);

  std::string::size_type pos;
  while ((pos = buffer_.find('\n')) != std::string::npos) {
    std::string line = buffer_.substr(0, pos);
    while (!line.empty() && (line.back() == '\r' || line.back() == ' ')) {
      line.pop_back();
    }
    if (!line.empty()) {
      rust_log(level_, rust::Str(line.data(), line.size()));
    }
    buffer_.erase(0, pos + 1);
  }
}

void RustLogTarget::flush() {
  if (!buffer_.empty()) {
    while (!buffer_.empty() &&
           (buffer_.back() == '\r' || buffer_.back() == ' ')) {
      buffer_.pop_back();
    }
    if (!buffer_.empty()) {
      rust_log(level_, rust::Str(buffer_.data(), buffer_.size()));
      buffer_.clear();
    }
  }
}

RustMain::RustMain(int argc, char **argv) : Main(argc, argv) {}

eckit::LogTarget *RustMain::createInfoLogTarget() const {
  return new RustLogTarget(LogLevel::Info);
}
eckit::LogTarget *RustMain::createWarningLogTarget() const {
  return new RustLogTarget(LogLevel::Warn);
}
eckit::LogTarget *RustMain::createErrorLogTarget() const {
  return new RustLogTarget(LogLevel::Error);
}
eckit::LogTarget *RustMain::createDebugLogTarget() const {
  return new RustLogTarget(LogLevel::Debug);
}
eckit::LogTarget *RustMain::createMetricsLogTarget() const {
  return new RustLogTarget(LogLevel::Trace);
}

void init() {
  if (!eckit::Main::ready()) {
    static const char *argv[] = {"eckit-rs", nullptr};
    static auto *main = new RustMain(1, const_cast<char **>(argv));
    (void)main;
  }
}

// ==================== Configuration ====================

bool ConfigWrapper::has(rust::Str key) const {
  return config_.has(std::string(key));
}

bool ConfigWrapper::is_list(rust::Str key) const {
  return config_.isList(std::string(key));
}

bool ConfigWrapper::is_empty() const { return config_.empty(); }

rust::String ConfigWrapper::get_string(rust::Str key,
                                       rust::Str default_val) const {
  return rust::String(
      config_.getString(std::string(key), std::string(default_val)));
}

int64_t ConfigWrapper::get_long(rust::Str key, int64_t default_val) const {
  return config_.getLong(std::string(key), default_val);
}

int32_t ConfigWrapper::get_int(rust::Str key, int32_t default_val) const {
  return config_.getInt(std::string(key), default_val);
}

bool ConfigWrapper::get_bool(rust::Str key, bool default_val) const {
  return config_.getBool(std::string(key), default_val);
}

double ConfigWrapper::get_double(rust::Str key, double default_val) const {
  return config_.getDouble(std::string(key), default_val);
}

rust::Vec<rust::String> ConfigWrapper::get_string_vector(
    rust::Str key, const rust::Vec<rust::String> &default_val) const {
  std::vector<std::string> def;
  def.reserve(default_val.size());
  for (const auto &s : default_val) {
    def.emplace_back(std::string(s));
  }
  auto vec = config_.getStringVector(std::string(key), def);
  rust::Vec<rust::String> result;
  result.reserve(vec.size());
  for (const auto &s : vec) {
    result.push_back(rust::String(s));
  }
  return result;
}

std::unique_ptr<ConfigWrapper> ConfigWrapper::get_sub(rust::Str key) const {
  return std::make_unique<ConfigWrapper>(
      config_.getSubConfiguration(std::string(key)));
}

size_t ConfigWrapper::sub_count(rust::Str key) const {
  return config_.getSubConfigurations(std::string(key)).size();
}

std::unique_ptr<ConfigWrapper> ConfigWrapper::sub_at(rust::Str key,
                                                     size_t index) const {
  auto subs = config_.getSubConfigurations(std::string(key));
  return std::make_unique<ConfigWrapper>(subs.at(index));
}

size_t ConfigWrapper::root_sub_count() const {
  return config_.getSubConfigurations().size();
}

std::unique_ptr<ConfigWrapper> ConfigWrapper::root_sub_at(size_t index) const {
  auto subs = config_.getSubConfigurations();
  return std::make_unique<ConfigWrapper>(subs.at(index));
}

void ConfigWrapper::set_string(rust::Str key, rust::Str value) {
  config_.set(std::string(key), std::string(value));
}

void ConfigWrapper::set_long(rust::Str key, int64_t value) {
  config_.set(std::string(key), static_cast<long>(value));
}

void ConfigWrapper::set_int(rust::Str key, int32_t value) {
  config_.set(std::string(key), value);
}

void ConfigWrapper::set_bool(rust::Str key, bool value) {
  config_.set(std::string(key), value);
}

void ConfigWrapper::set_double(rust::Str key, double value) {
  config_.set(std::string(key), value);
}

void ConfigWrapper::remove(rust::Str key) { config_.remove(std::string(key)); }

std::unique_ptr<ConfigWrapper> create() {
  return std::make_unique<ConfigWrapper>();
}

std::unique_ptr<ConfigWrapper> from_path(rust::Str path) {
  auto p = eckit::PathName{std::string(path)};
  auto yaml = eckit::YAMLConfiguration{p};
  return std::make_unique<ConfigWrapper>(yaml);
}

std::unique_ptr<ConfigWrapper> from_yaml(rust::Str yaml) {
  auto str = std::string(yaml);
  auto parsed = eckit::YAMLConfiguration{str};
  return std::make_unique<ConfigWrapper>(parsed);
}

std::unique_ptr<ConfigWrapper> clone(const ConfigWrapper &src) {
  return std::make_unique<ConfigWrapper>(src.inner());
}

// ==================== DataHandle ====================

int64_t DataHandleWrapper::open_for_read() {
  return static_cast<int64_t>(handle_->openForRead());
}

void DataHandleWrapper::open_for_write(int64_t estimated_length) {
  handle_->openForWrite(eckit::Length(estimated_length));
}

int64_t DataHandleWrapper::read(rust::Slice<uint8_t> buf) {
  return handle_->read(buf.data(), static_cast<long>(buf.size()));
}

int64_t DataHandleWrapper::write(rust::Slice<const uint8_t> buf) {
  return handle_->write(buf.data(), static_cast<long>(buf.size()));
}

void DataHandleWrapper::close() { handle_->close(); }

int64_t DataHandleWrapper::position() const {
  return static_cast<int64_t>(handle_->position());
}

int64_t DataHandleWrapper::seek(int64_t offset) {
  return static_cast<int64_t>(handle_->seek(eckit::Offset(offset)));
}

bool DataHandleWrapper::can_seek() const { return handle_->canSeek(); }

int64_t DataHandleWrapper::estimate() const {
  return static_cast<int64_t>(handle_->estimate());
}

int64_t DataHandleWrapper::save_into(DataHandleWrapper &target) {
  return static_cast<int64_t>(handle_->saveInto(*target.handle_));
}

std::unique_ptr<DataHandleWrapper> data_handle_from_file(rust::Str path) {
  auto p = eckit::PathName{std::string(path)};
  return std::make_unique<DataHandleWrapper>(p.fileHandle());
}

std::unique_ptr<DataHandleWrapper>
data_handle_from_part(rust::Str path, int64_t offset, int64_t length) {
  return std::make_unique<DataHandleWrapper>(
      new eckit::PartFileHandle(eckit::PathName{std::string(path)},
                                eckit::Offset(offset), eckit::Length(length)));
}

std::unique_ptr<DataHandleWrapper>
data_handle_from_buffer(rust::Slice<const uint8_t> data) {
  return std::make_unique<DataHandleWrapper>(
      new eckit::MemoryHandle(data.data(), data.size()));
}

std::unique_ptr<DataHandleWrapper>
data_handle_from_multi(rust::Slice<const rust::String> paths) {
  auto *mh = new eckit::MultiHandle();
  for (const auto &p : paths) {
    (*mh) += eckit::PathName(std::string(p)).fileHandle();
  }
  return std::make_unique<DataHandleWrapper>(mh);
}

std::unique_ptr<DataHandleWrapper>
data_handle_tee(rust::Slice<const rust::String> paths) {
  std::vector<eckit::DataHandle *> handles;
  handles.reserve(paths.size());
  for (const auto &p : paths) {
    handles.push_back(eckit::PathName(std::string(p)).fileHandle());
  }
  return std::make_unique<DataHandleWrapper>(new eckit::TeeHandle(handles));
}

// ==================== Message + Reader ====================

bool MessageWrapper::is_valid() const { return static_cast<bool>(msg_); }

size_t MessageWrapper::length() const { return msg_.length(); }

int64_t MessageWrapper::offset() const {
  // eckit::Offset has operator long long(), so cast through that.
  return static_cast<int64_t>(static_cast<long long>(msg_.offset()));
}

rust::String MessageWrapper::get_string(rust::Str key) const {
  return rust::String(msg_.getString(std::string(key)));
}

int64_t MessageWrapper::get_long(rust::Str key) const {
  return msg_.getLong(std::string(key));
}

double MessageWrapper::get_double(rust::Str key) const {
  return msg_.getDouble(std::string(key));
}

rust::Slice<const uint8_t> MessageWrapper::data() const {
  return rust::Slice<const uint8_t>(static_cast<const uint8_t *>(msg_.data()),
                                    msg_.length());
}

void MessageWrapper::write_to(DataHandleWrapper &handle) const {
  msg_.write(handle.inner());
}

std::unique_ptr<MessageWrapper> MessageWrapper::clone() const {
  return std::make_unique<MessageWrapper>(msg_);
}

ReaderWrapper::ReaderWrapper(DataHandleWrapper &handle)
    : reader_(std::make_unique<eckit::message::Reader>(handle.inner(), true)) {}

std::unique_ptr<MessageWrapper> ReaderWrapper::next() {
  auto msg = reader_->next();
  return std::make_unique<MessageWrapper>(std::move(msg));
}

std::unique_ptr<ReaderWrapper> new_reader(DataHandleWrapper &handle) {
  return std::make_unique<ReaderWrapper>(handle);
}

// ==================== Library registration ====================

RustLibrary::RustLibrary(rust::Box<LibraryBox> lib)
    : eckit::system::Library(std::string(library_name(*lib))),
      rust_(std::move(lib)) {}

std::string RustLibrary::version() const {
  return std::string(library_version(*rust_));
}

std::string RustLibrary::gitsha1(unsigned int count) const {
  return std::string(library_git_sha1(*rust_, count));
}

std::string RustLibrary::home() const {
  if (library_home_is_set(*rust_)) {
    return std::string(library_home(*rust_));
  }
  return Library::home();
}

std::string RustLibrary::libraryHome() const {
  if (library_library_home_is_set(*rust_)) {
    return std::string(library_library_home(*rust_));
  }
  return Library::libraryHome();
}

std::string RustLibrary::prefixDirectory() const {
  if (library_prefix_directory_is_set(*rust_)) {
    return std::string(library_prefix_directory(*rust_));
  }
  return Library::prefixDirectory();
}

std::string RustLibrary::expandPath(const std::string &path) const {
  rust::Str rpath(path.data(), path.size());
  if (library_expand_path_is_set(*rust_, rpath)) {
    return std::string(library_expand_path(*rust_, rpath));
  }
  return Library::expandPath(path);
}

bool RustLibrary::debug() const {
  if (library_debug_is_set(*rust_)) {
    return library_debug(*rust_);
  }
  return Library::debug();
}

const void *RustLibrary::addr() const {
  // Return a pointer inside the code segment so dladdr can resolve the
  // binary path. `this` is heap-allocated and dladdr cannot resolve it.
  return reinterpret_cast<const void *>(&register_library);
}

void register_library(rust::Box<LibraryBox> lib) {
  // Heap-allocate and leak — Library registers itself with LibraryManager
  // in its constructor and must live for the process lifetime.
  new RustLibrary(std::move(lib));
}

std::unique_ptr<ConfigWrapper> library_configuration(rust::Str name) {
  auto &lib = eckit::system::LibraryManager::lookup(std::string(name));
  const auto &cfg = lib.configuration();
  return std::make_unique<ConfigWrapper>(eckit::LocalConfiguration(cfg));
}

// ==================== Stream (base class) ====================

void StreamWrapper::write_char(uint8_t c) { *stream_ << static_cast<char>(c); }
void StreamWrapper::write_bool(bool v) { *stream_ << v; }
void StreamWrapper::write_int(int32_t v) { *stream_ << v; }
void StreamWrapper::write_long(int64_t v) {
  *stream_ << static_cast<long long>(v);
}
void StreamWrapper::write_unsigned_long(uint64_t v) {
  *stream_ << static_cast<unsigned long long>(v);
}
void StreamWrapper::write_double(double v) { *stream_ << v; }
void StreamWrapper::write_string(rust::Str v) {
  *stream_ << std::string(v.data(), v.size());
}
void StreamWrapper::write_blob(rust::Slice<const uint8_t> data) {
  stream_->writeBlob(data.data(), data.size());
}

uint8_t StreamWrapper::read_char() {
  char c{};
  *stream_ >> c;
  return static_cast<uint8_t>(c);
}
bool StreamWrapper::read_bool() {
  bool v{};
  *stream_ >> v;
  return v;
}
int32_t StreamWrapper::read_int() {
  int v{};
  *stream_ >> v;
  return v;
}
int64_t StreamWrapper::read_long() {
  long long v{};
  *stream_ >> v;
  return static_cast<int64_t>(v);
}
uint64_t StreamWrapper::read_unsigned_long() {
  unsigned long long v{};
  *stream_ >> v;
  return static_cast<uint64_t>(v);
}
double StreamWrapper::read_double() {
  double v{};
  *stream_ >> v;
  return v;
}
rust::String StreamWrapper::read_string() {
  std::string v;
  *stream_ >> v;
  return rust::String(v);
}

int64_t StreamWrapper::read_bytes(rust::Slice<uint8_t> buf) {
  throw eckit::SeriousBug("read_bytes not supported on this stream type");
}

rust::Slice<const uint8_t> StreamWrapper::buffer() {
  throw eckit::SeriousBug("buffer() not supported on this stream type");
}

int64_t StreamWrapper::bytes_written() const { return stream_->bytesWritten(); }

// ==================== TcpStreamWrapper ====================

TcpStreamWrapper::TcpStreamWrapper(const std::string &host, int port)
    : socket_(eckit::net::TCPClient().connect(host, port)), tcp_(socket_) {
  stream_ = &tcp_;
}

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
  return std::make_unique<DataHandleWrapper>(
      new eckit::TCPSocketHandle(tcp_.socket()));
}

std::unique_ptr<DataHandleWrapper> StreamWrapper::into_data_handle() {
  throw eckit::NotImplemented(
      "StreamWrapper::into_data_handle is only supported on TCP streams",
      Here());
}

rust::Vec<LibraryVersion> library_versions() {
  rust::Vec<LibraryVersion> out;
  constexpr size_t sha1len = 8;
  for (const auto &name : eckit::system::LibraryManager::list()) {
    const auto &lib = eckit::system::LibraryManager::lookup(name);
    out.push_back(LibraryVersion{
        rust::String(lib.name()), rust::String(lib.version()),
        rust::String(lib.gitsha1(sha1len)), rust::String(lib.libraryHome())});
  }
  return out;
}

// ==================== MemoryWriteStreamWrapper ====================

MemoryWriteStreamWrapper::MemoryWriteStreamWrapper() : buf_(4096), mem_(buf_) {
  stream_ = &mem_;
}

rust::Slice<const uint8_t> MemoryWriteStreamWrapper::buffer() {
  return {static_cast<const uint8_t *>(buf_.data()),
          static_cast<size_t>(mem_.bytesWritten())};
}

// ==================== MemoryReadStreamWrapper ====================

MemoryReadStreamWrapper::MemoryReadStreamWrapper(
    rust::Slice<const uint8_t> data)
    : buf_(data.data(), data.size()), mem_(buf_) {
  stream_ = &mem_;
}

// ==================== Factory functions ====================

std::unique_ptr<StreamWrapper> stream_connect(rust::Str host, int32_t port) {
  return std::make_unique<TcpStreamWrapper>(std::string(host), port);
}

std::unique_ptr<StreamWrapper> stream_memory_write() {
  return std::make_unique<MemoryWriteStreamWrapper>();
}

std::unique_ptr<StreamWrapper>
stream_memory_read(rust::Slice<const uint8_t> data) {
  return std::make_unique<MemoryReadStreamWrapper>(data);
}

} // namespace eckit_bridge
