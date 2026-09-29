// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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

HandleHolder::HandleHolder(DataHandle& handle) : handle_(&handle), owned_(false) {}

HandleHolder::HandleHolder(DataHandle* handle) : handle_(handle), owned_(true) {}

HandleHolder::~HandleHolder() {
    if (owned_) {
        delete handle_;
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
