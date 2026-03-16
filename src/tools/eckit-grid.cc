/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <algorithm>
#include <memory>
#include <sstream>
#include <string>

#include "eckit/geo/Grid.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/log/JSON.h"
#include "eckit/log/Log.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/EckitTool.h"
#include "eckit/option/SimpleOption.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/spec/Custom.h"


namespace eckit {


class EckitGrid final : public EckitTool {
public:

    EckitGrid(int argc, char** argv) : EckitTool(argc, argv) {
        options_.push_back(new option::SimpleOption<std::string>("grid", "spec"));
        options_.push_back(new option::SimpleOption<bool>("minmax-ll", "Limits for (lon, lat) coordinates"));
        options_.push_back(new option::SimpleOption<bool>("calculate-bbox", "Calculate bounding box"));
        options_.push_back(new option::SimpleOption<bool>("calculate-uid", "Calculate UID"));
        options_.push_back(new option::SimpleOption<bool>("validate-uid", "Validate UID"));
        options_.push_back(new option::SimpleOption<long>("precision", "Output precision"));
    }

private:

    void usage(const std::string& tool) const override {
        Log::info() << "\n"
                       "Usage: "
                    << tool << "[options] ..." << std::endl;
    }

    int minimumPositionalArguments() const override { return 1; }

    void execute(const option::CmdArgs& args) override {
        std::stringstream stream;
        JSON out(stream);
        out.precision(args.getInt("precision", 16));
        out.startObject();

        std::string spec;
        for (const auto& arg : args) {
            spec += " " + arg;
        }

        std::unique_ptr<const geo::Grid> grid([](const std::string& str) -> const geo::Grid* {
            std::unique_ptr<spec::Spec> spec(spec::Custom::make_from_value(YAMLParser::decodeString(str)));
            return geo::GridFactory::build(*spec);
        }(spec));

        out << "spec" << grid->spec_str();
        out << "uid" << grid->uid();
        out << "size" << grid->size();

        {
            auto bbox = grid->boundingBox();
            out << "bounding box";
            (out.startList() << bbox.north() << bbox.west() << bbox.south() << bbox.east()).endList();
        }

        auto minmax_latlon = args.getBool("minmax-ll", false);
        if (minmax_latlon) {
            auto [lat, lon] = grid->to_latlons();
            ASSERT(lat.size() == lon.size());
            ASSERT(!lat.empty());

            geo::PointLonLat min{lon.front(), lat.front()};
            geo::PointLonLat max{min};

            for (size_t i = 0; i < lat.size(); ++i) {
                min = {std::min(min.lon(), lon[i]), std::min(min.lat(), lat[i])};
                max = {std::max(max.lon(), lon[i]), std::max(max.lat(), lat[i])};
            }

            out << "min";
            (out.startList() << min.lon() << min.lat()).endList();

            out << "max";
            (out.startList() << max.lon() << max.lat()).endList();
        }

        auto calculate_bbox = args.getBool("calculate-bbox", false);
        if (calculate_bbox) {
            std::unique_ptr<geo::area::BoundingBox> bbox{grid->calculate_bbox()};
            out << "bounding box (calculated)";
            (out.startList() << bbox->north() << bbox->west() << bbox->south() << bbox->east()).endList();
        }

        auto calculate_uid = args.getBool("calculate-uid", false);
        if (calculate_uid) {
            out << "uid (calculated)" << grid->calculate_uid();
        }

        out.endObject();
        Log::info() << stream.str() << std::endl;

        auto validate_uid = args.getBool("validate-uid", false);
        if (validate_uid) {
            auto uid = grid->uid();
            Log::info() << "uid: '" << uid << "'" << std::endl;

            auto uid_calculated = grid->calculate_uid();
            Log::info() << "uid (calculated): '" << uid_calculated << "'" << std::endl;

            if (uid != uid_calculated) {
                throw BadValue("'" + uid + "' != '" + uid_calculated + "'");
            }
        }

        Log::info() << std::endl;
    }
};


}  // namespace eckit


int main(int argc, char** argv) {
    eckit::EckitGrid app(argc, argv);
    return app.start();
}
