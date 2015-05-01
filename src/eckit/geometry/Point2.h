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

class LLBoundBox2 {

public: // methods

    LLBoundBox2() :
    min_( 0., 0. ),
    max_( 0., 0. )
    {}

    LLBoundBox2( double __north, double __south, double __east, double __west ) :
        min_( __west, __south ),
        max_( __east, __north )
    {
      if( !validate() )
      {
        std::stringstream msg;
        msg << "Invalid bounding box: N,S,E,W = " << north() << ", " << south() << ", " << east() << ", " << west();
        throw eckit::BadParameter(msg.str(),Here());
      }
    }

    LLBoundBox2( const LLPoint2& min, const LLPoint2& max ) :
        min_(min),
        max_(max)
    {
      if( !validate() )
      {
        std::stringstream msg;
        msg << "Invalid bounding box: N,S,E,W = " << north() << ", " << south() << ", " << east() << ", " << west();
        throw eckit::BadParameter(msg.str(),Here());
      }
    }

    LLBoundBox2( LLPoint2 pt, double x_delta, double y_delta )
    {
       LLPoint2 pt2(pt.lon() + x_delta,pt.lat() + y_delta);
       min_ = std::min(pt,pt2);
       max_ = std::max(pt,pt2);

       if( !validate() )
       {
          std::stringstream msg;
          msg << "Invalid bounding box: N,S,E,W = " << north() << ", " << south() << ", " << east() << ", " << west();
          throw eckit::BadParameter(msg.str(),Here());
       }
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
       return (min_ == rhs.min_) && (max_ == rhs.max_);
    }

    bool operator!=(const LLBoundBox2& rhs) const
    {
       return !operator==(rhs);
    }

    operator eckit::Value() const
    {
        std::vector<Value> pts;
        pts.push_back(min_);
        pts.push_back(max_);
        return Value::makeList(pts);
    }

    bool validate() const
    {
        // For grib1[-180->+180], describing areas which overlaps over +180 or -180,
        // we can assume min_(LON) <= max_(LON) ?
        // return ( min_(LAT) <= max_(LAT) ) && ( min_(LON) <= max_(LON) ) && ( area() > 0 );
        return ( (min_(LAT) <= max_(LAT)) && (area() > 0) );
    }

    LLPoint2 min() const { return min_; }
    LLPoint2 max() const { return max_;   }

    double area() const { return fabs(( max_[LON] - min_[LON] ) * ( max_[LAT] - min_[LAT] )); }

    bool empty() const { return ( area() == 0. ); }

    void print( std::ostream& out ) const
    {
        out << "BoundBox2( " << min_ << "," << max_ << ")";
    }

    friend std::ostream& operator<<(std::ostream& s,const LLBoundBox2& o)
    {
        o.print(s); return s;
    }

    double east()  const { return max_.lon(); }
    double north() const { return max_.lat(); }
    double west()  const { return min_.lon(); }
    double south() const { return min_.lat(); }

private: // members

    LLPoint2 min_;
    LLPoint2 max_;

};

//------------------------------------------------------------------------------------------------------

bool points_equal( const Point2& a, const Point2& b );

//---------------------------------------------------------------------------------------------------------

} // namespace geometry
} // namespace eckit

#endif

