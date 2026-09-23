// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/TransferWatcher.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


struct DummyTransferWatcher : public TransferWatcher {
    void watch(const void*, long) {}
};

TransferWatcher& TransferWatcher::dummy() {
    static DummyTransferWatcher x;
    return x;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
