// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/bases/Watcher.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


struct DummyWatcher : public Watcher {
    void watch() {}
};

Watcher& Watcher::dummy() {
    static DummyWatcher x;
    return x;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
