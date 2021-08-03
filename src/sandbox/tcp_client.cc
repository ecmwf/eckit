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
#include "eckit/net/TCPClient.h"
#include "eckit/net/TCPStream.h"
#include "eckit/runtime/Application.h"
#include "eckit/runtime/Tool.h"


using namespace eckit;

class Client : public Application {

    virtual void run();

public:
    Client(int argc, char** argv) :
        Application(argc, argv, "HOME") {}
};


void Client::run() {
    std::string remoteHost = "localhost";


    net::TCPClient client;
    net::TCPStream s(client.connect(remoteHost, 9013));

    Log::info() << "Connecting to " << s.socket().remoteHost() << ":" << s.socket().remotePort() << std::endl;

    for (int loop = 0; loop < 100; loop++) {

        for (const char* data : {"AAAAAAA", "BBBBBBBB", "CCCCCCCCCCCCC"}) {

            MemoryHandle mh(data, strlen(data));

            // 2. send verb
            std::string verb("archive");
            s << verb;
            Log::info() << "sent verb " << verb << std::endl;

            // 3.1 send file size
            s << Length(mh.size());
            Log::info() << "sent file size " << mh.size() << std::endl;

            // 3.2 recv data port
            int dataPort;
            s >> dataPort;

            // 3.2 connect to data server

            Log::info() << "Connecting to " << remoteHost << ":" << dataPort << std::endl;
            TCPHandle tcpdata(remoteHost, dataPort);

            mh.saveInto(tcpdata);
        }

        s << "flush";
        Log::info() << "sent verb flush" << std::endl;
    }

    s << "stop";
    Log::info() << "sent verb stop" << std::endl;
}


int main(int argc, char** argv) {
    Client app(argc, argv);
    app.start();
}
