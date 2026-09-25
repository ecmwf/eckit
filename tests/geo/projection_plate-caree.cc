// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <memory>
#include <variant>

#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


using P = std::unique_ptr<Projection>;


CASE("projection: plate-caree") {
    Point p = PointXY{1, 1};
    Point q = PointLonLat{1, 1};
    P projection(Factory<Projection>::instance().get("plate-carree").create(spec::Custom{}));

    EXPECT(points_equal(q, projection->inv(p)));
    EXPECT(std::holds_alternative<PointLonLat>(projection->inv(p)));

    EXPECT(points_equal(p, projection->fwd(q)));
    EXPECT(std::holds_alternative<PointXY>(projection->fwd(q)));
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
