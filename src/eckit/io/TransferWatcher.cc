/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


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
