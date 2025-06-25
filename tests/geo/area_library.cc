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


#include <memory>

#include "eckit/geo/Area.h"
#include "eckit/geo/area/library/GeoJSON.h"
#include "eckit/geo/eckit_geo_config.h"

#if eckit_HAVE_GEO_AREA_SHAPEFILE
#include "eckit/geo/area/library/Shapefile.h"
#endif

#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("area_libraries") {
    using lib_type = std::unique_ptr<area::Library>;

    for (const auto& lib : {
             lib_type(new area::library::GeoJSON("CNTR_RG_60M_2024_4326_mini.geojson", "CNTR_NAME")),
#if eckit_HAVE_GEO_AREA_SHAPEFILE
             lib_type(new area::library::Shapefile("CNTR_RG_60M_2024_4326.shp.zip", "", "CNTR_NAME")),
#endif
         }) {
        lib->list(Log::info()) << std::endl;

        // std::unique_ptr<Area> area(gj.make_area(0));
        std::unique_ptr<Area> area(lib->make_area_from_name("Portugal"));

        for (double lat = 39.7; lat > 37.; lat -= .05) {
            for (double lon = -31.5; lon <= -23.5; lon += 0.05) {
                std::cout << (area->contains(PointLonLat{lon, lat}) ? '!' : '.');
            }
            std::cout << std::endl;
        }
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
