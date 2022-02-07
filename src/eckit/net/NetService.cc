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
#include "eckit/config/Resource.h"
#include "eckit/log/Log.h"
#include "eckit/net/NetUser.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/runtime/ProcessControler.h"
#include "eckit/thread/ThreadControler.h"
#include "eckit/io/Select.h"

namespace eckit {
namespace net {


class NetServiceProcessControler : public ProcessControler {

public:
    NetServiceProcessControler(const std::string& name, NetUser* user, TCPServer& server, long parent, bool visible);

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


NetService::NetService(int port, bool visible, const SocketOptions& options) :
    server_(port, options), visible_(visible) {}

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

    std::ostringstream oss;
    oss << "Waiting on port " << port();

    while (!stopped()) {

        Log::status() << oss.str() << std::endl;

        if(timeout()) {
            Select select(server_);
            if(!select.ready(timeout())) {
                // This will allow to check stopped() again
                continue;
            }
        }

        NetUser* user = newUser(server_.accept(oss.str()));

        if (runAsProcess()) {
            NetServiceProcessControler t(name(),
                                         user,
                                         server_,
                                         Monitor::instance().self(),
                                         visible_);
            t.start();
        }
        else {
            ThreadControler t(user);
            t.start();
        }
    }
}

bool NetService::runAsProcess() const {
    return Resource<bool>(name() + "NetServiceForkProcess", preferToRunAsProcess());
}

bool NetService::preferToRunAsProcess() const {
    return false;
}

long NetService::timeout() const {
    return 0;
}

//----------------------------------------------------------------------------------------------------------------------

NetServiceProcessControler::NetServiceProcessControler(const std::string& name, NetUser* user, TCPServer& server,
                                                       long parent, bool visible) :
    ProcessControler(true), name_(name), user_(user), server_(server), parent_(parent), visible_(visible) {
    Log::info() << "NetServiceProcessControler::NetServiceProcessControler" << std::endl;
}

void NetServiceProcessControler::run() {

    eckit::Monitor::instance().reset();  // needed to the monitor to work on forked (but not execed process)
    Monitor::instance().parent(parent_);
    Monitor::instance().name(name_);
    // Monitor::instance().kind(name_);
    Monitor::instance().show(visible_);

    Log::info() << "NetServiceProcessControler::run start" << std::endl;

    try {
        user_->run();
    }
    catch (std::exception& e) {
        Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
        Log::error() << "** Exception is ignored" << std::endl;
    }

    Log::info() << "NetServiceProcessControler::run end" << std::endl;
}

void NetServiceProcessControler::afterForkInParent() {
    // This will close the connected socket
    Log::info() << "NetServiceProcessControler::run afterForkInParent" << std::endl;
    user_.reset(nullptr);
}

void NetServiceProcessControler::afterForkInChild() {
    // Close the accept() socket that is used in the parent
    Log::info() << "NetServiceProcessControler::run afterForkInChild" << std::endl;
    server_.close();
}

}  // namespace net
}  // namespace eckit
