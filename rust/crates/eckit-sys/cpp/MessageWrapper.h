// eckit Message + Reader bridge — iterate GRIB messages over a DataHandle.
#pragma once

#include "DataHandleWrapper.h"
#include "eckit/message/Message.h"
#include "eckit/message/Reader.h"

#include "rust/cxx.h"

#include <cstddef>
#include <cstdint>
#include <memory>

namespace eckit_bridge {

//----------------------------------------------------------------------------------------------------------------------

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
    void write_to(DataHandleWrapper& handle) const;

    /// Clone (Message is ref-counted internally).
    std::unique_ptr<MessageWrapper> clone() const;

    /// Access underlying for other C++ bridge code.
    const eckit::message::Message& inner() const { return msg_; }
    eckit::message::Message& inner() { return msg_; }
};

//----------------------------------------------------------------------------------------------------------------------

/// Wraps `eckit::message::Reader` for Rust FFI.
/// Reads messages from a `DataHandle`.
class ReaderWrapper {
    std::unique_ptr<eckit::message::Reader> reader_;

public:

    explicit ReaderWrapper(DataHandleWrapper& handle);

    /// Returns next message, or an invalid message when exhausted.
    std::unique_ptr<MessageWrapper> next();
};

//----------------------------------------------------------------------------------------------------------------------

// Factory
std::unique_ptr<ReaderWrapper> new_reader(DataHandleWrapper& handle);

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit_bridge
