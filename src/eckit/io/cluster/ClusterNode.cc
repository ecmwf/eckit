/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>

#include "eckit/config/Resource.h"
#include "eckit/io/cluster/ClusterDisks.h"
#include "eckit/io/cluster/ClusterNode.h"
#include "eckit/io/cluster/ClusterNodes.h"
#include "eckit/io/cluster/NodeInfo.h"
#include "eckit/net/Port.h"
#include "eckit/net/TCPClient.h"
#include "eckit/net/TCPStream.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/thread/Thread.h"
#include "eckit/thread/ThreadControler.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class ClusterHeartBeat : public Thread {
    int hostOffset_;
    ClusterNode& owner_;
    virtual void run();

public:
    ClusterHeartBeat(ClusterNode& owner, int hostOffset=0) :
        hostOffset_(hostOffset),
        owner_(owner) {}
};

void ClusterHeartBeat::run() {
    Monitor::instance().name("heartbeat");
    std::vector<std::string> hosts = Resource<std::vector<std::string>>("clusterHost", {"localhost"});
    int port         = net::Port("cluster", 9555);
    std::string reply;
    NodeInfo remote;

    // Which cluster host are we talking to in this thread
    ASSERT(hostOffset_ < hosts.size());
    std::string host = hosts[hostOffset_];

    // If there are other cluster hosts to consider, spawn heartbeat threads for them too
    if (hosts.size() > hostOffset_ + 1) {
        ThreadControler t(new ClusterHeartBeat(owner_, hostOffset_ + 1));
        t.start();
    }

    for (;;) {
        net::TCPClient client;

        try {
            Log::status() << "Connecting to " << host << ":" << port << std::endl;
            net::TCPStream s(client.connect(host, port));
            Log::status() << "Connected to " << host << ":" << port << std::endl;

            NodeInfo::thisNode().port(owner_.port());
            NodeInfo::thisNode().attributes(owner_.attributes());

            remote = NodeInfo::sendLogin(s);

            owner_.initialise(s);

            // s << owner_.port();

            char x[] = ".:";
            int n    = 0;

            for (;;) {
                Monitor::instance().state(x[n]);
                n = 1 - n;
                s << "heartbeat";
                s >> reply;

                if (reply == "sync") {
                    ClusterNodes::receive(s);
                    bool syncDisks;
                    s >> syncDisks;
                    if (syncDisks) {
                        ClusterDisks::receive(s);
                    }
                }

                if (reply == "exit") {
                    ::exit(0);
                }

                ::sleep(20);
                owner_.refresh(s);
            }
        }
        catch (std::exception& e) {
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is handled" << std::endl;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

ClusterNode::ClusterNode() {}

ClusterNode::~ClusterNode() {}


void ClusterNode::heartbeat() {
    ThreadControler t(new ClusterHeartBeat(*this));
    t.start();
}

// By default, there are no specified attributes
const std::set<std::string>& ClusterNode::attributes() const {
    static std::set<std::string> nullAttrs;
    return nullAttrs;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
