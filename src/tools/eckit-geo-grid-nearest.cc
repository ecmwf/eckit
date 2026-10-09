// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <memory>
#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Figure.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Search.h"
#include "eckit/geo/projection/LonLatToXYZ.h"
#include "eckit/log/Log.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/EckitTool.h"
#include "eckit/option/SimpleOption.h"
#include "eckit/option/VectorOption.h"
#include "eckit/spec/Custom.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

class EckitGeoGridNearest final : public EckitTool {
public:

    EckitGeoGridNearest(int argc, char** argv) : EckitTool(argc, argv) {
        options_.push_back(new option::SimpleOption<bool>("uid", "by grid unique identifier, instead of name"));
        options_.push_back(new option::VectorOption<double>("nearest-point", "nearest point location (lon/lat)", 2));
        options_.push_back(new option::SimpleOption<size_t>("nearest-k", "nearest k points"));
    }

private:

    void execute(const option::CmdArgs& args) override {
        size_t nearest_k = args.getUnsigned("nearest-k", 1);
        if (nearest_k == 0) {
            return;
        }

        auto uid = args.getBool("uid", false);

        std::vector<double> point{0, 0};
        args.get("nearest-point", point);
        ASSERT(point.size() == 2);

        geo::PointLonLat nearest_point{point[0], point[1]};

        auto& out = Log::info();
        out.precision(args.getInt("precision", 16));

        for (const auto& arg : args) {
            std::unique_ptr<const geo::Grid> grid(
                geo::GridFactory::build(spec::Custom({{uid ? "uid" : "grid", std::string(arg)}})));

            const geo::Search search(*grid);
            geo::projection::LonLatToXYZ to_xyz(grid->figure().a(), grid->figure().b());

            for (const auto& near : search.search_knn(nearest_point, nearest_k)) {
                out << near.index << ", " << to_xyz.inv(near.point) << ", " << near.distance << std::endl;
            }
        }
    }

    void usage(const std::string& tool) const override {
        Log::info() << "\n"
                       "Usage: "
                    << tool << "[options] ..." << std::endl;
    }

    int minimumPositionalArguments() const override { return 0; }
};

}  // namespace eckit

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    eckit::EckitGeoGridNearest app(argc, argv);
    return app.start();
}
