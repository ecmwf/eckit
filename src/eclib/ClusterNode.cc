/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eclib/ClusterNode.h"
#include "eclib/ClusterNodes.h"
#include "eclib/Monitor.h"
#include "eclib/Mutex.h"
#include "eclib/NodeInfo.h"
#include "eclib/Port.h"
#include "eclib/Resource.h"
#include "eclib/TCPClient.h"
#include "eclib/TCPStream.h"
#include "eclib/TCPStream.h"
#include "eclib/thread/Thread.h"
#include "eclib/thread/ThreadControler.h"

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
    static const char *here = __FUNCTION__;

    Monitor::name("heartbeat");
    string host   = Resource<string>("clusterHost","localhost");
    int port =  Port("cluster",9555);
    string reply;
    NodeInfo remote;

	for(;;)
	{
        TCPClient client;

        try {
            Log::status() <<"Connecting to " << host << ":" << port << endl;
            TCPStream s(client.connect(host, port));
            Log::status() <<"Connected to " << host << ":" << port << endl;

            NodeInfo::thisNode().port(owner_.port());

            remote = NodeInfo::sendLogin(s);

            owner_.initialise(s);

            //s << owner_.port();

            char x[] = ".:";
            int n = 0;

            for(;;) {
                Monitor::state(x[n]); n = 1 - n;
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
        catch(exception& e)
        {
            Log::error() << "** " << e.what() << " Caught in " << here <<  endl;
            Log::error() << "** Exception is handled" << endl;
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

