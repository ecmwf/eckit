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

#include "eckit/geo/grid/reduced/HEALPix.h"
#include "eckit/testing/Test.h"


namespace eckit::test {


CASE("HEALPix") {
    SECTION("HEALPix::to_points") {
        std::unique_ptr<const geo::Grid> grid(new geo::grid::reduced::HEALPix(2, geo::Ordering::healpix_ring));

        static const std::vector<geo::PointLonLat> expected_points_ring{
            {45, 66.443536},     {135, 66.443536},    {225, 66.443536},    {315, 66.443536},    {22.5, 41.810315},
            {67.5, 41.810315},   {112.5, 41.810315},  {157.5, 41.810315},  {202.5, 41.810315},  {247.5, 41.810315},
            {292.5, 41.810315},  {337.5, 41.810315},  {0, 19.471221},      {45, 19.471221},     {90, 19.471221},
            {135, 19.471221},    {180, 19.471221},    {225, 19.471221},    {270, 19.471221},    {315, 19.471221},
            {22.5, 0},           {67.5, 0},           {112.5, 0},          {157.5, 0},          {202.5, 0},
            {247.5, 0},          {292.5, 0},          {337.5, 0},          {0, -19.471221},     {45, -19.471221},
            {90, -19.471221},    {135, -19.471221},   {180, -19.471221},   {225, -19.471221},   {270, -19.471221},
            {315, -19.471221},   {22.5, -41.810315},  {67.5, -41.810315},  {112.5, -41.810315}, {157.5, -41.810315},
            {202.5, -41.810315}, {247.5, -41.810315}, {292.5, -41.810315}, {337.5, -41.810315}, {45, -66.443536},
            {135, -66.443536},   {225, -66.443536},   {315, -66.443536},
        };

        auto points = grid->to_points();
        EXPECT_EQUAL(points.size(), expected_points_ring.size());

        for (int i = 0; i < points.size(); ++i) {
            EXPECT(geo::points_equal(std::get<geo::PointLonLat>(points[i]), expected_points_ring[i]));
        }
    }
}


}  // namespace eckit::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
