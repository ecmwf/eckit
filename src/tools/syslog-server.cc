/*
** syslog-server.c -- a datagram sockets "server" demo
*/

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "eckit/eckit.h"

#include "eckit/config/Resource.h"
#include "eckit/io/Buffer.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Log.h"
#include "eckit/net/Port.h"
#include "eckit/net/UDPServer.h"
#include "eckit/runtime/Application.h"
#include "eckit/utils/Translator.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

class SysLogServer : public Application {
public:
    SysLogServer(int argc, char** argv);
    ~SysLogServer();

private:
    SysLogServer(const SysLogServer&);
    SysLogServer& operator=(const SysLogServer&);

    virtual void run();

private:  // members
    eckit::net::Port port_;
};

SysLogServer::SysLogServer(int argc, char** argv) :
    Application(argc, argv), port_("syslog", 6512) {}

SysLogServer::~SysLogServer() {}

void SysLogServer::run() {

    unique();

    net::UDPServer server(port_);

    Log::info() << Application::name() << " listening on port " << port_ << std::endl;

    size_t syslogMaxMessageSize = eckit::Resource<size_t>("syslogMaxMessageSize", 16 * 1024);
    eckit::Buffer buffer(syslogMaxMessageSize);

    for (;;) {

        size_t recv  = server.receive(buffer);
        buffer[recv] = '\0';

        Log::info() << "Received message " << Bytes(recv) << " : " << (const char*)buffer << std::endl;

        buffer.zero();
    }
}

}  // namespace eckit


//----------------------------------------------------------------------------------------------------------------------


int main(int argc, char** argv) {
    eckit::SysLogServer app(argc, argv);
    app.start();
    return 0;
}
