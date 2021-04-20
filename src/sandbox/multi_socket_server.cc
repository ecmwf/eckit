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

#include "eckit/filesystem/PathName.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/io/TCPHandle.h"
#include "eckit/net/MultiSocket.h"
#include "eckit/net/TCPServer.h"
#include "eckit/net/TCPStream.h"
#include "eckit/runtime/Application.h"
#include "eckit/runtime/Tool.h"


using namespace eckit;

class Server : public Application {

    virtual void run();

public:
    Server(int argc, char** argv) : Application(argc, argv, "HOME") {}
};


void Server::run() {


    net::MultiSocket server(9013);
    char buffer[4096 * 26];

    for (int i = 0; i < 2; ++i) {
        net::MultiSocket s(server.accept());

        ASSERT(s.read(buffer, sizeof(buffer)) == sizeof(buffer));
        ASSERT(s.write(buffer, sizeof(buffer)) == sizeof(buffer));
    }

}


int main(int argc, char** argv) {
    Server app(argc, argv);
    app.start();
}
