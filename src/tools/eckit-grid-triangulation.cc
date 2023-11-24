/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <array>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "eckit/config/MappedConfiguration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/projection/LonLatToXYZ.h"
#include "eckit/log/Log.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/EckitTool.h"
#include "eckit/option/SimpleOption.h"

#include "libqhullcpp/Qhull.h"
#include "libqhullcpp/QhullFacet.h"
#include "libqhullcpp/QhullFacetList.h"
#include "libqhullcpp/QhullPoints.h"
#include "libqhullcpp/QhullVertexSet.h"


namespace eckit::tools {


struct Triangulation {
    using triangle_type = std::array<countT, 3>;

    size_t n_triangles() const {
        ASSERT(qhull_);
        return static_cast<size_t>(qhull_->facetCount());
    }

    std::vector<triangle_type> triangles() const {
        ASSERT(qhull_);

        std::vector<triangle_type> tri;
        tri.reserve(qhull_->facetCount());

        for (const auto& facet : qhull_->facetList()) {
            auto vertices = facet.vertices();
            ASSERT(vertices.size() == 3);

            tri.emplace_back(triangle_type{vertices[0].id(), vertices[1].id(), vertices[2].id()});
        }

        return tri;
    }

protected:
    std::unique_ptr<const orgQhull::Qhull> qhull_;
};


struct Triangulation3D : Triangulation {
    Triangulation3D(const std::vector<double>& lon, const std::vector<double>& lat) {
        ASSERT(lon.size() == lat.size());

        auto N = static_cast<int>(lon.size());
        std::vector<double> coord(N * 3);

        geo::projection::LonLatToXYZ lonlat2xyz(1.);
        for (size_t i = 0; i < N; ++i) {
            auto p = lonlat2xyz.fwd({lon[i], lat[i]});

            coord[i * 3 + 0] = p.X;
            coord[i * 3 + 1] = p.Y;
            coord[i * 3 + 2] = p.Z;
        }

        qhull_ = std::make_unique<orgQhull::Qhull>("", 3, N, coord.data(), "Qt");
    }
};


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

    auto [lat, lon] = grid->to_latlon();
    ASSERT(lon.size() == lat.size());

    std::unique_ptr<Triangulation> tri(new Triangulation3D(lon, lat));
    for (const auto& t : tri->triangles()) {
        Log::info() << t[0] << ' ' << t[1] << ' ' << t[2] << std::endl;
    }
};


}  // namespace eckit::tools


int main(int argc, char** argv) {
    eckit::tools::EckitGridTriangulation app(argc, argv);
    return app.start();
}
