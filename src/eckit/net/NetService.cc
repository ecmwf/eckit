/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/net/NetService.h"
#include "eckit/log/Log.h"
#include "eckit/net/NetUser.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/thread/ThreadControler.h"
#include "eckit/config/Resource.h"
#include "eckit/runtime/ProcessControler.h"

namespace eckit {
namespace net {


class NetServiceProcessControler : public ProcessControler {

public:
    NetServiceProcessControler(const std::string& name,
                               NetUser *user,
                               TCPServer& server,
                               long parent,
                               bool visible);

private:

    std::string name_;
    std::unique_ptr<NetUser> user_;
    TCPServer& server_;
    long parent_;

    bool visible_;

    virtual void run();
    virtual void afterForkInParent();
    virtual void afterForkInChild();
};


NetService::NetService(int port, bool visible, const SocketOptions& options):
    server_(port, options),
    visible_(visible) {}

NetService::~NetService() {}

std::string NetService::hostname() const {
    return server_.localHost();
}

int NetService::port() const {
    return server_.localPort();
}

void NetService::run() {
    Monitor::instance().show(visible_);
    Monitor::instance().name(name());
    Monitor::instance().kind(name());

    Log::status() << "Waiting on port " << port() << std::endl;

    while (!stopped()) {

        if (process()) {
            NetServiceProcessControler t(name(), newUser(server_.accept()), server_, Monitor::instance().self(), visible_);
            t.start();
        }
        else {
            ThreadControler t(newUser(server_.accept()));
            t.start();
        }
    }
}

bool NetService::process() const {
    return Resource<bool>(name() + "NetServiceForkProcess", false);
}


//------------------------------------------------------------------------

NetServiceProcessControler::NetServiceProcessControler(const std::string& name,
        NetUser *user,
        TCPServer& server,
        long parent,
        bool visible):
    ProcessControler(true),
    name_(name),
    user_(user),
    server_(server),
    parent_(parent),
    visible_(visible) {

}

void NetServiceProcessControler::run() {
    eckit::Monitor::instance().reset(); // needed to the monitor to work on forked (but not execed process)
    Monitor::instance().parent(parent_);
    Monitor::instance().name(name_);
    // Monitor::instance().kind(name_);
    Monitor::instance().show(visible_);

    try {
        user_->run();
    }
    catch (std::exception& e) {
        Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
        Log::error() << "** Exception is ignored" << std::endl;
    }
}

void NetServiceProcessControler::afterForkInParent() {
    // This will close the connected socket
    user_.reset(0);
}

void NetServiceProcessControler::afterForkInChild() {
    // Close the accept() socket that is used in the parent
    server_.close();
}

} // namespace net
} // namespace eckit
