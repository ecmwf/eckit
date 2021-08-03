/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/eckit.h"

#include "eckit/config/Resource.h"
#include "eckit/io/cluster/NodeInfo.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Log.h"

#include "eckit/io/HandleHolder.h"
#include "eckit/log/BigNum.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Seconds.h"


//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

HandleHolder::HandleHolder(DataHandle& handle) :
    handle_(&handle), owned_(false) {}

HandleHolder::HandleHolder(DataHandle* handle) :
    handle_(handle), owned_(true) {}

HandleHolder::~HandleHolder() {
    if (owned_)
        delete handle_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
