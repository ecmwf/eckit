#include <vector>

#include "eckit/geometry/Domain.h"

//------------------------------------------------------------------------------------------------------

namespace eckit {
namespace geometry {

//------------------------------------------------------------------------------------------------------

Domain::~Domain() {}


//------------------------------------------------------------------------------------------------------

bool BoxLL::is_global() const
{
   // need to determine this
   return false;
}

bool BoxLL::has_poles() const
{
   // need to determine this
   return false;
}


//------------------------------------------------------------------------------------------------------

} // namespace geometry
} // namespace eckit
