/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <string.h>
#include <unistd.h>
#include <fstream>
#include <sstream>

#include "eckit/config/Resource.h"
#include "eckit/io/MultiSocketHandle.h"
#include "eckit/io/PartFileHandle.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Timer.h"
#include "eckit/net/MultiSocket.h"
#include "eckit/runtime/Application.h"


using namespace eckit;

class Client : public Application {

    virtual void run();
    void test(const std::string& host, int port);
    void send(const std::string& host, int port);
    void grid(const std::string& host, int port);

public:
    Client(int argc, char** argv) :
        Application(argc, argv, "HOME") {}
};

void Client::test(const std::string& host, int port) {

    for (int i = 10; i <= 20; i += 10) {

        net::MultiSocket client(i, 4096);
        net::MultiSocket s(client.connect(host, port));

        const char p[] = "abcdefghijklmnopqrstuvwxyz";
        for (size_t i = 0; i < 4096; ++i) {
            ASSERT(s.write(p, 26) == 26);
        }

        char q[26];
        for (size_t i = 0; i < 4096; ++i) {
            ASSERT(s.read(q, 26) == 26);
            ASSERT(::memcmp(p, q, 26) == 0);
        }
    }
}


void Client::send(const std::string& host, int port) {
    int streams     = Resource<int>("--streams", 10);
    int messageSize = Resource<int>("--message-size", 64 * 1024);
    long long size  = Resource<long long>("--size", 1024 * 1024 * 1024);
    PathName file   = Resource<PathName>("--file", "/dev/zero");

    if (file.size() > 0) {
        size = std::min(Length(size), file.size());
    }
    Log::info() << "Sending " << Bytes(size) << " from " << file << std::endl;

    PartFileHandle in(file, 0, size);
    MultiSocketHandle out(host, port, streams, messageSize);
    in.saveInto(out);
}

void Client::grid(const std::string& host, int port) {
    int streams_start     = Resource<int>("--streams-start", 1);
    int streams_end       = Resource<int>("--streams-end", 20);
    int streams_step      = Resource<int>("--streams-step", 1);
    int messageSize_start = Resource<int>("--message-size-start", 1024);
    int messageSize_end   = Resource<int>("--message-size-end", 1024 * 1024);
    int messageSize_step  = Resource<int>("--message-size-step", 2);
    int bufferSize        = Resource<int>("--buffer-size", 0);
    long long size        = Resource<long long>("--size", 1024 * 1024 * 1024);
    PathName file         = Resource<PathName>("--file", "/dev/zero");

    std::string csv_file = Resource<std::string>("--csv", "sandbox_multi_socket_client.csv");

    std::ofstream csv(csv_file);
    csv << "size"
        << ","
        << "streams"
        << ","
        << "messageSize"
        << ","
        << "elapsed" << std::endl;

    if (file.size() > 0) {
        size = std::min(Length(size), file.size());
    }

    for (int streams = streams_start; streams <= streams_end; streams += streams_step) {
        int messageSize = messageSize_start;
        while (messageSize <= messageSize_end) {
            Log::info() << "Sending " << Bytes(size) << " from " << file << std::endl;

            PartFileHandle in(file, 0, size);
            MultiSocketHandle out(host, port, streams, messageSize, bufferSize);

            Timer timer;
            in.saveInto(out);

            csv << size << "," << streams << "," << messageSize << "," << timer.elapsed() << std::endl;

            if (messageSize_step < 1024) {
                messageSize *= messageSize_step;
            }
            else {
                messageSize += messageSize_step;
            }
        }
    }
}

void Client::run() {

    std::string host = Resource<std::string>("--host", "localhost");
    int port         = Resource<int>("--port", 9013);
    bool test        = Resource<bool>("--test", false);
    bool grid        = Resource<bool>("--grid", false);

    if (test) {
        Client::test(host, port);
        return;
    }

    if (grid) {
        Client::grid(host, port);
        return;
    }

    Client::send(host, port);
}


int main(int argc, char** argv) {
    Client app(argc, argv);
    app.start();
}
