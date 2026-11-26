// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/mpi/Status.h"

#include "eckit/exception/Exceptions.h"

namespace eckit::mpi {

//----------------------------------------------------------------------------------------------------------------------

Status::Status() : content_(new NullStatus()) {
    content_->attach();
}

Status::Status(StatusContent* p) : content_(p) {
    ASSERT(p);
    content_->attach();
}

Status::~Status() {
    content_->detach();
}

Status::Status(const Status& s) : content_(s.content_) {
    content_->attach();
}

Status& Status::operator=(const Status& s) {
    if (this == &s) {
        return *this;
    }

    content_->detach();
    content_ = s.content_;
    content_->attach();
    return *this;
}

StatusContent::~StatusContent() {}

void NullStatus::print(std::ostream& os) const {
    os << "NullStatus("
       << "source=" << source() << ",tag=" << tag() << ",error=" << error() << ")";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::mpi
