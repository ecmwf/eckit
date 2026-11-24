// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <cstddef>
#include <iostream>
#include <memory>
#include <string>

#include "eckit/eckit_config.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Area.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/area/Library.h"
#include "eckit/geo/area/library/GeoJSON.h"
#include "eckit/geo/eckit_geo_config.h"
#include "eckit/log/Log.h"
#include "eckit/testing/Test.h"

#if eckit_HAVE_GEO_AREA_SHAPEFILE
#include "eckit/geo/area/library/Shapefile.h"
#endif


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

        std::unique_ptr<Area> area(lib->make_area_from_name("Portugal"));

        for (double lat = 39.7; lat > 37.; lat -= .05) {
            for (double lon = -31.5; lon <= -23.5; lon += 0.05) {
                std::cout << (area->contains(PointLonLat{lon, lat}) ? '!' : '.');
            }
            std::cout << std::endl;
        }
    }
}


#if eckit_HAVE_GEO_AREA_SHAPEFILE
const std::string SHAPEFILE = PathName("CNTR_RG_60M_2024_4326.shp.zip").realName().asString();


CASE("shapefile area: spec") {
    using A = std::unique_ptr<const Area>;

    A portugal(AreaFactory::make_from_string("{type: shapefile, file: '" + SHAPEFILE +
                                             "', name_field: CNTR_NAME, name: Portugal}"));
    EXPECT(portugal->type() == "shapefile");
    EXPECT(portugal->spec_str() ==
           R"({"file":")" + SHAPEFILE + R"(","name":"Portugal","name_field":"CNTR_NAME","type":"shapefile"})");

    A same(AreaFactory::make_from_string(portugal->spec_str()));
    EXPECT(*same == *portugal);

    // mainland and islands (multiple rings)
    EXPECT(portugal->contains(PointLonLat{-8., 40.}));
    EXPECT(portugal->contains(PointLonLat{-25.5, 37.8}));
    EXPECT_NOT(portugal->contains(PointLonLat{-3.7, 40.4}));

    // as the library's
    A from_library(area::library::Shapefile(SHAPEFILE, "", "CNTR_NAME").make_area_from_name("Portugal"));
    EXPECT(*from_library == *portugal);

    // by index, specified by name (if the name field is known)
    A first(AreaFactory::make_from_string("{type: shapefile, file: '" + SHAPEFILE + "', index: 0}"));
    EXPECT(first->spec_str() == R"({"file":")" + SHAPEFILE + R"(","index":0,"type":"shapefile"})");

    A first_named(
        AreaFactory::make_from_string("{type: shapefile, file: '" + SHAPEFILE + "', name_field: CNTR_NAME, index: 0}"));
    EXPECT(first_named->spec().has("name"));
    EXPECT_NOT(first_named->spec().has("index"));

    for (const auto& spec : {
             "{type: shapefile, file: '" + SHAPEFILE + "'}",
             "{type: shapefile, file: '" + SHAPEFILE + "', name: Portugal}",
             "{type: shapefile, file: '" + SHAPEFILE + "', name_field: CNTR_NAME, name: Atlantis}",
             "{type: shapefile, file: '" + SHAPEFILE + "', index: 100000}",
         }) {
        EXPECT_THROWS_AS(A(AreaFactory::make_from_string(spec)), exception::AreaError);
    }
}


CASE("shapefile area: crop") {
    std::unique_ptr<const Area> portugal(AreaFactory::make_from_string("{type: shapefile, file: '" + SHAPEFILE +
                                                                       "', name_field: CNTR_NAME, name: Portugal}"));
    std::unique_ptr<const Grid> grid(GridFactory::make_from_string("{grid: [0.5, 0.5]}"));

    const auto indices = grid->crop(*portugal);
    EXPECT(!indices.empty());

    const auto points = grid->to_points();
    size_t contained  = 0;
    for (const auto& p : points) {
        contained += portugal->contains(p) ? 1 : 0;
    }
    EXPECT(indices.size() == contained);
}


#if eckit_HAVE_CURL
CASE("shapefile area: from a URL (downloaded and cached, transparently)") {
    using A = std::unique_ptr<const Area>;

    const auto url = "file://" + SHAPEFILE;
    A portugal(
        AreaFactory::make_from_string("{type: shapefile, url: '" + url + "', name_field: CNTR_NAME, name: Portugal}"));
    EXPECT(portugal->spec_str() ==
           R"({"name":"Portugal","name_field":"CNTR_NAME","type":"shapefile","url":")" + url + R"("})");

    A from_file(AreaFactory::make_from_string("{type: shapefile, file: '" + SHAPEFILE +
                                              "', name_field: CNTR_NAME, name: Portugal}"));
    for (const auto& p : {PointLonLat{-8., 40.}, PointLonLat{-25.5, 37.8}, PointLonLat{-3.7, 40.4}}) {
        EXPECT(portugal->contains(p) == from_file->contains(p));
    }

    // the same polygons, however specified
    EXPECT(portugal->spec_str() != from_file->spec_str());
    EXPECT(portugal->uid() == from_file->uid());
    EXPECT(*portugal == *from_file);
}
#endif


CASE("shapefile area: from a library") {
    using A = std::unique_ptr<const Area>;

    // a library as registered by the shared areas (see shapefile.yaml), here of a local file
    AreaFactory::add_library("test-cntr-rg-60m-2024",
                             new spec::Custom{{"type", "shapefile"}, {"file", SHAPEFILE}, {"name_field", "CNTR_NAME"}});

    A portugal(AreaFactory::make_from_string("{type: shapefile, library: test-cntr-rg-60m-2024, name: Portugal}"));
    EXPECT(portugal->spec_str() ==
           R"({"library":"test-cntr-rg-60m-2024","name":"Portugal","name_field":"CNTR_NAME","type":"shapefile"})");

    A same(AreaFactory::make_from_string(portugal->spec_str()));
    EXPECT(*same == *portugal);

    A from_file(AreaFactory::make_from_string("{type: shapefile, file: '" + SHAPEFILE +
                                              "', name_field: CNTR_NAME, name: Portugal}"));
    EXPECT(*portugal == *from_file);

    // other areas differ
    A france(AreaFactory::make_from_string("{type: shapefile, library: test-cntr-rg-60m-2024, name: France}"));
    EXPECT(*france != *portugal);

    EXPECT_THROWS_AS(A(AreaFactory::make_from_string("{type: shapefile, library: unknown, name: Portugal}")),
                     exception::AreaError);
}


CASE("shapefile area: libraries of the shared areas") {
    // relative urls are of the shared file's url_prefix
    std::unique_ptr<spec::Custom> lib(AreaFactory::library("cntr-rg-60m-2024"));
    EXPECT(lib->get_string("url") ==
           "https://gisco-services.ec.europa.eu/distribution/v2/countries/shp/CNTR_RG_60M_2024_4326.shp.zip");
    EXPECT(lib->get_string("name_field") == "CNTR_NAME");
}
#endif


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
