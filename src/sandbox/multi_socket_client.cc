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
#include "eckit/net/TCPClient.h"
#include "eckit/net/TCPStream.h"
#include "eckit/runtime/Application.h"
#include "eckit/runtime/Tool.h"


using namespace eckit;

class Client : public Application {

    virtual void run();

public:
    Client(int argc, char** argv) : Application(argc, argv, "HOME") {}
};


void Client::run() {
    std::string remoteHost = "localhost";


    for (int i = 10; i <= 20; i += 10) {

        net::MultiSocket client(i, 4096);
        net::MultiSocket s(client.connect(remoteHost, 9013));

        const char p[] = "abcdefghijklmnopqrstuvwxyz";
        for (size_t i = 0; i < 4096; ++i) {
            ASSERT(s.write(p, 26) == 26);
        }

        char q[26] ;
        for (size_t i = 0; i < 4096; ++i) {
            ASSERT(s.read(q, 26) == 26);
            ASSERT(::memcmp(p, q, 26)==0);
        }
    }
}


int main(int argc, char** argv) {
    Client app(argc, argv);
    app.start();
}
