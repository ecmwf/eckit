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
#include <ostream>

#include "eckit/config/MappedConfiguration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/projection/LonLatToXYZ.h"
#include "eckit/geo/triangulation/Triangulation3.h"
#include "eckit/log/Log.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/EckitTool.h"
#include "eckit/option/SimpleOption.h"


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

    auto& out = std::cout;

    geo::triangulation::Triangulation3 tri(*grid);

    auto [lat, lon] = grid->to_latlon();

    auto N = grid->size();
    ASSERT(N == lat.size());
    ASSERT(N == lon.size());

    out << "$MeshFormat\n"
           "4.1 0 8\n"
           "$EndMeshFormat\n";

    out << "$Nodes\n" << N << '\n';
    geo::projection::LonLatToXYZ lonlat2xyz(1.);
    for (size_t i = 0; i < N; ++i) {
        auto p = lonlat2xyz.fwd(geo::PointLonLat{lon[i], lat[i]});
        out << (i + 1) << ' ' << p.X << ' ' << p.Y << ' ' << p.Z << '\n';
    }
    out << "$EndNodes\n";

    out << "$Elements\n" << tri.size() << '\n';
    size_t i = 0;
    for (const auto& t : tri) {
        out << (i + 1) << " 2 4 1 1 1 0 " << t[0] << ' ' << t[1] << ' ' << t[2] << '\n';
    }
    out << "$EndElements\n";
};


}  // namespace eckit::tools


int main(int argc, char** argv) {
    eckit::tools::EckitGridTriangulation app(argc, argv);
    return app.start();
}
