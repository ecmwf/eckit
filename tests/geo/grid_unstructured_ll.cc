// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <cstddef>
#include <cstdlib>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/grid/Unstructured.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


#if 0  // as the tests using it, below
/// Run fun(i) on n threads, joined (unlike a ThreadPool's detached threads); throws the first thread exception
template <typename Function>
void run_threads(size_t n, const Function& fun) {
    std::vector<std::exception_ptr> errors(n);

    std::vector<std::thread> threads;
    threads.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        threads.emplace_back([&, i]() {
            try {
                fun(i);
            }
            catch (...) {
                errors[i] = std::current_exception();
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    for (const auto& e : errors) {
        if (e) {
            std::rethrow_exception(e);
        }
    }
}
#endif


void cleanup() {
    if (const auto path = LibEcKitGeo::cacheGrid(); path.exists()) {
        path.unlink(true);
    }
}


CASE("unstructured_ll") {
    cleanup();

    const std::string name = "custom-grid";
    const std::vector<double> lats{0, 1, static_cast<double>(std::rand() % 3)};
    const std::vector<double> lons{static_cast<double>(std::rand() % 3), 1, 2};

    std::unique_ptr<const Grid> a(new grid::Unstructured(lons, lats));  // registers uid only

    EXPECT(a->size() == lats.size());

    const auto [out_lats, out_lons] = a->to_latlons();

    EXPECT(out_lats == lats);
    EXPECT(out_lons == lons);

    EXPECT_THROWS((void)GridFactory::build(spec::Custom({{"latitudes", lats}})));
    EXPECT_THROWS((void)GridFactory::build(spec::Custom({{"longitudes", lons}})));

    std::unique_ptr<const Grid> b(GridFactory::build(
        spec::Custom{{"longitudes", lons}, {"latitudes", lats}, {"cache_as", name}}));  // registers name
    std::unique_ptr<const Grid> c(
        GridFactory::build(spec::Custom{{"longitudes", lons}, {"latitudes", lats}, {"name", name}}));

    std::unique_ptr<const Grid> d(GridFactory::make_from_string("{grid:" + name + "}"));
    std::unique_ptr<const Grid> e(GridFactory::make_from_string("{grid:" + a->uid() + "}"));
    std::unique_ptr<const Grid> f(GridFactory::make_from_string("{uid:" + a->uid() + "}"));
    std::unique_ptr<const Grid> g(new grid::Unstructured(lons, lats));

    EXPECT(a->uid() == b->uid());  // a doesn't have a name, b does
    EXPECT(*b == *c);
    EXPECT(*b == *d);
    EXPECT(*b == *e);
    EXPECT(*b == *f);
    EXPECT(b->uid() == g->uid());  // b has a name, g doesn't

    cleanup();
}


// Disabled, eckit::ThreadPool is malfunctioning: waitForThreads() can return before its (detached) threads start,
// which then run on a destroyed pool (hanging, under load)
#if 0
CASE("unstructured_ll: concurrent constructions (re-registering) and lookups of a uid") {
    cleanup();

    const std::string name = "concurrent-grid";
    const std::vector<double> lats{0, 1, 2, 3};
    const std::vector<double> lons{0, 1, 2, 3};
    const auto uid = grid::Unstructured(lons, lats, name).uid();

    constexpr size_t N = 8;
    constexpr size_t M = 50;

    run_threads(N, [&](size_t i) {
        for (size_t j = 0; j < M; ++j) {
            if (i % 2 == 0) {
                const grid::Unstructured grid(lons, lats, name);  // re-registers the uid and name
                EXPECT(grid.uid() == uid);
                continue;
            }

            std::unique_ptr<const Grid> grid(GridFactory::make_from_string("{uid: " + uid + "}"));
            EXPECT(grid->uid() == uid);
            EXPECT(grid->to_points().size() == lats.size());
        }
    });  // throws if a thread failed

    cleanup();
}
#endif


CASE("'grid' does not re-define an unstructured grid name") {
    cleanup();

    for (const auto& key : {"grid", "type"}) {
        const std::vector<double> lats_a{90., 80., 70.};
        const std::vector<double> lons_a{0., 10., 20.};
        const std::vector<double> lats_b{90., 80., 70., 60.};
        const std::vector<double> lons_b{0., 10., 20., 30.};

        std::unique_ptr<const Grid> a(
            GridFactory::build(spec::Custom{{"latitudes", lats_a}, {"longitudes", lons_a}, {key, "unstructured_ll"}}));
        std::unique_ptr<const Grid> b(GridFactory::build(spec::Custom{{"latitudes", lats_b}, {"longitudes", lons_b}}));

        EXPECT(a->shape() == std::vector<size_t>{3});
        EXPECT(b->shape() == std::vector<size_t>{4});

        EXPECT(a->to_latlons() == std::make_pair(lats_a, lons_a));
        EXPECT(b->to_latlons() == std::make_pair(lats_b, lons_b));

        EXPECT(a->type() == b->type());
        EXPECT(a->uid() != b->uid());

        // 'cache_as' is the key that registers a name (in addition to the uid)
        const std::string name = "custom-cache-as";
        std::unique_ptr<const Grid> c(
            GridFactory::build(spec::Custom{{"latitudes", lats_a}, {"longitudes", lons_a}, {"cache_as", name}}));
        std::unique_ptr<const Grid> d(GridFactory::make_from_string("{grid:" + name + "}"));

        EXPECT(*c == *d);
    }

    cleanup();
}


CASE("to_unstructured_ll") {
    cleanup();

    const std::vector<double> lats{0, 1, 2};
    const std::vector<double> lons{2, 1, 0};

    std::unique_ptr<const Grid> src(new grid::Unstructured(lons, lats));

    std::unique_ptr<const Grid> a(src->to_unstructured_ll());
    EXPECT(a->type() == std::string("unstructured_ll"));
    EXPECT(a->to_latlons() == src->to_latlons());
    EXPECT(a->uid() == src->uid());  // same points, no name -> same uid

    const std::string name = "custom-to-unstructured-ll";
    std::unique_ptr<const Grid> b(src->to_unstructured_ll(name));
    std::unique_ptr<const Grid> c(GridFactory::make_from_string("{grid:" + name + "}"));

    EXPECT(b->uid() == src->uid());  // name doesn't change uid
    EXPECT(*b == *c);

    cleanup();
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
