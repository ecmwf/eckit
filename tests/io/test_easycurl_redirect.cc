// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <array>
#include <chrono>
#include <exception>
#include <string>
#include <thread>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/EasyCURL.h"
#include "eckit/net/SocketOptions.h"
#include "eckit/net/TCPServer.h"
#include "eckit/testing/Test.h"


namespace eckit::tests::io {


namespace {


using milliseconds = std::chrono::milliseconds;


/// Replies once with a redirect (its body optionally later than its headers), then once with the redirect's target
class RedirectingServer {
public:

    explicit RedirectingServer(milliseconds body_delay) :
        // An ephemeral port, as tests running at the same time (ctest -j, or builds on the same host) could collide on
        // a fixed one: a failed bind is retried without limit, and clients can reach another test's server. If a fixed
        // port is ever needed (e.g. a firewall rule), with server_ a net::TCPServer:
        // server_(7577, net::SocketOptions::server().bindAddress("127.0.0.1")),
        server_(net::SocketOptions::data().bindAddress("127.0.0.1")),
        port_(server_.localPort()),  // also listens, so connections wait for accept
        thread_([this, body_delay] { run(body_delay); }) {}

    RedirectingServer(const RedirectingServer&)            = delete;
    RedirectingServer& operator=(const RedirectingServer&) = delete;

    ~RedirectingServer() { join(); }

    std::string url() const { return "http://127.0.0.1:" + std::to_string(port_) + "/source"; }

    /// Request lines received, once the server has finished
    const std::vector<std::string>& requests() {
        join();
        if (error_) {
            std::rethrow_exception(error_);
        }
        return requests_;
    }

private:

    static constexpr int ACCEPT_TIMEOUT_S = 5;  // bounds the wait if the client never follows the redirect

    net::EphemeralTCPServer server_;
    int port_;
    std::vector<std::string> requests_;
    std::exception_ptr error_;
    std::thread thread_;  // last, as it uses the other members

    void join() {
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    static std::string read_request_line(const net::TCPSocket& socket) {
        std::string request;
        while (request.size() < 4 || request.compare(request.size() - 4, 4, "\r\n\r\n") != 0) {
            char c = 0;
            if (socket.read(&c, 1) != 1) {
                throw SeriousBug("RedirectingServer: incomplete request");
            }
            request += c;
        }
        return request.substr(0, request.find("\r\n"));
    }

    static void send(const net::TCPSocket& socket, const std::string& data) {
        if (socket.write(data.data(), static_cast<long>(data.size())) != static_cast<long>(data.size())) {
            throw SeriousBug("RedirectingServer: incomplete response");
        }
    }

    void run(milliseconds body_delay) {
        const std::string headers =
            "HTTP/1.1 302 Found\r\nLocation: /target\r\nContent-Length: 5\r\nConnection: close\r\n\r\n";
        const std::string body = "moved";

        try {
            {
                net::TCPSocket socket(server_.accept("Waiting for request", ACCEPT_TIMEOUT_S));
                requests_.push_back(read_request_line(socket));

                if (body_delay.count() > 0) {
                    send(socket, headers);
                    std::this_thread::sleep_for(body_delay);
                    send(socket, body);
                }
                else {
                    send(socket, headers + body);
                }
            }

            net::TCPSocket socket(server_.accept("Waiting for redirected request", ACCEPT_TIMEOUT_S));
            requests_.push_back(read_request_line(socket));
            send(socket, "HTTP/1.1 200 OK\r\nContent-Length: 3\r\nConnection: close\r\n\r\nHi!");
        }
        catch (...) {
            error_ = std::current_exception();
        }
    }
};


void expect_redirect_followed(bool stream, milliseconds body_delay) {
    RedirectingServer server(body_delay);

    EasyCURL curl;
    curl.followLocation(false);  // redirects followed by EasyCURL itself
    curl.setopt("CURLOPT_NOPROXY", "*");

    auto response = curl.GET(server.url(), stream);
    EXPECT(response.code() == 200);

    if (stream) {
        std::array<char, 3> buffer{};
        EXPECT(response.read(buffer.data(), buffer.size()) == buffer.size());
        EXPECT(std::string(buffer.data(), buffer.size()) == "Hi!");
    }
    else {
        EXPECT(response.body() == "Hi!");
    }

    const std::vector<std::string> expected{"GET /source HTTP/1.1", "GET /target HTTP/1.1"};
    EXPECT(server.requests() == expected);
}


}  // namespace


CASE("streamed GET follows a redirect, with its body later than its headers") {
    expect_redirect_followed(true, milliseconds(200));
}


CASE("streamed GET follows a redirect, with its body and headers together") {
    expect_redirect_followed(true, milliseconds(0));
}


CASE("GET follows a redirect, with its body later than its headers") {
    expect_redirect_followed(false, milliseconds(200));
}


}  // namespace eckit::tests::io


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
