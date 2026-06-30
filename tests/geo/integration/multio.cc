/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <algorithm>
#include <random>
#include <string>
#include <vector>

#include "eckit/geo/Grid.h"
#include "eckit/geo/grid/ORCA.h"
#include "eckit/geo/grid/reduced/HEALPix.h"
#include "eckit/geo/grid/reduced/ReducedGaussian.h"
#include "eckit/geo/grid/regular/RegularGaussian.h"
#include "eckit/geo/grid/regular/RegularLL.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


using Sequence = std::vector<unsigned long>;


Sequence make_random_sequence(Sequence::value_type min, Sequence::value_type max, Sequence::size_type N) {
    static std::mt19937 gen{std::random_device{}()};
    static std::uniform_int_distribution<Sequence::value_type> dist(min, max);

    Sequence s(N);
    std::generate(s.begin(), s.end(), [&] { return dist(gen); });
    return s;
}


Sequence make_power_sequence(Sequence::value_type min, Sequence::value_type max) {
    Sequence s;
    for (auto i = min; i <= max; i *= 2) {
        s.push_back(i);
    }
    return s;
}


CASE("MARS grid") {
    auto grid_type = [](const std::string& grid) {
        std::unique_ptr<const eckit::geo::Grid> g(eckit::geo::GridFactory::build(eckit::spec::Custom{{"grid", grid}}));
        return g->type();
    };


    SECTION("regular_ll") {
        static std::mt19937 gen{std::random_device{}()};
        static std::uniform_real_distribution<double> dist(0.001, 1.);

        EXPECT("regular_ll" == grid_type(std::to_string(dist(gen)) + "/" + std::to_string(dist(gen))));

        EXPECT_THROWS_AS(grid_type("1/-0.1"), eckit::Exception);
    }


    SECTION("regular_gg") {
        const Sequence F{16,  24,  32,  48/*,  64,  80,  96,   128,  160,  192,  200,  256,  320,
                         400, 512, 576, 640, 800, 912, 1024, 1280, 1600, 2000, 2560, 4000, 8000*/};

        const auto Fn = [](const auto& other) {
            auto seq = make_random_sequence(2, 640 /*8000*/, 20);
            seq.insert(seq.end(), other.begin(), other.end());
            return seq;
        }(F);

        for (const auto& n : Fn) {
            EXPECT("regular_gg" == grid_type("F" + std::to_string(n)));
        }
    }


    SECTION("reduced_gg") {
        for (const auto& n : make_random_sequence(2, 8000, 20 /*reduced number of tests*/)) {
            EXPECT("reduced_gg" == grid_type("O" + std::to_string(n)));
        }

        for (const auto& n : Sequence{32, 48, 64, 80, 96, 128, 160, 200, 256, 320, 400, 512, 640, 800,
                                      /*1024, 1280, 8000*/}) {
            EXPECT("reduced_gg" == grid_type("N" + std::to_string(n)));
        }

        EXPECT_THROWS_AS(grid_type("N1279"), eckit::Exception);
    }


    SECTION("ORCA") {
        for (const std::string& name : {
                 "ORCA025",
                 "ORCA1",
                 "ORCA12",
                 "ORCA2",
                 "eORCA025",
                 "eORCA1",
                 "eORCA12",
             }) {
            EXPECT("ORCA" == grid_type(name + "_F"));
            EXPECT("ORCA" == grid_type(name + "_T"));
            EXPECT("ORCA" == grid_type(name + "_U"));
            EXPECT("ORCA" == grid_type(name + "_V"));
            EXPECT("ORCA" == grid_type(name + "_W"));

            EXPECT_THROWS_AS(grid_type(name), eckit::Exception);
            EXPECT_THROWS_AS(grid_type(name + "_X"), eckit::Exception);
        }
    }
}


CASE("eckit::geo Grid") {
    // FIXME This is largely testing compilation, downstream should NOT rely on API calls but in communicating
    // key/values, and use polymorphism instead


    SECTION("regular_ll") {
        grid::regular::RegularLL grid({1., 1.});

        Log::info() << "Grid: '" << grid.spec_str() << "'" << std::endl
                    << "grid.nlon() = " << grid.nlon()                   //
                    << "\ngrid.nlat() = " << grid.nlat()                 //
                    << "\ngrid.first_point()) = " << grid.first_point()  //
                    << "\ngrid.last_point()) = " << grid.last_point()    //
                    << "\ngrid.dlon() = " << grid.dlon()                 //
                    << "\ngrid.dlat() = " << grid.dlat()                 //
                    << std::endl;
    }


    SECTION("reduced_gg") {
        grid::reduced::ReducedGaussian grid(20);

        Log::info() << "Grid: '" << grid.spec_str() << "'" << std::endl
                    << "grid.pl()[0] = " << grid.pl().front()                                              //
                    << "\ngrid.ny() = " << grid.ny()                                                       //
                    << "\ngrid.latitudes()[0] = " << grid.latitudes().front()                              //
                    << "\ngrid.longitudes(grid.ny() / 2)[-1] = " << grid.longitudes(grid.ny() / 2).back()  //
                    << std::endl;
    }


    SECTION("regular_gg") {
        grid::regular::RegularGaussian grid(20);

        Log::info() << "Grid: '" << grid.spec_str() << "'" << std::endl
                    << "grid.first_point()) = " << grid.first_point()  //
                    << "\ngrid.last_point()) = " << grid.last_point()  //
                    << "\ngrid.dx() = " << grid.dx()                   // FIXME Did we ever need this???
                    << std::endl;
    }


    SECTION("HEALPix") {
        grid::reduced::HEALPix grid(2);

        Log::info() << "Grid: '" << grid.spec_str() << "'" << std::endl
                    << "grid.first_point()) = " << grid.first_point()  //
                    << "\ngrid.last_point()) = " << grid.last_point()  //
                    << "grid.Nside() = " << grid.Nside()               //
                    << "grid.order() = " << grid.order()               //
                    << std::endl;
    }


    SECTION("ORCA") {
        static const std::string UID = "d5bde4f52ff3a9bea5629cd9ac514410";
        grid::ORCA grid(UID);

        Log::info() << "Grid: '" << grid.spec_str() << "'" << std::endl << std::endl;

        EXPECT(grid.uid() == UID);
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
