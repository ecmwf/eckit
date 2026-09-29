// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/Grid.h"
#include "eckit/log/Log.h"
#include "eckit/option/EckitTool.h"


namespace eckit::tools {


struct EckitGeoGridList final : EckitTool {
    EckitGeoGridList(int argc, char** argv) : EckitTool(argc, argv) {}

    void execute(const option::CmdArgs&) override {
        geo::GridFactory::list(Log::info());
        Log::info() << std::endl;
    }

    void usage(const std::string& tool) const override {
        Log::info() << "\n"
                       "Usage: "
                    << tool << "[options] ..." << std::endl;
    }
};


}  // namespace eckit::tools


int main(int argc, char** argv) {
    eckit::tools::EckitGeoGridList app(argc, argv);
    return app.start();
}
