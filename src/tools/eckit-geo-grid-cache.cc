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
#include <regex>
#include <string>

#include "eckit/geo/Grid.h"
#include "eckit/log/Log.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/EckitTool.h"
#include "eckit/option/SimpleOption.h"
#include "eckit/spec/Custom.h"
#include "eckit/utils/StringTools.h"


namespace eckit::tools {


struct EckitGeoGridCache final : EckitTool {
    EckitGeoGridCache(int argc, char** argv) : EckitTool(argc, argv) {
        options_.push_back(new option::SimpleOption<std::string>("grid", "grid regular expression (case insensitive)"));
        options_.push_back(new option::SimpleOption<bool>("match", "match instead of search (default: false)"));
        options_.push_back(new option::SimpleOption<bool>("dryrun", "dry run (default: false)"));
    }

    void usage(const std::string& tool) const override {
        Log::info() << "\n"
                       "Usage: "
                    << tool << "[options] ..." << std::endl;
    }

    void execute(const option::CmdArgs& args) override {
        const auto match  = args.getBool("match", false);
        const auto dryrun = args.getBool("dryrun", false);

        auto cache = [dryrun](const spec::Spec& spec) {
            if (dryrun) {
                Log::info() << spec << std::endl;
            }
            else {
                std::unique_ptr<const geo::Grid>(geo::GridFactory::build(spec))->cache();
            }
        };

        for (const auto& arg : args) {
            if (const auto uid = StringTools::lower(arg); geo::Grid::is_uid(uid)) {
                cache(spec::Custom{{"uid", uid}});
                continue;
            }

            const std::regex pattern(arg, std::regex::icase);
            for (auto& [grid, _] : geo::GridSpecByName::instance().store()) {
                if (match ? std::regex_match(grid, pattern) : std::regex_search(grid, pattern)) {
                    cache(spec::Custom{{"grid", grid}});
                }
            }
        }
    }
};


}  // namespace eckit::tools


int main(int argc, char** argv) {
    eckit::tools::EckitGeoGridCache app(argc, argv);
    return app.start();
}
