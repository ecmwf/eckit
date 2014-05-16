#ifndef eckit_FloatCompare_h
#define eckit_FloatCompare_h

#include <limits>

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

/// @todo move this class into eckit

class FloatCompare {
public:

    static bool is_equal( float  a, float  b, float  epsilon = std::numeric_limits<float>::epsilon());
    static bool is_equal( double a, double b, double epsilon = std::numeric_limits<double>::epsilon() );

};

//---------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif

