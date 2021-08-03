/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <algorithm>
#include <vector>

#include "eckit/exception/Exceptions.h"

#include "eckit/log/UserChannel.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class UserBuffer : public std::streambuf {
public:
    typedef UserChannel::MsgType MsgType;

    UserBuffer(std::size_t size = 1024) :
        std::streambuf(), buffer_(size + 1), msgType_(UserChannel::NONE), user_(nullptr) {
        ASSERT(size);
        char* base = &buffer_.front();
        setp(base, base + buffer_.size() - 1);  // don't consider the space for '\0'
    }

    ~UserBuffer() { sync(); }

    void msgType(MsgType t) { msgType_ = t; }
    MsgType msgType() const { return msgType_; }

    void userMsg(UserMsg* p) { user_ = p; }
    UserMsg* userMsg() const { return user_; }

private:
    std::vector<char> buffer_;  ///< internal buffer
    MsgType msgType_;           ///< type of next message
    UserMsg* user_;             ///< interface to user messeges

    bool dumpBuffer() {
        std::replace(pbase(), epptr(), '\n', '\0');

        switch (msgType_) {
            case UserChannel::NONE:
                break;
            case UserChannel::INFO:
                if (user_)
                    user_->infoMsg(pbase());
                break;
            case UserChannel::WARN:
                if (user_)
                    user_->warningMsg(pbase());
                break;
            case UserChannel::ERROR:
                if (user_)
                    user_->errorMsg(pbase());
                break;
        }

        setp(pbase(), epptr());
        return true;
    }

protected:
    virtual int_type overflow(int_type ch) {
        if (ch == traits_type::eof()) {
            return sync();
        }
        dumpBuffer();
        sputc(ch);
        return traits_type::to_int_type(ch);
    }

    virtual int_type sync() { return (dumpBuffer() ? 0 : -1); }
};

//----------------------------------------------------------------------------------------------------------------------

UserChannel::UserChannel() :
    std::ostream(new UserBuffer()), buffer_(dynamic_cast<UserBuffer*>(rdbuf())) {
    ASSERT(buffer_);
}

UserChannel::~UserChannel() {
    delete buffer_;
}

void UserChannel::msgType(UserChannel::MsgType t) {
    buffer_->msgType(t);
}

void UserChannel::userMsg(UserMsg* p) {
    buffer_->userMsg(p);
}

UserMsg* UserChannel::userMsg() const {
    return buffer_->userMsg();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
