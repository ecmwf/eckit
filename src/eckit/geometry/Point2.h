#ifndef eckit_Point2_h
#define eckit_Point2_h

#include "eckit/value/Value.h"
#include "eckit/geometry/KPoint.h"

//------------------------------------------------------------------------------------------------------

namespace eckit {
namespace geometry {

//------------------------------------------------------------------------------------------------------

void reduceTo2Pi( double& d );

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

    operator eckit::Value() const;

    friend std::ostream& operator<<(std::ostream& s,const Point2& p);

};

//------------------------------------------------------------------------------------------------------

class LLPoint2 : public Point2 {
public:

    LLPoint2() : Point2() {}

    LLPoint2( const Point2& p ) : Point2(p) { reduceTo2Pi(x_[LON]); }

    LLPoint2( double lon, double lat ) : Point2(lon,lat) { reduceTo2Pi(x_[LON]); }

    double lat() const { return x_[LAT]; }
    double lon() const { return x_[LON]; }

    LLPoint2& lon(double lon) { x_[LON] = lon; reduceTo2Pi(x_[LON]); return *this; }
    LLPoint2& lat(double lat) { x_[LAT] = lat; return *this; }

    void assign( double lon, double lat )
    {
        x_[LON] = lon;
        x_[LAT] = lat;
        reduceTo2Pi(x_[LON]);
    }

    bool operator <(const LLPoint2& rhs) const {
       return lon() < rhs.lon() || lat() < rhs.lat();
    }
    bool operator >(const LLPoint2& rhs) const {
       return lon() > rhs.lon() || lat() > rhs.lat();
    }

    operator eckit::Value() const;
};

//------------------------------------------------------------------------------------------------------

bool points_equal( const Point2& a, const Point2& b );

//---------------------------------------------------------------------------------------------------------

} // namespace geometry
} // namespace eckit

#endif

