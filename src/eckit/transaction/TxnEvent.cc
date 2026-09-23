// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/transaction/TxnEvent.h"

#include <ostream>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ClassSpec TxnEvent::classSpec_ = {
    &Streamable::classSpec(),
    "TxnEvent",
};
Reanimator<TxnEvent> TxnEvent::reanimator_;

TxnEvent::TxnEvent() : txnID_(0) {}

TxnEvent::TxnEvent(Stream& s) : txnID_(0) {
    s >> txnID_;
}

void TxnEvent::encode(Stream& s) const {
    s << txnID_;
}

TxnEvent::~TxnEvent() {}

void TxnEvent::print(std::ostream& s) const {
    s << "TxnEvent " << txnID_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
