#ifndef eckit_geometry_Point3_h
#define eckit_geometry_Point3_h

#include "eckit/geometry/KPoint.h"

//------------------------------------------------------------------------------------------------------

namespace eckit {
namespace geometry {

//------------------------------------------------------------------------------------------------------

class Point3 : public eckit::geometry::KPoint<3> {

    typedef KPoint<3> BasePoint;

public:

    Point3(): BasePoint() {}

    Point3( const BasePoint& p ) : BasePoint(p) {}

    Point3( const double* p ) : BasePoint(p) {}

    Point3( double x, double y, double z ) : BasePoint( NoInit() )
    {
        x_[XX] = x;
        x_[YY] = y;
        x_[ZZ] = z;
    }

    double  operator[] (const size_t& i) const { assert(i<3); return x_[i]; }
    double& operator[] (const size_t& i)       { assert(i<3); return x_[i]; }

    template < typename T >
    void assign( const T& p )
    {
        x_[XX] = p[XX];
        x_[YY] = p[YY];
        x_[ZZ] = p[ZZ];
    }

};

//------------------------------------------------------------------------------------------------------

bool points_equal( const Point3&, const Point3& );

//------------------------------------------------------------------------------------------------------

} // namespace geometry
} // namespace eckit

#endif

