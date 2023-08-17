/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/geometry/Grid.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/option/EckitTool.h"
#include "eckit/option/SimpleOption.h"

#include "eckit/geometry/GridConfig.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

class EckitGrid final : public EckitTool {
public:
    EckitGrid(int argc, char** argv) :
        EckitTool(argc, argv) {
        options_.push_back(new option::SimpleOption<bool>("list", "List possible grids"));
    }

private:
    void execute(const option::CmdArgs& args) override {

        Log::info() << geometry::GridConfig::instance().config() << std::endl;

        geometry::GridFactory::list(Log::info());
        Log::info() << std::endl;
    }

    void usage(const std::string& tool) const override {
        Log::info() << "\n"
                       "Usage: "
                    << tool << "[options] ..." << std::endl;
    }
};

}  // namespace eckit

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    eckit::EckitGrid app(argc, argv);
    return app.start();
}
