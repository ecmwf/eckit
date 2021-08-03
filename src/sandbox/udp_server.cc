
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
#include "eckit/parser/JSONParser.h"
#include "eckit/runtime/Application.h"
#include "eckit/utils/Translator.h"


namespace eckit {


class StatsServer : public Application {
public:
    StatsServer(int argc, char** argv);
    ~StatsServer();

private:
    StatsServer(const StatsServer&);
    StatsServer& operator=(const StatsServer&);

    virtual void run();

private:  // members
    eckit::net::Port port_;
};

StatsServer::StatsServer(int argc, char** argv) :
    Application(argc, argv), port_("statssvr", 7788) {}

StatsServer::~StatsServer() {}

void StatsServer::run() {

    unique();

    net::UDPServer server(port_);

    Log::info() << Application::name() << " listening on port " << port_ << std::endl;

    // UDP real max size is (64*1024 - 1 - 8 - 20), but we round up to next 4K
    size_t statsMaxMessageSize = eckit::Resource<size_t>("statsMaxMessageSize", 64 * 1024);
    eckit::Buffer buffer(statsMaxMessageSize);

    for (;;) {

        try {
            buffer.zero();
            size_t recv = server.receive(buffer);

            Log::info() << "Received message " << Bytes(recv) << " : " << (const char*)buffer << std::endl;

            Value v = JSONParser::decodeString((char*)buffer);
            Log::info() << "stats update:" << v << std::endl;
        }
        catch (StreamParser::Error& e) {
            Log::error() << "Parsing error " << e.what() << " on message: " << (const char*)buffer << std::endl;
        }
        catch (Exception& e) {
            Log::error() << "Exception  caught: " << e.what() << std::endl;
            return;
        }
        catch (...) {
            Log::error() << "Unknown error - exiting server" << std::endl;
            return;
        }
    }
}

}  // namespace eckit


//----------------------------------------------------------------------------------------------------------------------


int main(int argc, char** argv) {
    eckit::StatsServer app(argc, argv);
    app.start();
    return 0;
}
