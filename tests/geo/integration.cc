/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to itr by virtue of its status as an intergovernmental organisation nor
 * does itr submit to any jurisdiction.
 */


#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/eckit_geo_config.h"
#include "eckit/testing/Test.h"

#include "eckit/geo/Grid.h"


namespace eckit::geo::test {


using eckit::geo::Grid;


CASE("gridSpec") {
    struct test_type {
        const std::string spec;
        const std::vector<size_t> shape;
    };


    SECTION("regular lat/lon") {
        std::unique_ptr<const Grid> grid1(GridFactory::make_from_string("grid: 2/1.0"));
        EXPECT(grid1->spec_str() == R"({"grid":[2,1]})");

        std::unique_ptr<const Grid> grid2(GridFactory::make_from_string("grid: .250/001e-1"));
        EXPECT(grid2->spec_str() == R"({"grid":[0.25,0.1]})");

        std::vector<test_type> tests{
            {R"({"grid":[0.05,0.05]})", {3601, 7200}},
            {R"({"grid":[0.1,0.1]})", {1801, 3600}},
            {R"({"grid":[0.125,0.125]})", {1441, 2880}},
            {R"({"grid":[0.15,0.15]})", {1201, 2400}},
            {R"({"grid":[0.2,0.2]})", {901, 1800}},
            {R"({"grid":[0.25,0.25]})", {721, 1440}},
            {R"({"grid":[0.3,0.3]})", {601, 1200}},
            {R"({"grid":[0.35,0.35]})", {515, 1029}},
            {R"({"grid":[0.4,0.4]})", {451, 900}},
            {R"({"grid":[0.5,0.5]})", {361, 720}},
            {R"({"grid":[0.6,0.6]})", {301, 600}},
            {R"({"grid":[0.7,0.7]})", {257, 515}},
            {R"({"grid":[0.75,0.75]})", {241, 480}},
            {R"({"grid":[0.8,0.8]})", {225, 450}},
            {R"({"grid":[0.9,0.9]})", {201, 400}},
            {R"({"grid":[1,1]})", {181, 360}},
            {R"({"grid":[1.2,1.2]})", {151, 300}},
            {R"({"grid":[1.25,1.25]})", {145, 288}},
            {R"({"grid":[1.4,1.4]})", {129, 258}},
            {R"({"grid":[1.5,1.5]})", {121, 240}},
            {R"({"grid":[1.6,1.6]})", {113, 225}},
            {R"({"grid":[1.8,1.8]})", {101, 200}},
            {R"({"grid":[2,2]})", {91, 180}},
            {R"({"grid":[2.5,2.5]})", {73, 144}},
            {R"({"grid":[5,5]})", {37, 72}},
            {R"({"grid":[10,10]})", {19, 36}},
            {R"({"grid":[30,30]})", {7, 12}},
        };
        for (const auto& t : tests) {
            std::unique_ptr<const Grid> grid(GridFactory::make_from_string(t.spec));
            EXPECT(grid->spec_str() == t.spec);
            EXPECT(grid->shape() == t.shape);
        }
    }


    SECTION("HEALPix") {
        std::vector<test_type> tests{
            {R"({"grid":"H2","order":"nested"})", {48}},          {R"({"grid":"H2"})", {48}},
            {R"({"grid":"H4","order":"nested"})", {192}},         {R"({"grid":"H4"})", {192}},
            {R"({"grid":"H8","order":"nested"})", {768}},         {R"({"grid":"H8"})", {768}},
            {R"({"grid":"H16","order":"nested"})", {3072}},       {R"({"grid":"H16"})", {3072}},
            {R"({"grid":"H32","order":"nested"})", {12288}},      {R"({"grid":"H32"})", {12288}},
            {R"({"grid":"H64","order":"nested"})", {49152}},      {R"({"grid":"H64"})", {49152}},
            {R"({"grid":"H128","order":"nested"})", {196608}},    {R"({"grid":"H128"})", {196608}},
            {R"({"grid":"H256","order":"nested"})", {786432}},    {R"({"grid":"H256"})", {786432}},
            {R"({"grid":"H512","order":"nested"})", {3145728}},   {R"({"grid":"H512"})", {3145728}},
            {R"({"grid":"H1024","order":"nested"})", {12582912}}, {R"({"grid":"H1024"})", {12582912}},
        };
        for (const auto& t : tests) {
            std::unique_ptr<const Grid> grid(GridFactory::make_from_string(t.spec));
            EXPECT(grid->spec_str() == t.spec);
            EXPECT(grid->shape() == t.shape);
        }
    }


    SECTION("Gaussian grids") {
        std::vector<test_type> tests{
            {R"({"grid":"N32"})", {6114}},       {R"({"grid":"N48"})", {13280}},     {R"({"grid":"N64"})", {23112}},
            {R"({"grid":"N80"})", {35718}},      {R"({"grid":"N96"})", {50662}},     {R"({"grid":"N128"})", {88838}},
            {R"({"grid":"N160"})", {138346}},    {R"({"grid":"N200"})", {213988}},   {R"({"grid":"N256"})", {348528}},
            {R"({"grid":"N320"})", {542080}},    {R"({"grid":"N400"})", {843490}},   {R"({"grid":"N512"})", {1373624}},
            {R"({"grid":"N640"})", {2140702}},   {R"({"grid":"N1024"})", {5447118}}, {R"({"grid":"N1280"})", {8505906}},

            {R"({"grid":"O32"})", {5248}},       {R"({"grid":"O48"})", {10944}},     {R"({"grid":"O64"})", {18688}},
            {R"({"grid":"O80"})", {28480}},      {R"({"grid":"O96"})", {40320}},     {R"({"grid":"O128"})", {70144}},
            {R"({"grid":"O160"})", {108160}},    {R"({"grid":"O200"})", {167200}},   {R"({"grid":"O256"})", {271360}},
            {R"({"grid":"O320"})", {421120}},    {R"({"grid":"O400"})", {654400}},   {R"({"grid":"O512"})", {1067008}},
            {R"({"grid":"O640"})", {1661440}},   {R"({"grid":"O1024"})", {4231168}}, {R"({"grid":"O1280"})", {6599680}},
            {R"({"grid":"O2560"})", {26306560}},
        };
        for (const auto& t : tests) {
            std::unique_ptr<const Grid> grid(GridFactory::make_from_string(t.spec));
            EXPECT(grid->spec_str() == t.spec);
            EXPECT(grid->shape() == t.shape);
        }
    }


    if (eckit_HAVE_ECKIT_CODEC) {
        SECTION("gridSpec (FESOM)") {
            std::vector<test_type> tests{
                {R"({"grid":"pi_N"})", {3140}},
            };
            for (const auto& t : tests) {
                std::unique_ptr<const Grid> grid(GridFactory::make_from_string(t.spec));
                EXPECT(grid->spec_str() == t.spec);
                EXPECT(grid->shape() == t.shape);
            }
        }


        SECTION("gridSpec (ICON)") {
            std::vector<test_type> tests{
                {R"({"grid":"icon-grid-0055-r02b05-n"})", {2656}},
            };
            for (const auto& t : tests) {
                std::unique_ptr<const Grid> grid(GridFactory::make_from_string(t.spec));
                EXPECT(grid->spec_str() == t.spec);
                EXPECT(grid->shape() == t.shape);
            }
        }


        SECTION("gridSpec (ORCA)") {
            std::vector<test_type> tests{
                {R"({"grid":"ORCA2_T"})", {182, 149}},  // eORCA025_T [1740494] is too large as an integration test
            };
            for (const auto& t : tests) {
                std::unique_ptr<const Grid> grid(GridFactory::make_from_string(t.spec));
                EXPECT(grid->spec_str() == t.spec);
                EXPECT(grid->shape() == t.shape);
            }
        }
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
