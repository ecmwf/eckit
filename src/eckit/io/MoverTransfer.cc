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
#include "eckit/io/cluster/ClusterNodes.h"
#include "eckit/io/cluster/NodeInfo.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Progress.h"
#include "eckit/net/Connector.h"
#include "eckit/net/TCPServer.h"
#include "eckit/net/TCPStream.h"
#include "eckit/runtime/Metrics.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Thread.h"
#include "eckit/thread/ThreadControler.h"
#include "eckit/value/Value.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

MoverTransfer::MoverTransfer(TransferWatcher& watcher) :
    watcher_(watcher) {}

MoverTransfer::~MoverTransfer() {}

Length MoverTransfer::transfer(DataHandle& from, DataHandle& to) {

    bool send_costs = true;

    if (!from.moveable())
        throw SeriousBug(from.title() + " is not moveable");
    if (!to.moveable())
        throw SeriousBug(to.title() + " is not moveable");

    // Attributes that are required from the mover

    std::set<std::string> moverAttributes;
    {
        auto&& f = from.requiredMoverAttributes();
        moverAttributes.insert(f.begin(), f.end());
        auto&& t = to.requiredMoverAttributes();
        moverAttributes.insert(t.begin(), t.end());
    }

    // Using node-specific info, determine beneficial nodes to use

    std::map<std::string, Length> cost;
    from.cost(cost, true);
    to.cost(cost, false);

    // Should any of the nodes be removed from the cost matrix, because they don't support
    // the required attributes?
    // Also remove any movers that are not up

    for (auto it = cost.begin(); it != cost.end(); /* no increment */) {
        if (ClusterNodes::available("mover", it->first)) {
            if (!ClusterNodes::lookUp("mover", it->first).supportsAttributes(moverAttributes)) {
                cost.erase(it++);
            } else {
                ++it;
            }
        } else {
            cost.erase(it++);
        }
    }

    if (cost.empty()) {
        NodeInfo info     = ClusterNodes::any("mover", moverAttributes);
        cost[info.node()] = 0;
        send_costs        = false;
        //        throw SeriousBug(std::string("No cost for ") + from.title() + " => " + to.title());
    }

    Log::info() << "MoverTransfer::transfer(" << from << "," << to << ")" << std::endl;

    Log::info() << "MoverTransfer::transfer cost:" << std::endl;
    for (std::map<std::string, Length>::iterator j = cost.begin(); j != cost.end(); ++j)
        Log::info() << "   " << (*j).first << " => " << Bytes((*j).second) << std::endl;

    net::Connector& c(net::Connector::service("mover", cost));
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
    if (estimate)
        Log::status() << Bytes(estimate) << " ";
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


    Metrics::set("mover_node", c.node());
    if (send_costs) {
        for (auto j = cost.begin(); j != cost.end(); ++j) {
            std::string h        = (*j).first;
            unsigned long long l = (*j).second;
            Metrics::set("mover_costs." + h, l);
        }
    }
    // Metrics::set("mover_metric", prefix_);
    // //    ASSERT(len == total);

    Log::message() << " " << std::endl;


    return len;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
