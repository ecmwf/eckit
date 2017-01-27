/*
 * (C) Copyright 1996-2017 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>

#include "eckit/io/cluster/ClusterNode.h"
#include "eckit/io/cluster/ClusterNodes.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/thread/Mutex.h"
#include "eckit/io/cluster/NodeInfo.h"
#include "eckit/net/Port.h"
#include "eckit/config/Resource.h"
#include "eckit/net/TCPClient.h"
#include "eckit/net/TCPStream.h"
#include "eckit/net/TCPStream.h"
#include "eckit/thread/Thread.h"
#include "eckit/thread/ThreadControler.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class ClusterHeartBeat : public Thread {
    ClusterNode& owner_;
    virtual void run();
public:
    ClusterHeartBeat(ClusterNode& owner):
        owner_(owner) {}
};

void ClusterHeartBeat::run()
{
    Monitor::instance().name("heartbeat");
    std::string host   = Resource<std::string>("clusterHost","localhost");
    int port = Port("cluster", 9555);
    std::string reply;
    NodeInfo remote;

	for(;;)
	{
        TCPClient client;

        try {
            Log::status() <<"Connecting to " << host << ":" << port << std::endl;
            TCPStream s(client.connect(host, port));
            Log::status() <<"Connected to " << host << ":" << port << std::endl;

            NodeInfo::thisNode().port(owner_.port());

            remote = NodeInfo::sendLogin(s);

            owner_.initialise(s);

            //s << owner_.port();

            char x[] = ".:";
            int n = 0;

            for(;;) {
                Monitor::instance().state(x[n]); n = 1 - n;
                s << "heartbeat";
                s >> reply;



                if(reply == "sync") {
                    ClusterNodes::receive(s);
                }

                if(reply == "exit") {
                    ::exit(0);
                }

                ::sleep(20);
                owner_.refresh(s);
            }
        }
        catch(std::exception& e)
        {
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is handled" << std::endl;
        }
    }
}



ClusterNode::ClusterNode()
{
}

ClusterNode::~ClusterNode()
{
}


void ClusterNode::heartbeat()
{
    ThreadControler t(new ClusterHeartBeat(*this));
    t.start();
}

//-----------------------------------------------------------------------------

} // namespace eckit

