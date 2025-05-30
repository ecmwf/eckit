/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/codec/Session.h"

#include <atomic>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>

#include "eckit/codec/Exceptions.h"
#include "eckit/filesystem/PathName.h"

namespace eckit::codec {

using lock_guard = std::lock_guard<std::recursive_mutex>;


class SessionImpl {
public:

    void store(Stream stream);
    Record record(const std::string& path, size_t offset);

private:

    std::recursive_mutex mutex_;

    std::vector<Stream> handles_;
    std::map<std::string, Record> records_;
};

//---------------------------------------------------------------------------------------------------------------------

class ActiveSession {
public:

    static ActiveSession& instance();
    SessionImpl& current();
    void push();
    void pop();

    Record record(const std::string& path, size_t offset);

    void store(Stream stream);

private:

    friend class Session;
    std::recursive_mutex mutex_;

    std::unique_ptr<SessionImpl> session_;
    std::atomic<size_t> count_{0};
};

//---------------------------------------------------------------------------------------------------------------------

ActiveSession& ActiveSession::instance() {
    static ActiveSession instance;
    return instance;
}

//---------------------------------------------------------------------------------------------------------------------

Record ActiveSession::record(const std::string& path, size_t offset) {
    if (count_ != 0) {
        return current().record(path, offset);
    }

    return {};
}

//---------------------------------------------------------------------------------------------------------------------

void ActiveSession::store(Stream stream) {
    if (count_ != 0) {
        return current().store(stream);
    }
}

//---------------------------------------------------------------------------------------------------------------------

SessionImpl& ActiveSession::current() {
    lock_guard lock(mutex_);
    if (count_ == 0) {
        throw Exception("No eckit::codec session is currently active", Here());
    }
    return *session_;
}

//---------------------------------------------------------------------------------------------------------------------

void ActiveSession::push() {
    lock_guard lock(mutex_);
    if (count_ == 0) {
        ASSERT(session_ == nullptr);
        session_ = std::make_unique<SessionImpl>();
    }
    ++count_;
}

//---------------------------------------------------------------------------------------------------------------------

void ActiveSession::pop() {
    lock_guard lock(mutex_);
    if (count_ == 0) {
        throw Exception("No eckit::codec session is currently active", Here());
    }
    --count_;
    if (count_ == 0) {
        session_.reset();
    }
}

//---------------------------------------------------------------------------------------------------------------------

void SessionImpl::store(Stream stream) {
    lock_guard lock(mutex_);
    handles_.emplace_back(stream);
}

//---------------------------------------------------------------------------------------------------------------------

Record SessionImpl::record(const std::string& path, size_t offset) {
    lock_guard lock(mutex_);
    auto key = Record::URI{PathName(path).fullName(), offset}.str();
    if (records_.find(key) == records_.end()) {
        records_.emplace(key, Record{});
    }
    return records_.at(key);
}

//---------------------------------------------------------------------------------------------------------------------

Session::Session() {
    ActiveSession::instance().push();
}

//---------------------------------------------------------------------------------------------------------------------

Session::~Session() {
    ActiveSession::instance().pop();
}

//---------------------------------------------------------------------------------------------------------------------

bool Session::active() {
    return ActiveSession::instance().count_ > 0;
}

//---------------------------------------------------------------------------------------------------------------------

Record Session::record(const std::string& path, size_t offset) {
    return ActiveSession::instance().record(path, offset);
}

//---------------------------------------------------------------------------------------------------------------------

Record Session::record(Stream stream, size_t offset) {
    std::stringstream id;
    id << &stream.datahandle();
    return ActiveSession::instance().record(id.str(), offset);
}

//---------------------------------------------------------------------------------------------------------------------

void Session::store(Stream stream) {
    ActiveSession::instance().store(stream);
}

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec
