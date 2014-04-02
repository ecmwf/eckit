#ifndef eckit_Point2_h
#define eckit_Point2_h

#include "eckit/geometry/KPoint.h"

//------------------------------------------------------------------------------------------------------

namespace eckit {
namespace geometry {

//------------------------------------------------------------------------------------------------------

class Point2 : public eckit::geometry::KPoint<2> {

    typedef KPoint<2> BasePoint;

public:

    Point2(): BasePoint() {}

    Point2( const BasePoint& p ): BasePoint(p) {}

    double  operator[] (const size_t& i) const { assert(i<2); return x_[i]; }
    double& operator[] (const size_t& i)       { assert(i<2); return x_[i]; }

    Point2( const double* p ): KPoint<2>(p) {}

    Point2( double x, double y ) : BasePoint( NoInit() )
    {
        x_[XX] = x;
        x_[YY] = y;
    }

    template < typename T >
    void assign( const T& p )
    {
        x_[XX] = p[XX];
        x_[YY] = p[YY];
    }

    friend std::ostream& operator<<(std::ostream& s,const Point2& p);

};

//------------------------------------------------------------------------------------------------------

class LLPoint2 : public Point2 {
public:

    enum { LAT = XX, LON = YY };

    LLPoint2() : Point2() {}

    LLPoint2( const Point2& p ) : Point2(p) { reduceTo2Pi(); }

    LLPoint2( const double lat, const double lon ) : Point2(lat,lon) { reduceTo2Pi(); }

    double lat() const { return x_[LAT]; }
    double lon() const { return x_[LON]; }

private:

    void reduceTo2Pi()
    {
        while(x_[LON] < 0)    x_[LON] += 360;
        while(x_[LON] >= 360) x_[LON] -= 360;
    }

};

//------------------------------------------------------------------------------------------------------

/// @todo this could be generalized to n-dimensions with n-points

template < typename POINT >
struct BoundBox2
{
    BoundBox2(){}

    BoundBox2( const POINT& bottom_left, const POINT& top_right ) :
        bottom_left_(bottom_left),
        top_right_(top_right)
    {
        ASSERT( bottom_left_(XX) <= top_right_(XX) );
        ASSERT( bottom_left_(YY) <= top_right_(YY) );
    }

    POINT bottom_left_;
    POINT top_right_;

};

//------------------------------------------------------------------------------------------------------

bool points_equal( const Point2& a, const Point2& b );

//---------------------------------------------------------------------------------------------------------

} // namespace geometry
} // namespace eckit

#endif

