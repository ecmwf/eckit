/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/Grid.h"
#include "eckit/log/Log.h"
#include "eckit/option/EckitTool.h"


namespace eckit::tools {


struct EckitGridList final : EckitTool {
    EckitGridList(int argc, char** argv) : EckitTool(argc, argv) {}

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
    eckit::tools::EckitGridList app(argc, argv);
    return app.start();
}
