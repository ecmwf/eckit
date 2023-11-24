/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <memory>

#include "eckit/config/MappedConfiguration.h"
#include "eckit/geo/Grid.h"
#include "eckit/log/Log.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/EckitTool.h"
#include "eckit/option/SimpleOption.h"
#include "eckit/exception/Exceptions.h"


namespace eckit::tools {


struct EckitGridTriangulation final : EckitTool {
    EckitGridTriangulation(int argc, char** argv) :
        EckitTool(argc, argv) {
        options_.push_back(new option::SimpleOption<std::string>("grid", "grid name"));
    }

    void execute(const option::CmdArgs&) override;

    void usage(const std::string& tool) const override {
        Log::info() << "\n"
                       "Usage: "
                    << tool << "--grid=O32" << std::endl;
    }
};


void EckitGridTriangulation::execute(const option::CmdArgs& args) {
    std::unique_ptr<const geo::Grid> grid(
        geo::GridFactory::build(MappedConfiguration{{{"grid", args.getString("grid")}}}));

    Log::info() << grid << std::endl;

    auto [lon, lat] = grid->to_latlon();
    ASSERT(lon.size() == lat.size());

    // TODO
}


}  // namespace eckit::tools


int main(int argc, char** argv) {
    eckit::tools::EckitGridTriangulation app(argc, argv);
    return app.start();
}
