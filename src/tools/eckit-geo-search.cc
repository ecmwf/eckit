// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <algorithm>
#include <chrono>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Figure.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Search.h"
#include "eckit/geo/projection/LonLatToXYZ.h"
#include "eckit/geo/search/KDTree.h"
#include "eckit/log/Log.h"
#include "eckit/log/Timer.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/EckitTool.h"
#include "eckit/option/SimpleOption.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/spec/Custom.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

class EckitGeoSearch final : public EckitTool {
public:

    EckitGeoSearch(int argc, char** argv) : EckitTool(argc, argv) {
        options_.push_back(new option::SimpleOption<std::string>("grid", "grid searched (spec)"));
        options_.push_back(new option::SimpleOption<std::string>(
            "query-grid", "grid of the queries (spec, default the grid searched)"));
        options_.push_back(new option::SimpleOption<std::string>(
            "search", "nn, knn, radius, knn_or_radius, knn_and_radius or knn_or_same_radius (default nn)"));
        options_.push_back(new option::SimpleOption<size_t>("k", "search-k"));
        options_.push_back(new option::SimpleOption<double>("radius", "search-radius"));
        options_.push_back(new option::SimpleOption<double>("tolerance", "search-tolerance"));
        options_.push_back(new option::SimpleOption<bool>("order-index", "neighbours by index, not by distance"));
        options_.push_back(new option::SimpleOption<std::string>(
            "query-order", "order to query in: morton (default), or input if queries near each other are consecutive"));
        options_.push_back(new option::SimpleOption<bool>("shuffle", "queries in random order (fixed seed)"));
        options_.push_back(new option::SimpleOption<bool>(
            "profile", "time the phases of the search, single-threaded, per query point (search nn or knn)"));
    }

private:

    static std::unique_ptr<const geo::Grid> grid(const std::string& str) {
        std::unique_ptr<spec::Spec> spec(spec::Custom::make_from_value(YAMLParser::decodeString(str)));
        return std::unique_ptr<const geo::Grid>(geo::GridFactory::build(*spec));
    }

    // best of 3, as timings vary between runs
    template <typename F>
    static double seconds(const F& f) {
        double best = 0.;
        for (int run = 0; run < 3; ++run) {
            const auto start = std::chrono::steady_clock::now();
            f();
            const auto t = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
            best         = run == 0 ? t : std::min(best, t);
        }
        return best;
    }

    // the phases of csr_search, timed separately (single-threaded, unlike csr_search itself with OpenMP)
    static void profile(const geo::Grid& grid, const geo::Search& search, const std::vector<geo::PointLonLat>& queries,
                        size_t k) {
        const auto n       = static_cast<double>(queries.size());
        const auto& kdtree = search.kdtree();

        std::vector<geo::PointXYZ> xyz;
        const auto t_convert = seconds([&]() {
            const geo::projection::LonLatToXYZ to_xyz(geo::FigureFactory::build(grid.figure()));
            xyz.clear();
            xyz.reserve(queries.size());
            for (const auto& q : queries) {
                xyz.emplace_back(to_xyz.fwd(q));
            }
        });

        std::vector<size_t> order;
        const auto t_order = seconds([&]() { order = geo::search::KDTree::query_order(xyz); });

        size_t found = 0;  // so the queries aren't optimised away
        auto tree    = [&](size_t i) {
            found +=
                k == 1 ? kdtree.nearest_neighbour(xyz[i]).index + 1 : kdtree.k_nearest_neighbours(xyz[i], k).size();
        };

        const auto t_tree_input = seconds([&]() {
            for (size_t i = 0; i < xyz.size(); ++i) {
                tree(i);
            }
        });
        const auto t_tree       = seconds([&]() {
            for (auto i : order) {
                tree(i);
            }
        });
        const auto t_search     = seconds([&]() {
            for (auto i : order) {
                found += search.search(xyz[i]).size();
            }
        });
        const auto t_csr        = seconds([&]() { found += search.csr_search(xyz).size(); });

        auto& out = Log::info();
        out << "profile (us per query point, single-threaded):\n"
            << "  convert (lon, lat) to (x, y, z): " << 1e6 * t_convert / n << "\n"
            << "  Morton order: " << 1e6 * t_order / n << "\n"
            << "  k-d tree queries, in Morton order: " << 1e6 * t_tree / n << "\n"
            << "  k-d tree queries, in input order: " << 1e6 * t_tree_input / n << "\n"
            << "  Search::search, over the k-d tree (sorting, copying): " << 1e6 * (t_search - t_tree) / n << "\n"
            << "  csr_search, over Search::search (CSR packing): " << 1e6 * (t_csr - t_order - t_search) / n << "\n"
            << "  csr_search of (x, y, z): " << 1e6 * t_csr / n << std::endl;
        ASSERT(found > 0);
    }

    void execute(const option::CmdArgs& args) override {
        const auto grid  = EckitGeoSearch::grid(args.getString("grid"));
        const auto query = args.has("query-grid") ? EckitGeoSearch::grid(args.getString("query-grid")) : nullptr;

        spec::Custom spec;
        spec.set("search", args.getString("search", "nn"));
        if (args.has("k")) {
            spec.set("search-k", args.getUnsigned("k"));
        }
        for (const std::string key : {"radius", "tolerance"}) {
            if (args.has(key)) {
                spec.set("search-" + key, args.getDouble(key));
            }
        }
        if (args.has("query-order")) {
            spec.set("search-query-order", args.getString("query-order"));
        }

        const auto order =
            args.getBool("order-index", false) ? geo::Search::ColumnOrder::INDEX : geo::Search::ColumnOrder::DISTANCE;

        const auto [lat, lon] = (query ? *query : *grid).to_latlons();
        std::vector<geo::PointLonLat> queries;
        queries.reserve(lat.size());
        for (size_t i = 0; i < lat.size(); ++i) {
            queries.emplace_back(lon[i], lat[i]);
        }

        if (args.getBool("shuffle", false)) {
            std::shuffle(queries.begin(), queries.end(), std::mt19937(1));
        }

        Timer timer("eckit-geo-search", Log::debug());

        const geo::Search search(*grid, spec);
        const auto t_build = timer.elapsed();

        const auto csr      = search.csr_search(queries, order);
        const auto t_search = timer.elapsed() - t_build;

        Log::info() << "num_threads: " << LibEcKitGeo::searchNumThreads() << "\n"
                    << "rows: " << csr.rows() << "\n"
                    << "cols: " << csr.cols << "\n"
                    << "entries: " << csr.size() << "\n"
                    << "build or load: " << t_build << " s\n"
                    << "search: " << t_search << " s" << std::endl;

        if (args.getBool("profile", false)) {
            const auto name = args.getString("search", "nn");
            if (name != "nn" && name != "knn") {
                throw UserError("eckit-geo-search: --profile requires --search=nn or knn");
            }
            profile(*grid, search, queries, name == "nn" ? 1 : args.getUnsigned("k"));
        }
    }

    void usage(const std::string& tool) const override {
        Log::info() << "\n"
                       "Usage: "
                    << tool << " --grid=<grid> [--query-grid=<grid>] [--search=<search> --k=<k> ...]" << std::endl;
    }

    int minimumPositionalArguments() const override { return 0; }
};

}  // namespace eckit

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    eckit::EckitGeoSearch app(argc, argv);
    return app.start();
}
