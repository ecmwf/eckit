/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/thread/AutoLock.h"
#include "eckit/log/Bytes.h"
#include "eckit/net/Connector.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/io/MoverTransfer.h"
#include "eckit/io/cluster/NodeInfo.h"
#include "eckit/log/Progress.h"
#include "eckit/net/TCPServer.h"
#include "eckit/net/TCPStream.h"
#include "eckit/thread/Thread.h"
#include "eckit/thread/ThreadControler.h"
#include "eckit/io/cluster/ClusterNodes.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

MoverTransfer::MoverTransfer(TransferWatcher& watcher):
        watcher_(watcher)
{
}

MoverTransfer::~MoverTransfer()
{
}

Length MoverTransfer::transfer(DataHandle& from, DataHandle& to)
{

    if (!from.moveable()) throw SeriousBug(from.title() + " is not moveable");
    if (!to.moveable()) throw SeriousBug(to.title() + " is not moveable");

    std::map<std::string,Length> cost;
    from.cost(cost, true);
    to.cost(cost, false);

    if (cost.empty()) {
        NodeInfo info = ClusterNodes::any("mover");
        cost[info.node()] = 0;
//        throw SeriousBug(std::string("No cost for ") + from.title() + " => " + to.title());
    }

    Log::info() << "MoverTransfer::transfer(" << from << "," << to << ")" << std::endl;;

    Log::info() << "MoverTransfer::transfer cost:" << std::endl;
    for (std::map<std::string,Length> ::iterator j = cost.begin() ; j != cost.end() ; ++j)
        Log::info() << "   " << (*j).first << " => " << Bytes((*j).second) << std::endl;

    Connector& c(Connector::service("mover", cost));

    Log::message() << c.host() << std::endl;
    Stream& s = c;

	s << bool(false); // New batch

    //NodeInfo::sendLogin(s);
    //NodeInfo remote = NodeInfo::acceptLogin(s);

    Log::status() << "Sending input handle " << from.title() << std::endl;
    from.toRemote(s);

    Log::status() << "Sending output handle " << to.title() << std::endl;
    to.toRemote(s);

    Length estimate = from.estimate();

    AutoState state('M');
    if(estimate)
        Log::status() << Bytes(estimate) << " " ;
    Log::status() << from.title() << " => " << to.title() << std::endl;

    Progress progress("mover", 0, estimate);
    unsigned long long total = 0;
    bool more;
    s >> more;
    while(more)
    {
        long pos;
        std::string msg;
        s >> pos;
        s >> msg;
        total += pos;
        progress(total);
        s >> more;
    }

    unsigned long long len;
    s >> len;

//    ASSERT(len == total);

    Log::message() << " " << std::endl;


    return len;

}

//-----------------------------------------------------------------------------

} // namespace eckit

