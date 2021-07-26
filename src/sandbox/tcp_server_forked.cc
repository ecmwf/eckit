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
#include "eckit/runtime/ProcessControler.h"


using namespace eckit;

//----------------------------------------------------------------------------------------------------------------------

class FDBForker : public ProcessControler {

    net::TCPSocket socket_;

    void run() {
        Log::info() << " *********** pid is " << getpid() << std::endl;

        Monitor::instance().reset();  // needed to het monitor to work -- needs eckit change

        Log::info() << " *********** monitor task id " << Monitor::instance().self() << std::endl;

        Log::info() << "Waiting for client " << socket_.remoteHost() << ":" << socket_.remotePort() << std::endl;

        Log::status() << "Waiting for client " << socket_.remoteHost() << ":" << socket_.remotePort() << std::endl;

        net::EphemeralTCPServer data;

        net::InstantTCPStream control(socket_);

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
                    control << data.localPort();

                    // 3.3 accept data connection
                    InstantTCPSocketHandle tcp(data.accept("Waiting for data connection", 60));

                    Log::info() << "Accepted connection from " << data.remoteHost() << ":" << data.remotePort()
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

public:
    FDBForker(net::TCPSocket& socket) :
        ProcessControler(true), socket_(socket) {}
};

//----------------------------------------------------------------------------------------------------------------------

class FDBSvrApp : public Application {
public:
    FDBSvrApp(int argc, char** argv) :
        Application(argc, argv, "HOME") {}

    ~FDBSvrApp() {}

private:
    int port_;

    FDBSvrApp(const FDBSvrApp&) = delete;
    FDBSvrApp& operator=(const FDBSvrApp&) = delete;

    virtual void run() {
        unique();

        net::TCPServer server(net::Port("fdbsvr", 9013));
        server.closeExec(false);

        port_ = server.localPort();

        for (;;) {
            try {
                FDBForker f(server.accept());
                f.start();
            }
            catch (std::exception& e) {
                Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
                Log::error() << "** Exception is ignored" << std::endl;
            }
        }
    }
};

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    FDBSvrApp app(argc, argv);
    app.start();
    return 0;
}
