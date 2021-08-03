#include "eckit/config/Resource.h"
#include "eckit/io/Buffer.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/JSON.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Telemetry.h"
#include "eckit/runtime/Tool.h"
#include "eckit/utils/Tokenizer.h"

using eckit::runtime::Report;
using eckit::runtime::Telemetry;

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class TelemetryClient : public eckit::Tool {
public:
    TelemetryClient(int argc, char** argv) :
        Tool(argc, argv, "TELEMETRY_HOME") {}

    ~TelemetryClient() {}

    virtual void run();
};

void TelemetryClient::run() {

    if (argc() == 1) {
        std::cerr << "usage: " << argv(0) << " <message> [...]" << std::endl;
        exit(1);
    }

    Telemetry::report(Report::APPSTART);

    std::ostringstream msg;
    msg << argv(1);
    for (int i = 2; i < argc(); ++i) {
        msg << " " << argv(i);
    }

    std::string message          = msg.str();
    std::function<void(JSON&)> f = [message](JSON& j) -> void { j << "message" << message; };
    Telemetry::report(Report::INFO, f);

    double rate = 10.5;
    Telemetry::report(Report::METER, [rate](JSON& j) -> void { j << "myrate" << rate; });

    int counter = 42;
    Telemetry::report(Report::COUNTER, [counter](JSON& j) -> void { j << "mycounter" << counter; });

    Telemetry::report(Report::APPSTOP);
}

}  // namespace eckit


//----------------------------------------------------------------------------------------------------------------------


int main(int argc, char* argv[]) {
    eckit::TelemetryClient tool(argc, argv);
    return tool.start();
}
