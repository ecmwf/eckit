// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/EmptyHandle.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ClassSpec EmptyHandle::classSpec_ = {
    &DataHandle::classSpec(),
    "EmptyHandle",
};
Reanimator<EmptyHandle> EmptyHandle::reanimator_;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
