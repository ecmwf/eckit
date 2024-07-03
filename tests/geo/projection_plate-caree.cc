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

#include "eckit/geo/Projection.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


using P = std::unique_ptr<Projection>;


CASE("projection: plate-caree") {
    Point p = PointLonLat{1, 1};
    Point q = Point2{1, 1};
    P projection(ProjectionFactory::instance().get("plate-carree").create(spec::Custom{}));

    EXPECT(points_equal(q, projection->inv(p)));
    EXPECT(std::holds_alternative<Point2>(projection->inv(p)));

    EXPECT(points_equal(p, projection->fwd(q)));
    EXPECT(std::holds_alternative<PointLonLat>(projection->fwd(q)));
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
