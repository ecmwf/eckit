// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <unistd.h>

#include <sstream>

#include "eckit/config/Resource.h"
#include "eckit/io/Buffer.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Timer.h"
#include "eckit/net/MultiSocket.h"
#include "eckit/runtime/Application.h"

using namespace eckit;

class Server : public Application {

    virtual void run();
    void test(int port);

public:

    Server(int argc, char** argv) : Application(argc, argv, "HOME") {}
};

void Server::test(int port) {
    net::MultiSocket server(port);
    char buffer[4096 * 26];

    for (int i = 0; i < 2; ++i) {
        net::MultiSocket s(server.accept());

        ASSERT(s.read(buffer, sizeof(buffer)) == sizeof(buffer));
        ASSERT(s.write(buffer, sizeof(buffer)) == sizeof(buffer));
    }
}

void Server::run() {

    int port  = Resource<int>("--port", 9013);
    bool test = Resource<bool>("--test", false);


    if (test) {
        Server::test(port);
        return;
    }

    int readBuffer = Resource<int>("--readBuffer", 64 * 1024);
    net::MultiSocket server(port);
    Buffer buffer(readBuffer);

    for (;;) {
        Log::info() << "Ready" << std::endl;
        try {
            net::MultiSocket s(server.accept());

            Timer timer("Receive");
            long long total = 0;
            size_t len;
            while ((len = s.read(buffer, readBuffer)) > 0) {
                total += len;
            }
            Log::info() << "Received " << Bytes(total) << " at " << Bytes(total, timer) << std::endl;
        }
        catch (std::exception& e) {
            Log::error() << e.what() << std::endl;
        }
    }
}


int main(int argc, char** argv) {
    Server app(argc, argv);
    app.start();
}
