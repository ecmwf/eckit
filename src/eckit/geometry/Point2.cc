#include <vector>

#include "eckit/geometry/Point2.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/value/Value.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {
namespace geometry {

//----------------------------------------------------------------------------------------------------------------------

bool points_equal(const Point2& a, const Point2& b) {
    return eckit::types::is_approximately_equal<double>(Point2::distance2(a, b), 0.0);
}

Point2::operator Value() const {
    std::vector<Value> pts;
    pts.push_back(x_[XX]);
    pts.push_back(x_[YY]);
    return Value::makeList(pts);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace geometry
}  // namespace eckit
