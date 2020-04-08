#include "eckit/config/Resource.h"
#include "eckit/io/Buffer.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Telemetry.h"
#include "eckit/runtime/Tool.h"
#include "eckit/utils/Tokenizer.h"

using eckit::runtime::Telemetry;
using eckit::runtime::Report;

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class TelemetryClient : public eckit::Tool {
public:
    TelemetryClient(int argc, char** argv) : Tool(argc, argv, "TELEMETRY_HOME") {}

    ~TelemetryClient() {}

    virtual void run();
};

void TelemetryClient::run() {

    if (argc() == 1) {
        std::cerr << "usage: " << argv(0) << " <message> [...]" << std::endl;
        exit(1);
    }

    std::ostringstream msg;
    msg << argv(1);
    for (int i = 2; i < argc(); ++i) {
        msg << " " << argv(i);
    }

    Telemetry::report(Report::APPSTART);

    Telemetry::report(Report::APPSTOP);
}

}  // namespace eckit


//----------------------------------------------------------------------------------------------------------------------


int main(int argc, char* argv[]) {
    eckit::TelemetryClient tool(argc, argv);
    return tool.start();
}
