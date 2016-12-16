/*
 * (C) Copyright 1996-2017 ECMWF.
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
    content_(0) {
}

Status::Status(StatusContent* p) :
    content_(p) {
    content_->attach();
}

Status::~Status() {
    if( content_ ) {
        content_->detach();
    }
}

Status::Status(const Status& s) : content_(s.content_) {
    content_->attach();
}

Status& Status::operator=(const Status& s) {
    if( content_ ) {
        content_->detach();
    }
    content_ = s.content_;
    content_->attach();
    return *this;
}

StatusContent::~StatusContent() {
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace mpi
} // namepsace eckit
