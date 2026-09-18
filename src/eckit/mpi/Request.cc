// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/mpi/Request.h"

#include "eckit/mpi/Comm.h"

namespace eckit::mpi {

//----------------------------------------------------------------------------------------------------------------------

class NullRequestContent : public RequestContent {
public:

    virtual ~NullRequestContent() {}

    virtual void print(std::ostream& os) const { os << "NullRequest()"; }

    virtual int request() const { return -1; }

    virtual bool test() { return true; }
};

//----------------------------------------------------------------------------------------------------------------------

Request::Request() : content_(new NullRequestContent()) {
    content_->attach();
}

Request::Request(int request) : content_{nullptr} {
    *this = eckit::mpi::comm().request(request);
}

Request::Request(RequestContent* p) : content_(p) {
    content_->attach();
}

Request::~Request() {
    content_->detach();
}

Request::Request(const Request& s) : content_(s.content_) {
    content_->attach();
}

Request& Request::operator=(const Request& s) {
    if (this == &s) {
        return *this;
    }

    if (content_) {
        content_->detach();
    }
    content_ = s.content_;
    content_->attach();
    return *this;
}

int Request::request() const {
    return content_->request();
}

bool Request::test() {
    return content_->test();
}

void Request::print(std::ostream& out) const {
    content_->print(out);
}

RequestContent::~RequestContent() {}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::mpi
