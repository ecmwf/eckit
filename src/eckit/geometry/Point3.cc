#include "eckit/geometry/Point3.h"

#include "eckit/types/FloatCompare.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {
namespace geometry {

//----------------------------------------------------------------------------------------------------------------------

bool points_equal(const Point3& a, const Point3& b) {
    return eckit::types::is_approximately_equal<double>(Point3::distance2(a, b), 0.0);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace geometry
}  // namespace eckit
