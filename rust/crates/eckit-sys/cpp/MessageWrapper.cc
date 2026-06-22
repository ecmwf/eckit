// eckit Message + Reader bridge — implementation.

#include "eckit_exceptions.h"

#include "MessageWrapper.h"
#include "eckit-sys/src/lib.rs.h"

namespace eckit_bridge {

bool MessageWrapper::is_valid() const {
    return static_cast<bool>(msg_);
}

size_t MessageWrapper::length() const {
    return msg_.length();
}

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
    return rust::Slice<const uint8_t>(static_cast<const uint8_t*>(msg_.data()), msg_.length());
}

void MessageWrapper::write_to(DataHandleWrapper& handle) const {
    msg_.write(handle.inner());
}

std::unique_ptr<MessageWrapper> MessageWrapper::clone() const {
    return std::make_unique<MessageWrapper>(msg_);
}

ReaderWrapper::ReaderWrapper(DataHandleWrapper& handle) :
    reader_(std::make_unique<eckit::message::Reader>(handle.inner(), true)) {}

std::unique_ptr<MessageWrapper> ReaderWrapper::next() {
    auto msg = reader_->next();
    return std::make_unique<MessageWrapper>(std::move(msg));
}

std::unique_ptr<ReaderWrapper> new_reader(DataHandleWrapper& handle) {
    return std::make_unique<ReaderWrapper>(handle);
}

}  // namespace eckit_bridge
