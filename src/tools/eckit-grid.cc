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
#include <string>

#include "eckit/config/MappedConfiguration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/geometry/Grid.h"
#include "eckit/geometry/Point.h"
#include "eckit/geometry/Search.h"
#include "eckit/geometry/grid/Unstructured.h"
#include "eckit/log/Log.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/EckitTool.h"
#include "eckit/option/SimpleOption.h"
#include "eckit/option/VectorOption.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

class EckitGrid final : public EckitTool {
public:
    EckitGrid(int argc, char** argv) :
        EckitTool(argc, argv) {
        options_.push_back(new option::SimpleOption<bool>("uid", "by grid unique identifier, instead of name"));
        options_.push_back(new option::VectorOption<double>("nearest-point", "nearest point location (lon/lat)", 2));
        options_.push_back(new option::SimpleOption<size_t>("nearest-k", "nearest k points"));
    }

private:
    void execute(const option::CmdArgs& args) override {
        auto uid = args.getBool("uid", false);

        geometry::PointLonLat nearest_point{0, 0};
        size_t nearest_k = 0;
        if (std::vector<double> point; args.get("nearest-point", point)) {
            ASSERT(point.size() == 2);
            nearest_point = {point[0], point[1]};
            nearest_k     = args.getUnsigned("nearest-k", 1);
        }

        auto& out = Log::info();
        out.precision(args.getInt("precision", 16));

        for (const auto& arg : args) {
            std::unique_ptr<Configuration> cfg(new MappedConfiguration({{uid ? "uid" : "name", std::string(arg)}}));
            std::unique_ptr<const geometry::Grid> grid(geometry::GridFactory::build(*cfg));

            out << "size: " << grid->size() << std::endl;

            for (const auto& p : *grid) {
                out << p << std::endl;
            }

            //            for (const auto& p : geometry::grid::UnstructuredGrid(*grid)) {
            //                out << p << std::endl;
            //            }

            if (nearest_k > 0) {
                geometry::SearchLonLat search;
                search.build(grid->to_points());

                const auto* sep = "";
                for (auto& near : search.kNearestNeighbours(nearest_point, nearest_k)) {
                    out << sep << near;
                    sep = ", ";
                }
                out << std::endl;
            }

            // it += grid->size() - 1;
            // out << "last: " << *it << std::endl;
            // ASSERT(it == grid->rbegin());
        }
    }

    void usage(const std::string& tool) const override {
        Log::info() << "\n"
                       "Usage: "
                    << tool << "[options] ..." << std::endl;
    }

    int minimumPositionalArguments() const override {
        return 0;
    }
};

}  // namespace eckit

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    eckit::EckitGrid app(argc, argv);
    return app.start();
}
