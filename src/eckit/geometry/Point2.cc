#include "eckit/geometry/Point2.h"
#include "eckit/types/FloatCompare.h"

//------------------------------------------------------------------------------------------------------

namespace eckit {
namespace geometry {

//------------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& s,const Point2& p)
{
    s << '(' << p.x_[XX] << ","
             << p.x_[YY] << ")";
    return s;
}

//------------------------------------------------------------------------------------------------------

bool points_equal(const Point2 &a, const Point2 &b)
{
    return FloatCompare::is_equal( Point2::distance2(a,b), 0.0 );
}

//------------------------------------------------------------------------------------------------------

} // namespace geometry
} // namespace eckit

