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
#include <sstream>

#include "eckit/io/AutoCloser.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/io/TCPSocketHandle.h"
#include "eckit/log/Log.h"
#include "eckit/maths/Functions.h"
#include "eckit/net/NetService.h"
#include "eckit/net/NetUser.h"
#include "eckit/net/Port.h"
#include "eckit/net/TCPServer.h"
#include "eckit/net/TCPStream.h"
#include "eckit/runtime/Application.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/thread/Thread.h"
#include "eckit/thread/ThreadControler.h"


using namespace eckit;

//----------------------------------------------------------------------------------------------------------------------


class FDBUser : public eckit::net::NetUser {
public:
    FDBUser(eckit::net::TCPSocket& protocol) :
        net::NetUser(protocol) {}

    ~FDBUser() {}

    net::EphemeralTCPServer data_;

private:
    virtual void serve(eckit::Stream& control, std::istream&, std::ostream&) {

        try {

            std::string verb;
            for (;;) {

                // 2. recv a request (atm a 'verb' for action)
                control >> verb;

                Log::info() << "received verb " << verb << std::endl;

                if (verb == "archive") {

                    // 3.1 recv file size
                    Length fsize;
                    control >> fsize;

                    Log::info() << "received file size " << fsize << std::endl;

                    // 3.2 send data port
                    control << data_.localPort();

                    // 3.3 accept data connection
                    InstantTCPSocketHandle tcp(data_.accept("Waiting for data connection", 60));

                    Log::info() << "Accepted connection from " << data_.remoteHost() << ":" << data_.remotePort()
                                << std::endl;

                    MemoryHandle mh_(eckit::round(fsize, 4 * 1024));
                    Length total = tcp.saveInto(mh_);

                    ASSERT(fsize == total);

                    continue;
                }

                if (verb == "flush") {
                    Log::info() << "Flushing ... " << std::endl;
                    continue;
                }

                if (verb == "stop") {
                    Log::info() << "Stopping ... " << std::endl;
                    break;
                }
            }
        }
        catch (std::exception& e) {
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
    }
};

//----------------------------------------------------------------------------------------------------------------------

class FDBService : public eckit::net::NetService {
public:
    FDBService(int port) :
        net::NetService(port) {}

    ~FDBService() {}

private:
    virtual eckit::net::NetUser* newUser(eckit::net::TCPSocket& protocol) const { return new FDBUser(protocol); }

    virtual std::string name() const { return "fdbsvr"; }
};

//----------------------------------------------------------------------------------------------------------------------

class FDBSvrApp : public Application {
public:
    FDBSvrApp(int argc, char** argv) :
        Application(argc, argv, "HOME") {}

    ~FDBSvrApp() {}

private:  // methods
    FDBSvrApp(const FDBSvrApp&) = delete;
    FDBSvrApp& operator=(const FDBSvrApp&) = delete;

    virtual void run() {

        unique();

        eckit::ThreadControler tc(new FDBService(net::Port("fdbsvr", 9013)));
        tc.start();
        for (;;) {
            Log::status() << "Idle" << std::endl;
            ::sleep(60);
        }
        tc.stop();
    }
};

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    FDBSvrApp app(argc, argv);
    app.start();
    return 0;
}
