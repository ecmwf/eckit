/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <ostream>

#include "eckit/transaction/TxnEvent.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ClassSpec TxnEvent::classSpec_ = {
    &Streamable::classSpec(),
    "TxnEvent",
};
Reanimator<TxnEvent> TxnEvent::reanimator_;

TxnEvent::TxnEvent() :
    txnID_(0) {}

TxnEvent::TxnEvent(Stream& s) :
    txnID_(0) {
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
