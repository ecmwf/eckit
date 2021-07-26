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

#include "eckit/io/MemoryHandle.h"
#include "eckit/io/TCPSocketHandle.h"
#include "eckit/log/Log.h"
#include "eckit/maths/Functions.h"
#include "eckit/net/Port.h"
#include "eckit/net/TCPServer.h"
#include "eckit/net/TCPStream.h"
#include "eckit/runtime/Application.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/thread/Thread.h"
#include "eckit/thread/ThreadControler.h"


using namespace eckit;


class FDBServer;

class FDBConnection : public Thread {

    net::TCPSocket socket_;

    virtual void run();

public:
    FDBConnection(net::TCPSocket& socket) :
        socket_(socket) {}
};

void FDBConnection::run() {
    Monitor::instance().name("connection");
    Monitor::instance().show(true);

    try {

        Log::status() << "Receiving connection from " << socket_.remoteHost() << ":" << socket_.remotePort()
                      << std::endl;

        net::TCPStream control(socket_);

        for (;;) {

            // 2. recv a request (atm a 'verb' for action)
            std::string verb;
            control >> verb;

            Log::info() << "received verb " << verb << std::endl;

            if (verb == "archive") {

                // 3.1 recv file size
                Length fsize;
                control >> fsize;

                Log::info() << "received file size " << fsize << std::endl;

                // 3.2 send data port
                net::EphemeralTCPServer data_;
                control << data_.localPort();

                // 3.2 accept data connection
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

//----------------------------------------------------------------------------------------------------------------------


class FDBServer : public Application {

    virtual void run();

public:
    FDBServer(int argc, char** argv) :
        Application(argc, argv, "HOME") {}

    virtual ~FDBServer() {}
};

void FDBServer::run() {
    unique();  // there can be only one

    Log::status() << "Starting server" << std::endl;

    net::TCPServer server(net::Port("fdb", 9013));

    for (;;) {
        Log::status() << "-" << std::endl;
        try {
            ThreadControler ct(new FDBConnection(server.accept()));
            ct.start();
        }
        catch (std::exception& e) {
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    FDBServer app(argc, argv);
    app.start();
    return 0;
}
