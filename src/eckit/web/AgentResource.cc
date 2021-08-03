/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstring>
#include <fstream>

#include "eckit/io/Buffer.h"
#include "eckit/web/AgentResource.h"
#include "eckit/web/JavaAgent.h"
#include "eckit/web/Url.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class MemStream : public eckit::Stream {

    Buffer in_;
    Buffer out_;
    long pos_;
    long length_;

    virtual long write(const void* buf, long len);
    virtual long read(void* buf, long len);
    virtual std::string name() const { return "MemStream"; }

public:
    MemStream(const char* p, long len);

    long length() const { return length_; }
    const char* data() const { return out_; }
};


MemStream::MemStream(const char* p, long len) :
    in_(p, len), out_(10240), pos_(0), length_(0) {}

long MemStream::write(const void* buf, long len) {
    if (out_.size() - length_ < static_cast<size_t>(len))
        out_.resize(out_.size() * 2);

    ::memcpy((char*)out_ + length_, buf, len);

    length_ += len;
    return len;
}

long MemStream::read(void* buf, long len) {
    long size = std::min(long(len), long(in_.size() - pos_));

    if (size <= 0)
        return -1;

    ::memcpy(buf, (char*)in_ + pos_, size);
    pos_ += size;

    return size;
}

AgentResource::AgentResource() :
    HtmlResource("/agent") {}

AgentResource::~AgentResource() {}

void AgentResource::GET(std::ostream&, Url& url) {
    static std::ifstream in("/dev/null");
    static std::ofstream out("/dev/null");

    std::string content = url.headerIn().content();
    MemStream s(content.data(), content.length());

    std::string token = url.headerIn().getHeader("mars-token");

    std::cout << "Token is " << token << std::endl;

    JavaAgent::serve(s, in, out);
    url.headerOut().content(s.data(), s.length());

    token = "token";
    url.headerOut().setHeader("mars-token", token);
}

static AgentResource agentResourceInstance;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
