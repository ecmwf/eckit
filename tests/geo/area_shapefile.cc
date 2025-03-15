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


#include "eckit/geo/Shapefile.h"

#include <memory>

#include "eckit/geo/Area.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("Shapefile") {
    Shapefile shp("world_countries_coasts.shp.zip", "", "country");

    shp.list(Log::info()) << std::endl;

    // std::unique_ptr<Area> area(shp.make_area_from_entity(0));
    std::unique_ptr<Area> area(shp.make_area_from_name("Azores"));

    for (double lat = 39.7; lat > 37.; lat -= .05) {
        for (double lon = -31.33; lon <= -25.13; lon += 0.05) {
            std::cout << (area->contains({lon, lat}) ? '!' : '.');
        }
        std::cout << std::endl;
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
