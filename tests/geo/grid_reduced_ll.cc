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
#include <vector>

#include "eckit/geo/Grid.h"
#include "eckit/geo/Point.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("points") {
    spec::Custom spec{{"type", "reduced_ll"}, {"pl", std::vector<long>{20, 24, 24, 20}}};
    std::unique_ptr<const Grid> grid(GridFactory::build(spec));

    EXPECT_EQUAL(grid->size(), 88);

    const std::vector<PointLonLat> ref{
        {0., 90.},    {18., 90.},   {36., 90.},   {54., 90.},   {72., 90.},    //
        {90., 90.},   {108., 90.},  {126., 90.},  {144., 90.},  {162., 90.},   //
        {180., 90.},  {198., 90.},  {216., 90.},  {234., 90.},  {252., 90.},   //
        {270., 90.},  {288., 90.},  {306., 90.},  {324., 90.},  {342., 90.},   //
        {0., 30.},    {15., 30.},   {30., 30.},   {45., 30.},   {60., 30.},    //
        {75., 30.},   {90., 30.},   {105., 30.},  {120., 30.},  {135., 30.},   //
        {150., 30.},  {165., 30.},  {180., 30.},  {195., 30.},  {210., 30.},   //
        {225., 30.},  {240., 30.},  {255., 30.},  {270., 30.},  {285., 30.},   //
        {300., 30.},  {315., 30.},  {330., 30.},  {345., 30.},                 //
        {0., -30.},   {15., -30.},  {30., -30.},  {45., -30.},  {60., -30.},   //
        {75., -30.},  {90., -30.},  {105., -30.}, {120., -30.}, {135., -30.},  //
        {150., -30.}, {165., -30.}, {180., -30.}, {195., -30.}, {210., -30.},  //
        {225., -30.}, {240., -30.}, {255., -30.}, {270., -30.}, {285., -30.},  //
        {300., -30.}, {315., -30.}, {330., -30.}, {345., -30.},                //
        {0., -90.},   {18., -90.},  {36., -90.},  {54., -90.},  {72., -90.},   //
        {90., -90.},  {108., -90.}, {126., -90.}, {144., -90.}, {162., -90.},  //
        {180., -90.}, {198., -90.}, {216., -90.}, {234., -90.}, {252., -90.},
        {270., -90.}, {288., -90.}, {306., -90.}, {324., -90.}, {342., -90.},
    };

    auto points = grid->to_points();

    EXPECT(points.size() == grid->size());
    ASSERT(points.size() == ref.size());

    auto it = grid->begin();
    for (size_t i = 0; i < points.size(); ++i) {
        EXPECT(points_equal(ref[i], points[i]));
        EXPECT(points_equal(ref[i], *it));
        ++it;
    }
    EXPECT(it == grid->end());

    size_t i = 0;
    for (const auto& it : *grid) {
        EXPECT(points_equal(ref[i++], it));
    }
    EXPECT(i == grid->size());
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
