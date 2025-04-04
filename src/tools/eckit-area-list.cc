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

#include "eckit/geo/Area.h"
#include "eckit/geo/eckit_geo_config.h"
#include "eckit/log/Log.h"
#include "eckit/option/EckitTool.h"

#if eckit_HAVE_GEO_AREA_SHAPEFILE
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/area/library/Shapefile.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/SimpleOption.h"
#endif


namespace eckit::tools {


struct EckitAreaList final : EckitTool {
    EckitAreaList(int argc, char** argv) : EckitTool(argc, argv) {
#if eckit_HAVE_GEO_AREA_SHAPEFILE
        options_.push_back(new option::SimpleOption<std::string>("shapefile-file", "Shapefile file path"));
        options_.push_back(new option::SimpleOption<std::string>("shapefile-url", "Shapefile URL"));
#endif
    }

    void execute(const option::CmdArgs& args) override {
#if eckit_HAVE_GEO_AREA_SHAPEFILE
        using geo::area::library::Shapefile;

        if (PathName file = args.getString("shapefile-file", ""); !file.asString().empty()) {
            std::make_unique<Shapefile>(file)->list(Log::info());
            Log::info() << std::endl;

            return;
        }

        if (auto url = args.getString("shapefile-url"); !url.empty()) {
            std::unique_ptr<Shapefile>(Shapefile::make_from_url(url))->list(Log::info());
            Log::info() << std::endl;

            return;
        }
#endif

        geo::AreaFactory::list(Log::info());
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
    eckit::tools::EckitAreaList app(argc, argv);
    return app.start();
}
