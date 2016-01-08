#ifndef eckit_Domain_h
#define eckit_Domain_h

#include "eckit/geometry/Point2.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace geometry {

// Domain defines a abstract base class, used for cropping, masking, etc
//
class Domain {
public:

   virtual ~Domain();

   virtual bool is_global() const = 0;
   virtual bool has_poles() const = 0;
};


class Globe : public Domain {
public:
   Globe() {}

   virtual bool is_global() const { return true;}
   virtual bool has_poles() const { return true; }
};


class BoxLL : public Domain {
public:
   BoxLL() {}
   BoxLL( double north, double south, double east, double west ) : bbox_(north,south,east,west) {}
   BoxLL( const LLPoint2& min, const LLPoint2& max ) : bbox_(min,max) {}

   virtual bool is_global() const;
   virtual bool has_poles() const ;

   bool operator==(const BoxLL& rhs) const
   {
      return (bbox_ == rhs.bbox_) ;
   }

   bool operator!=(const BoxLL& rhs) const
   {
      return !operator==(rhs);
   }

   bool validate() const
   {
       return bbox_.validate();
   }

   LLPoint2 min() const { return bbox_.min(); }
   LLPoint2 max() const { return bbox_.max();  }

   double area() const { return  bbox_.area(); }

   bool empty() const { return ( area() == 0. ); }

   void print( std::ostream& out ) const
   {
       out << "BoxLL( " << bbox_ << ")";
   }

   friend std::ostream& operator<<(std::ostream& s,const BoxLL& o)
   {
       o.print(s); return s;
   }

   double east()  const { return bbox_.east(); }
   double north() const { return bbox_.north(); }
   double west()  const { return bbox_.west(); }
   double south() const { return bbox_.south(); }

private:
   LLBoundBox2 bbox_;
};


//---------------------------------------------------------------------------------------------------------

} // namespace geometry
} // namespace eckit

#endif
