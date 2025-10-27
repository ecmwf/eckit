/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   test_log_tcp.cc
/// @author Metin Cakircali
/// @date   Jun 2024

#include <array>
#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>
#include <string>
#include <thread>
#include <utility>

#include "eckit/io/SockBuf.h"
#include "eckit/log/Channel.h"
#include "eckit/log/Log.h"
#include "eckit/log/SysLog.h"
#include "eckit/log/SysLogTCPTarget.h"
#include "eckit/net/TCPClient.h"
#include "eckit/net/TCPServer.h"
#include "eckit/net/TCPStream.h"
#include "eckit/runtime/Main.h"
#include "eckit/testing/Test.h"

using namespace eckit;

//----------------------------------------------------------------------------------------------------------------------
// HELPERS

namespace {

const std::array<std::string, 24> test_messages = {"Hello, World!",
                                                   "Logging over TCP",
                                                   "This is a test message",
                                                   "Another log entry",
                                                   "Testing TCP logging",
                                                   "Log message number 6",
                                                   "Seventh message in the log",
                                                   "Eighth log entry",
                                                   "Ninth message for testing",
                                                   "Some more log messages",
                                                   "Yet another log message",
                                                   "Final log message for this test",
                                                   "End of test messages",
                                                   "Logging complete",
                                                   "All messages sent successfully",
                                                   "TCP logging test finished",
                                                   "No errors encountered",
                                                   "Test messages are consistent",
                                                   "This is a very long log message to test the TCP logging for long "
                                                   "messages, which should not cause any issues or truncation. ",
                                                   "TCP connection is stable",
                                                   "Server received all messages",
                                                   "Client sent all messages",
                                                   "Test completed without issues",
                                                   "Ready for next test"};

//----------------------------------------------------------------------------------------------------------------------

// a simple server that listens for incoming messages over TCP and checks them against expected messages
void listenLogs(int port) {
    net::TCPServer server(port);
    auto socket = server.accept();

    SockBuf buffer(socket);
    std::istream ins(&buffer);

    for (const auto& msg : test_messages) {
        std::string received;
        std::getline(ins, received);
        EXPECT_EQUAL(received, msg);
    }
}

void listenSysLogs(int port) {
    net::TCPServer server(port);
    auto socket = server.accept();

    SockBuf buffer(socket);
    std::istream ins(&buffer);

    for (const auto& msg : test_messages) {
        std::string received;
        std::getline(ins, received);
        // std::cout << "Received: " << received << std::endl;
        EXPECT(received.find(msg) != std::string::npos);
        EXPECT(received.find("software=\"test-eckit\"") != std::string::npos);
        EXPECT(received.find("swVersion=\"0.0.1\"") != std::string::npos);
    }
}

//----------------------------------------------------------------------------------------------------------------------

// send logs over TCP
void sendLogs(const int port) {
    auto socket = net::TCPClient().connect("localhost", port);
    Log::info().setTarget(new TCPTarget(socket));

    for (const auto& msg : test_messages) {
        Log::info() << msg << '\n';
    }

    // reset target to default
    Log::info().setTarget(Main::instance().createInfoLogTarget());
}

// send sysLogs over TCP
void sendSysLogs(const int port) {

    SysLog log("", 0, SysLog::User, SysLog::Info);
    log.enterpriseId("7464");  // ECMWF's id
    log.software("test-eckit");
    log.swVersion("0.0.1");

    auto socket = net::TCPClient().connect("localhost", port);
    Log::info().setTarget(new SysLogTCPTarget(socket, log));

    for (const auto& msg : test_messages) {
        Log::info() << msg << std::endl;
    }

    // reset target to default
    Log::info().setTarget(Main::instance().createInfoLogTarget());
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

namespace eckit::test {

CASE("Log: send logs over TCP") {
    constexpr int test_port = 7575;

    std::thread server(listenLogs, test_port);
    std::thread client(sendLogs, test_port);

    client.join();
    server.join();
}

CASE("Log: send syslogs over TCP") {
    constexpr int test_port = 7576;

    std::thread server(listenSysLogs, test_port);
    std::thread client(sendSysLogs, test_port);

    client.join();
    server.join();
}

}  // namespace eckit::test

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return testing::run_tests(argc, argv);
}
