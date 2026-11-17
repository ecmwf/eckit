// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <sys/socket.h>

#include <chrono>
#include <climits>
#include <thread>

#include "eckit/net/TCPSocket.h"
#include "eckit/testing/Test.h"

using namespace eckit;
using namespace eckit::net;
using namespace eckit::testing;
using namespace std::chrono_literals;

namespace {

// Exercise the socket input primitive without fixed ports or a running service.
class TestSocket : public TCPSocket {
public:

    explicit TestSocket(int fd) { socket_ = fd; }
};

struct SocketPair {
    int fds[2];
    SocketPair() { SYSCALL(::socketpair(AF_UNIX, SOCK_STREAM, 0, fds)); }
};

using Status = TCPSocket::InputStatus;

CASE("waitForInput times out on an idle connection") {
    SocketPair pair;
    TestSocket reader(pair.fds[0]);
    TestSocket writer(pair.fds[1]);
    EXPECT(reader.waitForInput(0ms) == Status::Timeout);
    const auto start = std::chrono::steady_clock::now();
    EXPECT(reader.waitForInput(20ms) == Status::Timeout);
    EXPECT(std::chrono::steady_clock::now() - start >= 20ms);
}

CASE("waitForInput detects a clean disconnect before any data") {
    SocketPair pair;
    TestSocket reader(pair.fds[0]);
    TestSocket writer(pair.fds[1]);
    writer.close();
    EXPECT(reader.waitForInput(100ms) == Status::Closed);
}

CASE("waitForInput leaves data available to the reader") {
    SocketPair pair;
    TestSocket reader(pair.fds[0]);
    TestSocket writer(pair.fds[1]);
    EXPECT(writer.write("x", 1) == 1);
    EXPECT(reader.waitForInput(0ms) == Status::Ready);
    EXPECT(reader.waitForInput(0ms) == Status::Ready);
    char byte = 0;
    EXPECT(reader.read(&byte, 1) == 1);
    EXPECT(byte == 'x');
    EXPECT(reader.waitForInput(0ms) == Status::Timeout);
}

CASE("waitForInput reports buffered data before a disconnect") {
    SocketPair pair;
    TestSocket reader(pair.fds[0]);
    TestSocket writer(pair.fds[1]);
    EXPECT(writer.write("x", 1) == 1);
    writer.close();
    EXPECT(reader.waitForInput(100ms) == Status::Ready);
    char byte = 0;
    EXPECT(reader.read(&byte, 1) == 1);
    EXPECT(byte == 'x');
    EXPECT(reader.waitForInput(100ms) == Status::Closed);
}

CASE("waitForInput waits for arriving data") {
    SocketPair pair;
    TestSocket reader(pair.fds[0]);
    TestSocket writer(pair.fds[1]);
    std::thread producer([&writer] {
        std::this_thread::sleep_for(20ms);
        writer.write("x", 1);
    });
    // Something is very wrong on our hardware if this takes 5+s.
    const auto status = reader.waitForInput(5000ms);
    producer.join();
    EXPECT(status == Status::Ready);
}

CASE("waitForInput rejects invalid sockets and out-of-range timeouts") {
    TCPSocket socket;
    EXPECT_THROWS_AS(socket.waitForInput(0ms), FailedSystemCall);
    EXPECT_THROWS_AS(socket.waitForInput(-1ms), AssertionFailed);
    EXPECT_THROWS_AS(socket.waitForInput(std::chrono::milliseconds(INT_MAX)), AssertionFailed);
    EXPECT_THROWS_AS(socket.waitForInput(std::chrono::milliseconds(INT_MAX) + 1ms), AssertionFailed);
}

}  // namespace

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
