/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/MoverTransfer.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Progress.h"
#include "eckit/net/Connector.h"
#include "eckit/runtime/Metrics.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/io/MoverTransferSelection.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

MoverTransfer::MoverTransfer(TransferWatcher& watcher) :
    watcher_(watcher) {}

MoverTransfer::~MoverTransfer() {}

Length MoverTransfer::transfer(DataHandle& from, DataHandle& to) {

    bool send_costs = true;

    if (!from.moveable()) {
        throw SeriousBug(from.title() + " is not moveable");
    }
    if (!to.moveable()) {
        throw SeriousBug(to.title() + " is not moveable");
    }


    // Using node-specific info, determine beneficial nodes to use

    MoverTransferSelection cost;
    from.selectMover(cost, true);
    to.selectMover(cost, false);

    Log::info() << "MoverTransfer::transfer(" << from << "," << to << ")" << std::endl;


    net::Connector& c(net::Connector::service(cost.selectedMover()));
    AutoLock<net::Connector> lock(c);
    // This will close the connector on unlock
    c.autoclose(true);

    Log::message() << c.node() << std::endl;
    Stream& s = c;

    s << bool(false);  // New batch

    // NodeInfo::sendLogin(s);
    // NodeInfo remote = NodeInfo::acceptLogin(s);

    Log::status() << "Sending input handle " << from.title() << std::endl;
    from.toRemote(s);

    Log::status() << "Sending output handle " << to.title() << std::endl;
    to.toRemote(s);

    Length estimate = from.estimate();

    AutoState state('M');
    if (estimate) {
        Log::status() << Bytes(estimate) << " ";
    }
    Log::status() << from.title() << " => " << to.title() << std::endl;

    Progress progress("mover", 0, estimate);
    watcher_.watch(nullptr, 0);
    unsigned long long total = 0;
    bool more;
    s >> more;
    while (more) {
        long pos;
        std::string msg;
        s >> pos;
        if (pos >= 0) {
            s >> msg;
            total += pos;
            progress(total);
            watcher_.watch(nullptr, total);
        }
        else if (pos == -1) {
            watcher_.fromHandleOpened();
        }
        else if (pos == -2) {
            watcher_.toHandleOpened();
        }
        s >> more;
    }

    unsigned long long len;
    s >> len;
    Metrics::receive(s);


    Log::message() << " " << std::endl;


    return len;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
