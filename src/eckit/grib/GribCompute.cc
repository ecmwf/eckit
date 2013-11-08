// File GribCompute.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "GribCompute.h"
#include "GribField.h"

namespace eckit {
namespace compute {

GribFieldSet merge(const GribFieldSet& a, const GribFieldSet& b) {
     GribFieldSet out(a.count() + b.count());
     for(size_t j = 0; j < a.count(); ++j) {
        out.add(a.willAdopt(j));
     }
     for(size_t j = 0; j < b.count(); ++j) {
        out.add(b.willAdopt(j));
     }
     return out;
}


GribFieldSet mean(const GribFieldSet& in)
{

    GribFieldSet out(1);

    size_t nfields = in.count();

    Log::info() << "mean(" << nfields << ")" << endl;

    ASSERT(nfields) ;

    size_t npoints = 0;
    GribField* first = in.willAdopt(0);
    const double* values = first->getValues(npoints);
    double* result = new double[npoints];
    ASSERT(result);

    std::copy(values, values + npoints, result);

    for(size_t j = 1; j < nfields; ++j) {
        size_t n = 0;
        const GribField* first = in.get(0);
        const double* values = first->getValues(n);
        ASSERT(n == npoints);
        for(size_t i = 0; i < npoints; ++i) {
            result[i] += values[i];
        }
    }

    for(size_t i = 0; i < npoints; ++i) {
        result[i] /= npoints;
    }

    out.add(new GribField(first, result, npoints));

    return out;
}

template<class OPERATOR>
double minmaxvalue(const GribFieldSet& in)
{
    size_t nfields = in.count();

    ASSERT(nfields) ;

    double result;

    for(size_t j = 0; j < nfields; ++j) {
        size_t npoints = 0;
        const GribField* f = in.get(j);
        const double* values = f->getValues(npoints);
        ASSERT(npoints);
        if(j == 0) {
            result = values[0];
        }

        for(size_t i = 0; i < npoints; ++i) {
            if( OPERATOR()(values[i],result)) {
                result = values[i];
            }
        }
    }

    return result;

}

double minvalue(const GribFieldSet& in)
{
    return minmaxvalue<std::less<double> >(in);
}

double maxvalue(const GribFieldSet& in)
{
    return minmaxvalue<std::greater<double> >(in);
}

} // namespace compute
} // namespace eckit
