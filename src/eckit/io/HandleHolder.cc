/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/HandleHolder.h"


//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

HandleHolder::HandleHolder(DataHandle& handle) : handle_(&handle), owned_(false) {}

HandleHolder::HandleHolder(DataHandle* handle) : handle_(handle), owned_(true) {}

HandleHolder::~HandleHolder() {
    if (owned_) {
        delete handle_;
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
