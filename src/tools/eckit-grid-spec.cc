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
#include <sstream>
#include <string>

#include "eckit/geo/Grid.h"
#include "eckit/log/Log.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/EckitTool.h"
#include "eckit/parser/YAMLParser.h"


namespace eckit {

class EckitGrid final : public EckitTool {
public:
    EckitGrid(int argc, char** argv) :
        EckitTool(argc, argv) {}

private:
    void execute(const option::CmdArgs& args) override {
        std::string user;

        if (args.count() == 0) {
            std::ostringstream out;
            YAMLParser(std::cin).parse().dump(out);
            user = out.str();
        }
        else {
            for (const auto& arg : args) {
                user += " " + arg;
            }
        }

        std::unique_ptr<const geo::Grid> grid(geo::GridFactory::make_from_string(user));
        Log::info() << grid->spec() << std::endl;
    }

    void usage(const std::string& tool) const override {
        Log::info() << "\n"
                       "Usage: "
                    << tool << "[options] ..." << std::endl;
    }

    int minimumPositionalArguments() const override { return 0; }
};

}  // namespace eckit


int main(int argc, char** argv) {
    eckit::EckitGrid app(argc, argv);
    return app.start();
}
