// eckit DataHandle bridge — implementation.

#include "eckit_exceptions.h"

#include "DataHandleWrapper.h"
#include "eckit-sys/src/lib.rs.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/io/MultiHandle.h"
#include "eckit/io/PartFileHandle.h"
#include "eckit/io/TeeHandle.h"

namespace eckit_bridge {

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

void DataHandleWrapper::close() {
    handle_->close();
}

int64_t DataHandleWrapper::position() const {
    return static_cast<int64_t>(handle_->position());
}

int64_t DataHandleWrapper::seek(int64_t offset) {
    return static_cast<int64_t>(handle_->seek(eckit::Offset(offset)));
}

bool DataHandleWrapper::can_seek() const {
    return handle_->canSeek();
}

int64_t DataHandleWrapper::estimate() const {
    return static_cast<int64_t>(handle_->estimate());
}

int64_t DataHandleWrapper::save_into(DataHandleWrapper& target) {
    return static_cast<int64_t>(handle_->saveInto(*target.handle_));
}

std::unique_ptr<DataHandleWrapper> DataHandleWrapper::from_file(rust::Str path) {
    auto p = eckit::PathName{std::string(path)};
    return std::make_unique<DataHandleWrapper>(p.fileHandle());
}

std::unique_ptr<DataHandleWrapper> DataHandleWrapper::from_part(rust::Str path, int64_t offset, int64_t length) {
    return std::make_unique<DataHandleWrapper>(
        new eckit::PartFileHandle(eckit::PathName{std::string(path)}, eckit::Offset(offset), eckit::Length(length)));
}

std::unique_ptr<DataHandleWrapper> DataHandleWrapper::from_buffer(rust::Slice<const uint8_t> data) {
    return std::make_unique<DataHandleWrapper>(new eckit::MemoryHandle(data.data(), data.size()));
}

std::unique_ptr<DataHandleWrapper> DataHandleWrapper::from_multi(rust::Slice<const rust::String> paths) {
    auto* mh = new eckit::MultiHandle();
    for (const auto& p : paths) {
        (*mh) += eckit::PathName(std::string(p)).fileHandle();
    }
    return std::make_unique<DataHandleWrapper>(mh);
}

std::unique_ptr<DataHandleWrapper> DataHandleWrapper::tee(rust::Slice<const rust::String> paths) {
    std::vector<eckit::DataHandle*> handles;
    handles.reserve(paths.size());
    for (const auto& p : paths) {
        handles.push_back(eckit::PathName(std::string(p)).fileHandle());
    }
    return std::make_unique<DataHandleWrapper>(new eckit::TeeHandle(handles));
}

namespace {

// `eckit::DataHandle` subclass that forwards `read()` / `seek()` to a Rust
// `Read + Seek` source held in a `rust::Box<ReaderBox>`. `openForRead`
// rewinds via `seek(0)` to match `eckit::FileHandle`'s `fopen("r")`
// semantics — eckit's archive pipeline relies on re-opening the source for
// the analyser and per-database passes.
class RustReaderHandle : public eckit::DataHandle {
public:

    explicit RustReaderHandle(rust::Box<ReaderBox> reader) : reader_(std::move(reader)) {}

    void print(std::ostream& s) const override { s << "RustReaderHandle[]"; }

    eckit::Length openForRead() override {
        if (invoke_reader_seek(*reader_, 0) < 0) {
            throw eckit::ReadError("RustReaderHandle: rewind failed on openForRead");
        }
        return eckit::Length(0);
    }

    long read(void* buffer, long length) override {
        if (length <= 0) {
            return 0;
        }
        auto* bytes = static_cast<uint8_t*>(buffer);
        rust::Slice<uint8_t> slice{bytes, static_cast<size_t>(length)};
        int64_t n = invoke_reader_read(*reader_, slice);
        if (n < 0) {
            throw eckit::ReadError("RustReaderHandle: error reading from Rust source");
        }
        return static_cast<long>(n);
    }

    bool canSeek() const override { return true; }

    eckit::Offset seek(const eckit::Offset& offset) override {
        int64_t pos = invoke_reader_seek(*reader_, static_cast<int64_t>(offset));
        if (pos < 0) {
            throw eckit::ReadError("RustReaderHandle: seek failed");
        }
        return eckit::Offset(pos);
    }

    void close() override {}

    eckit::Length estimate() override { return eckit::Length(0); }

    eckit::Length size() override { return eckit::Length(0); }

private:

    rust::Box<ReaderBox> reader_;
};

}  // namespace

std::unique_ptr<DataHandleWrapper> DataHandleWrapper::from_reader(rust::Box<ReaderBox> reader) {
    return std::make_unique<DataHandleWrapper>(new RustReaderHandle(std::move(reader)));
}

}  // namespace eckit_bridge
