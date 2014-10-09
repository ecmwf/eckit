#include <vector>

#include "eckit/geometry/Point2.h"
#include "eckit/types/FloatCompare.h"

//------------------------------------------------------------------------------------------------------

namespace eckit {
namespace geometry {

//------------------------------------------------------------------------------------------------------

void reduceTo2Pi( double& d )
{
	while( d < 0 )   d += 360;
	while( d > 360 ) d -= 360;
}

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

Point2::operator Value() const
{
    std::vector<Value> pts;
    pts.push_back(x_[XX]);
    pts.push_back(x_[YY]);
    return Value::makeList(pts);
}

LLPoint2::operator Value() const
{
    std::vector<Value> pts;
    pts.push_back(lat());
    pts.push_back(lon());
    return Value::makeList(pts);
}

//------------------------------------------------------------------------------------------------------

} // namespace geometry
} // namespace eckit

