// File SmallestSphere.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "eckit/exception/Exceptions.h"
#include "SmallestSphere.h"
#include <cmath>

namespace eckit {

SmallestSphere::SmallestSphere()
{
}

SmallestSphere::~SmallestSphere()
{

}

void SmallestSphere::print(ostream& os) const
{
    os << "SmallestSphere[]";
}

static double distance(const vector<double>& a, const vector<double>& b ) {
    return std::sqrt(std::inner_product(a.begin(), a.end(), b.begin(), 0));
}

void smallestSphere(const vector<vector<double> >& points) {
    ASSERT(points.size());
    size_t dims = points[0].size();
    ASSERT(dims >= 2);
    for(vector<vector<double> >::const_iterator j = points.begin(); j != points.end(); ++j) {
        ASSERT((*j).size() == dims);
    }


}

} // namespace
