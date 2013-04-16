/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/thread/AutoLock.h"
#include "eclib/Bytes.h"
#include "eclib/Connector.h"
#include "eclib/Monitor.h"
#include "eclib/MoverTransfer.h"
#include "eclib/NodeInfo.h"
#include "eclib/Progress.h"
#include "eclib/Resource.h"
#include "eclib/TCPServer.h"
#include "eclib/TCPStream.h"
#include "eckit/thread/Thread.h"
#include "eckit/thread/ThreadControler.h"
#include "eclib/ClusterNodes.h"

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

    map<string,Length> cost;
    from.cost(cost, true);
    to.cost(cost, false);

    if (cost.size() == 0) {
        NodeInfo info = ClusterNodes::any("mover");
        cost[info.node()] = 0;
//        throw SeriousBug(string("No cost for ") + from.title() + " => " + to.title());
    }

    Log::info() << "MoverTransfer::transfer(" << from << "," << to << ")" << endl;;

    Log::info() << "MoverTransfer::transfer cost:" << endl;
    for (map<string,Length> ::iterator j = cost.begin() ; j != cost.end() ; ++j)
        Log::info() << "   " << (*j).first << " => " << Bytes((*j).second) << endl;

    Connector& c(Connector::service("mover", cost));

    Log::message() << c.host() << endl;
    Stream& s = c;

	s << bool(false); // New batch

    //NodeInfo::sendLogin(s);
    //NodeInfo remote = NodeInfo::acceptLogin(s);

    Log::status() << "Sending input handle " << from.title() << endl;
    from.toRemote(s);

    Log::status() << "Sending output handle " << to.title() << endl;
    to.toRemote(s);

    Length estimate = from.estimate();

    AutoState state('M');
    if(estimate)
        Log::status() << Bytes(estimate) << " " ;
    Log::status() << from.title() << " => " << to.title() << endl;

    Progress progress("mover", 0, estimate);
    unsigned long long total = 0;
    bool more;
    s >> more;
    while(more) 
    {
        long pos;
        string msg;
        s >> pos;
        s >> msg;
        total += pos;
        progress(total);
        s >> more;
    }

    unsigned long long len;
    s >> len;

//    ASSERT(len == total);

    Log::message() << " " << endl;


    return len;

}

//-----------------------------------------------------------------------------

} // namespace eckit

