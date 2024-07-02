/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <cmath>
#include <memory>
#include <sstream>
#include <string>

#include "eckit/geo/Grid.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/log/JSON.h"
#include "eckit/log/Log.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/EckitTool.h"
#include "eckit/option/SimpleOption.h"
#include "eckit/parser/YAMLParser.h"


namespace eckit {

class EckitGrid final : public EckitTool {
public:
    EckitGrid(int argc, char** argv) : EckitTool(argc, argv) {
        options_.push_back(new option::SimpleOption<std::string>("grid", "grid spec"));
        options_.push_back(new option::SimpleOption<bool>("bounding-box", "Bounding box"));
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
        eckit::JSON out(stream);
        out.precision(args.getInt("precision", 16));

        std::string user;
        for (const auto& arg : args) {
            user += " " + arg;
        }

        std::unique_ptr<const geo::Grid> grid([](const std::string& str) -> const geo::Grid* {
            std::unique_ptr<geo::Spec> spec(geo::spec::Custom::make_from_value(YAMLParser::decodeString(str)));
            return geo::GridFactory::build(*spec);
        }(user));

        out << "spec" << grid->spec_str();
        out << "uid" << grid->uid();
        out << "size" << grid->size();


        // Earth's equator ~= 40075 km
        auto deg_km
            = [](double deg) { return std::to_string(deg) + " deg, " + std::to_string(deg * 40075. / 360.) + " km"; };

        auto nx = 1;  // grid->nx();
        auto ny = 1;  // grid->ny();

        out << "shape";
        (out.startList() << nx << ny).endList();

        // out << "resolution N-S" << deg_km((grid->y().front() - grid->y().back()) / (ny - 1));
        // out << "resolution E-W equator" << deg_km(360. / static_cast<double>(grid->nx(ny / 2)));
        // out << "resolution E-W midlat" << deg_km(360. * std::cos(grid->y(ny / 4) * M_PI / 180.) /
        // static_cast<double>(grid->nx(ny / 4))) << "resolution E-W pole" << deg_km(360. *
        // std::cos(grid->y().front() * M_PI / 180.) / static_cast<double>(grid->nx().front()));

        out << "spectral truncation linear" << (ny - 1);
        out << "spectral truncation quadratic" << (static_cast<int>(std::floor(2. / 3. * ny + 0.5)) - 1);
        out << "spectral truncation cubic" << (static_cast<int>(std::floor(0.5 * ny + 0.5)) - 1);

        {
            auto points = grid->to_points();
            auto first  = std::get<geo::PointLonLat>(points.front());
            auto last   = std::get<geo::PointLonLat>(points.back());

            out << "first";
            (out.startList() << first.lon << first.lat).endList();

            out << "last";
            (out.startList() << last.lon << last.lat).endList();
        }

        {
            auto bbox = grid->boundingBox();
            out << "bounding box";
            (out.startList() << bbox.north << bbox.west << bbox.south << bbox.east).endList();
        }

        Log::info() << stream.str() << std::endl;
    }
};

}  // namespace eckit


int main(int argc, char** argv) {
    eckit::EckitGrid app(argc, argv);
    return app.start();
}
