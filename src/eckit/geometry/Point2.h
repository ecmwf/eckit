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

	LLPoint2( double lat, double lon ) : Point2(lat,lon) { reduceTo2Pi(x_[LON]); }

    double lat() const { return x_[LAT]; }
    double lon() const { return x_[LON]; }

    void assign( double lat, double lon )
    {
        x_[LAT] = lat;
        x_[LON] = lon;
		reduceTo2Pi(x_[LON]);
    }

    operator eckit::Value() const;

};

//------------------------------------------------------------------------------------------------------

class LLBoundBox2 {

public: // methods

    LLBoundBox2() :
    bl_( 0., 0. ),
    tr_( 0., 0. )
    {}

    LLBoundBox2( double top, double bottom, double right, double left ) :
        bl_( bottom, left ),
        tr_( top, right )
    {
        ASSERT( validate() );
    }

    LLBoundBox2( const LLPoint2& bottom_left, const LLPoint2& top_right ) :
        bl_(bottom_left),
        tr_(top_right)
    {
        ASSERT( validate() );
    }

    static LLBoundBox2 make( const eckit::Value& v )
    {
        ASSERT( v.isList() );

        ValueList pts = v;
        ValueList p0 = pts[0];
        ValueList p1 = pts[1];

        return LLBoundBox2( LLPoint2(p0[0],p0[1]), LLPoint2(p1[0],p1[1]) );
    }

    bool operator==(const LLBoundBox2& rhs) const
    {
       return (bl_ == rhs.bl_) && (tr_ == rhs.tr_);
    }

    bool operator!=(const LLBoundBox2& rhs) const
    {
       return !operator==(rhs);
    }

    operator eckit::Value() const
    {
        std::vector<Value> pts;
        pts.push_back(bl_);
        pts.push_back(tr_);
        return Value::makeList(pts);
    }

    bool validate() const
    {
        return ( bl_(LAT) <= tr_(LAT) ) && ( bl_(LON) <= tr_(LON) ) && ( area() > 0 );
    }

    LLPoint2 bottom_left() const { return bl_; }
    LLPoint2 top_right()   const { return tr_;   }

    double area() const { return ( tr_[LON] - bl_[LON] ) * ( tr_[LAT] - bl_[LAT] ); }

    bool empty() const { return ( area() == 0. ); }

    void print( std::ostream& out ) const
    {
        out << "BoundBox2( " << bl_ << "," << tr_ << ")";
    }

    friend std::ostream& operator<<(std::ostream& s,const LLBoundBox2& o)
    {
        o.print(s); return s;
    }

    double east()  const { return tr_.lon(); }
    double north() const { return tr_.lat(); }
    double west()  const { return bl_.lon(); }
    double south() const { return bl_.lat(); }

private: // members

    LLPoint2 bl_;
    LLPoint2 tr_;

};

//------------------------------------------------------------------------------------------------------

bool points_equal( const Point2& a, const Point2& b );

//---------------------------------------------------------------------------------------------------------

} // namespace geometry
} // namespace eckit

#endif

