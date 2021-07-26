/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/mpi/Status.h"

#include "eckit/exception/Exceptions.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

Status::Status() :
    content_(new NullStatus()) {
    content_->attach();
}

Status::Status(StatusContent* p) :
    content_(p) {
    ASSERT(p);
    content_->attach();
}

Status::~Status() {
    content_->detach();
}

Status::Status(const Status& s) :
    content_(s.content_) {
    content_->attach();
}

Status& Status::operator=(const Status& s) {
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

}  // namespace mpi
}  // namespace eckit
