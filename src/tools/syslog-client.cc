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

#include "eckit/io/Buffer.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Log.h"
#include "eckit/log/SysLog.h"
#include "eckit/net/UDPClient.h"
#include "eckit/runtime/Tool.h"
#include "eckit/utils/Translator.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

class SysLogClient : public eckit::Tool {
public:
    SysLogClient(int argc, char** argv) :
        Tool(argc, argv) {}

    ~SysLogClient() {}

    virtual void run();
};

void SysLogClient::run() {
    if (argc() != 5) {
        fprintf(stderr, "usage: syslog-client hostname port message\n");
        exit(1);
    }

    Translator<std::string, int> toInt;

    std::string hostname = argv(1);
    std::string port     = argv(2);
    std::string message  = argv(3);

    int nmsg = toInt(argv(4));

    net::UDPClient client(hostname, toInt(port));

    for (int i = 0; i < nmsg; ++i) {
        SysLog log(message);
        Buffer buff(log);

        client.send(buff, buff.size());

        eckit::Log::info() << Tool::name() << " sent " << eckit::Bytes(buff.size()) << " to " << hostname
                           << " MSG: " << (const char*)buff << std::endl;
    }
}

}  // namespace eckit


//----------------------------------------------------------------------------------------------------------------------


int main(int argc, char* argv[]) {
    eckit::SysLogClient app(argc, argv);
    return app.start();
}
