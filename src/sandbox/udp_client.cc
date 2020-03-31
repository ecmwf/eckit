#include "eckit/config/Resource.h"
#include "eckit/io/Buffer.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Stats.h"
#include "eckit/runtime/Tool.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class StatsClient : public eckit::Tool {
public:
    StatsClient(int argc, char** argv) : Tool(argc, argv, "STATS_HOME") {}

    ~StatsClient() {}

    virtual void run();
};

void StatsClient::run() {

    if (argc() == 1) {
        std::cerr << "usage: " << argv(0) << " <message> [...]" << std::endl;
        exit(1);
    }

    std::ostringstream msg;
    msg << argv(1);
    for (int i = 2; i < argc(); ++i) {
        msg << " " << argv(i);
    }

    Stats::report("foo", msg.str());

}

}  // namespace eckit


//----------------------------------------------------------------------------------------------------------------------


int main(int argc, char* argv[]) {
    eckit::StatsClient app(argc, argv);
    return app.start();
}
