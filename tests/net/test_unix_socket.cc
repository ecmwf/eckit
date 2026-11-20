// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cstring>

#include <functional>
#include <future>
#include <memory>
#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/net/TCPClient.h"
#include "eckit/net/TCPServer.h"
#include "eckit/net/UnixSocketClient.h"
#include "eckit/net/UnixSocketServer.h"
#include "eckit/testing/Test.h"

using namespace eckit;
using namespace eckit::net;

namespace {

//----------------------------------------------------------------------------------------------------------------------

/// Relative to the working directory, as sun_path holds only about 100 characters
LocalPathName socketPath() {
    return LocalPathName("eckit_test_unix_socket." + std::to_string(::getpid()) + ".sock");
}

/// Echo a message through a listener of any transport, then check that closing the output is seen as end-of-file
void echo(Socket& listener, const std::function<std::unique_ptr<Socket>()>& connect) {
    auto server = std::async(std::launch::async, [&listener] {
        std::unique_ptr<Socket> socket = listener.acceptSocket();
        char buf[5];
        EXPECT_EQUAL(socket->read(buf, 5), 5);
        EXPECT_EQUAL(socket->write(buf, 5), 5);
        EXPECT_EQUAL(socket->read(buf, 5), 0);
    });

    std::unique_ptr<Socket> client = connect();
    EXPECT_EQUAL(client->write("hello", 5), 5);

    char buf[5];
    EXPECT_EQUAL(client->read(buf, 5), 5);
    EXPECT_EQUAL(std::string(buf, 5), "hello");

    client->closeOutput();
    server.get();
}

//----------------------------------------------------------------------------------------------------------------------

CASE("A TCP listener serves connections through the Socket interface") {
    EphemeralTCPServer server;
    int port = server.localPort();

    echo(server, [port] {
        auto client = std::make_unique<TCPClient>();
        client->connect("localhost", port);
        return client;
    });
}

CASE("A unix listener serves connections through the Socket interface") {
    UnixSocketServer server(socketPath());

    echo(server, [] {
        auto client = std::make_unique<UnixSocketClient>();
        client->connect(socketPath());
        return client;
    });
}

CASE("Connecting fails at once when no server is listening") {
    UnixSocketClient client;
    EXPECT_THROWS_AS(client.connect(socketPath()), FailedSystemCall);
    EXPECT(!client.isConnected());
}

CASE("A server refuses a path that exists, and removes its own path on destruction") {
    LocalPathName path = socketPath();
    {
        UnixSocketServer server(path);
        EXPECT(path.exists());
        EXPECT_THROWS_AS(UnixSocketServer{path}, FailedSystemCall);
        EXPECT(path.exists());
    }
    EXPECT(!path.exists());
}

CASE("A stale socket left at the path is replaced") {
    LocalPathName path = socketPath();

    // Bound, but with no server and no lock held: as if a server had been killed
    sockaddr_un addr;
    ::memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    ::strncpy(addr.sun_path, path.localPath(), sizeof(addr.sun_path) - 1);
    int s = ::socket(AF_UNIX, SOCK_STREAM, 0);
    EXPECT(::bind(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == 0);
    ::close(s);
    EXPECT(path.exists());

    UnixSocketServer server(path);
    echo(server, [path] {
        auto client = std::make_unique<UnixSocketClient>();
        client->connect(path);
        return client;
    });
}

CASE("A path that is not a socket is refused, and left alone") {
    LocalPathName path = socketPath();
    ::close(::open(path.localPath(), O_CREAT | O_WRONLY, 0644));

    EXPECT_THROWS_AS(UnixSocketServer{path}, FailedSystemCall);
    EXPECT(path.exists());
    ::unlink(path.localPath());
}

CASE("A connected client stops a new server from starting, even after its server has gone") {
    LocalPathName path = socketPath();
    UnixSocketClient client;
    {
        UnixSocketServer server(path);
        client.connect(path);
    }
    EXPECT(!path.exists());

    EXPECT_THROWS_AS(UnixSocketServer{path}, FailedSystemCall);

    client.close();
    UnixSocketServer server(path);
    EXPECT(path.exists());
}

CASE("A path that does not fit in a sockaddr_un is refused") {
    EXPECT_THROWS_AS(UnixSocketServer{LocalPathName(std::string(200, 'x'))}, UserError);
}

CASE("Accepting with a timeout throws TimeOut when nobody connects") {
    UnixSocketServer server(socketPath());
    EXPECT_THROWS_AS(server.acceptSocket("Waiting for connection", 1), TimeOut);
}

CASE("Sockets that do not listen cannot accept") {
    UnixSocket unixSocket;
    EXPECT_THROWS_AS(unixSocket.acceptSocket(), NotImplemented);

    TCPSocket tcpSocket;
    EXPECT_THROWS_AS(tcpSocket.acceptSocket(), NotImplemented);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace

int main(int argc, char** argv) {
    int result = eckit::testing::run_tests(argc, argv);
    ::unlink((socketPath() + ".lock").localPath());  // Servers never remove their lock files
    return result;
}
