// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/net/NetService.h"

#include "eckit/config/LibEcKit.h"
#include "eckit/config/Resource.h"
#include "eckit/io/Select.h"
#include "eckit/log/Log.h"
#include "eckit/net/NetUser.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/runtime/ProcessControler.h"
#include "eckit/thread/ConcurrencyLimiter.h"
#include "eckit/thread/ThreadControler.h"

#include <sys/socket.h>
#include <sys/time.h>

#include <memory>


namespace eckit::net {

//----------------------------------------------------------------------------------------------------------------------

/// Seconds a refused client has to send its request, before the refusal gives up on it
static constexpr int refusalReadTimeoutUSec = 100;

//----------------------------------------------------------------------------------------------------------------------

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

//----------------------------------------------------------------------------------------------------------------------

// Serves a connection in this process, and releases its slot when done

class NetServiceWorker : public Thread {
public:

    NetServiceWorker(NetUser* user, ConcurrencyLimiter& limiter) : user_(user), limiter_(limiter) {}

private:

    std::unique_ptr<NetUser> user_;
    ConcurrencyLimiter& limiter_;

    void run() override {
        AutoConcurrencySlot slot(limiter_);
        user_->run();
    }
};

//----------------------------------------------------------------------------------------------------------------------

// Waits for the child process serving a connection to terminate, and releases its slot

class NetServiceChildWaiter : public Thread {
public:

    NetServiceChildWaiter(std::unique_ptr<NetServiceProcessControler> child, ConcurrencyLimiter& limiter, bool visible) :
        child_(std::move(child)), limiter_(limiter), visible_(visible) {}

private:

    std::unique_ptr<NetServiceProcessControler> child_;
    ConcurrencyLimiter& limiter_;
    bool visible_;

    void run() override {
        Monitor::instance().show(visible_);
        Log::status() << "Waiting for child process" << std::endl;
        AutoConcurrencySlot slot(limiter_);
        child_->wait();
    }
};

//----------------------------------------------------------------------------------------------------------------------

NetService::NetService(int port, bool visible, const SocketOptions& options) :
    server_(port, options), visible_(visible) {}

NetService::NetService(int port, ConcurrencyLimiter& limiter, bool visible, const SocketOptions& options) :
    server_(port, options), visible_(visible), limiter_(limiter) {}

NetService::~NetService() = default;

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

        if (timeout()) {
            Select select(server_);
            if (!select.ready(timeout())) {
                // This will allow to check stopped() again
                continue;
            }
        }

        NetUser* user = newUser(server_.accept(oss.str()));

        if (limiter_) {
            serveLimited(user, limiter_->get());
        }
        else if (runAsProcess()) {
            NetServiceProcessControler t(name(), user, server_, Monitor::instance().self(), visible_);
            t.start();
        }
        else {
            ThreadControler t(user);
            t.start();
        }
    }
}

void NetService::serveLimited(NetUser* u, ConcurrencyLimiter& limiter) {

    std::unique_ptr<NetUser> user(u);

    std::string reason;
    if (!limiter.tryAcquire(reason)) {

        Log::warning() << "Refusing connection from " << user->protocol_.remoteHost() << ": " << reason << std::endl;

        // When refusing, we typically want to consume the input, and send back a nice response. But set a nice
        // snappy timeout, so that if the connection is misbehaving we don't block the server.

        timeval tv{0, refusalReadTimeoutUSec};
        ::setsockopt(user->protocol_.socket(), SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        try {
            user->reject(reason);
        }
        catch (std::exception& e) {
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
        return;
    }

    try {
        if (runAsProcess()) {
            // Fork in this thread, which closes the parent's copy of the connection before the next accept()
            std::unique_ptr<NetServiceProcessControler> child(new NetServiceProcessControler(
                name(), user.release(), server_, Monitor::instance().self(), visible_));
            child->start();
            ThreadControler t(new NetServiceChildWaiter(std::move(child), limiter, visible_));
            t.start();
        }
        else {
            ThreadControler t(new NetServiceWorker(user.release(), limiter));
            t.start();
        }
    }
    catch (std::exception& e) {
        limiter.release();
        Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
        Log::error() << "** Exception is ignored" << std::endl;
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
    ProcessControler(true), name_(name), user_(user), server_(server), parent_(parent), visible_(visible) {}

void NetServiceProcessControler::run() {

    eckit::Monitor::instance().reset();  // needed to the monitor to work on forked (but not execed process)
    Monitor::instance().parent(parent_);
    Monitor::instance().name(name_);
    // Monitor::instance().kind(name_);
    Monitor::instance().show(visible_);

    LOG_DEBUG_LIB(LibEcKit) << "NetServiceProcessControler::run start" << std::endl;

    try {
        user_->run();
    }
    catch (std::exception& e) {
        Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
        Log::error() << "** Exception is ignored" << std::endl;
    }

    LOG_DEBUG_LIB(LibEcKit) << "NetServiceProcessControler::run end" << std::endl;
}

void NetServiceProcessControler::afterForkInParent() {
    // This will close the connected socket
    LOG_DEBUG_LIB(LibEcKit) << "NetServiceProcessControler::run afterForkInParent" << std::endl;
    user_.reset(nullptr);
}

void NetServiceProcessControler::afterForkInChild() {
    // Close the accept() socket that is used in the parent
    LOG_DEBUG_LIB(LibEcKit) << "NetServiceProcessControler::run afterForkInChild" << std::endl;
    server_.close();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::net
