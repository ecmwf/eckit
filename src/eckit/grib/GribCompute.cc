// File GribCompute.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "eckit/utils/Timer.h"
#include "GribCompute.h"
#include "GribField.h"

namespace eckit {
namespace compute {

GribFieldSet merge(const GribFieldSet& a, const GribFieldSet& b) {
    GribFieldSet out(a.size() + b.size());
    for(size_t j = 0; j < a.size(); ++j) {
        out.add(a.willAdopt(j));
    }
    for(size_t j = 0; j < b.size(); ++j) {
        out.add(b.willAdopt(j));
    }
    return out;
}


GribFieldSet mean(const GribFieldSet& in)
{

    GribFieldSet out(1);

    size_t nfields = in.size();

    ASSERT(nfields) ;

    size_t npoints = 0;
    GribField* header = in.willAdopt(0);

    const double* values = header->getValues(npoints);
    double* result = new double[npoints];
    ASSERT(result);

    for(size_t i = 0; i < npoints; ++i) {
        result[i] = values[i];
    }

    header->release();



    for(size_t j = 1; j < nfields; ++j) {
        size_t n = 0;
        const GribField* f = in.get(j);

        values = f->getValues(n);
        ASSERT(n == npoints);

        for(size_t i = 0; i < npoints; ++i) {
            result[i] += values[i];
        }

        f->release();
    }

    double xfields = nfields;
    for(size_t i = 0; i < npoints; ++i) {
        result[i] /= xfields;
    }

    out.add(new GribField(header, result, npoints));

    return out;
}

template<class OPERATOR>
double minmaxvalue(const GribFieldSet& in)
{
    size_t nfields = in.size();

    ASSERT(nfields);

    double result;
    size_t npoints;

    const GribField* f = in.get(0);
    const double* values = f->getValues(npoints);
    ASSERT(npoints);
    result = values[0];

    for(size_t j = 0; j < nfields; ++j) {
        f = in.get(j);
        values = f->getValues(npoints);

        for(size_t i = 0; i < npoints; ++i) {
            if( OPERATOR()(values[i],result)) {
                result = values[i];
            }
        }
        f->release();
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

double accumulate(const GribFieldSet& in)
{
    size_t nfields = in.size();

    ASSERT(nfields);

    double result;
    size_t npoints;

    result = 0;

    for(size_t j = 0; j < nfields; ++j) {
        const GribField *f = in.get(j);
        const double* values = f->getValues(npoints);

        for(size_t i = 0; i < npoints; ++i) {
            result += values[i];
        }
        f->release();
    }

    return result;

}

double dot(const GribFieldSet& a, const GribFieldSet& b)
{
    ASSERT(a.size() == 1 && b.size() == 1);

    size_t na, nb;

    const GribField *fa = a.get(0);
    const double* va = fa->getValues(na);

    const GribField *fb = b.get(0);
    const double* vb = fb->getValues(nb);

    ASSERT(na == nb);

    double result = 0;


    result = 0;

    for(size_t i = 0; i < na; ++i) {
        result += va[i]*vb[i];
    }

    fa->release();
    fb->release();

    return result;

}

Matrix kernel(const GribFieldSet& in)
{
    Timer timer("kernel");
    size_t nfields = in.size();
    Matrix result(nfields, nfields);

    size_t na, nb;

    for(size_t i = 0; i < nfields; ++i) {
        const GribField *fa = in.get(i);
        const double* va = fa->getValues(na);


        for(size_t j = 0; j <= i; ++j) {
            const GribField *fb = in.get(j);
            const double* vb = fb->getValues(nb);
            ASSERT(na == nb);
            double dot = 0;

            for(size_t k = 0; k < na; ++k) {
                dot += va[k]*vb[k];
            }

            result(i,j) = result(j,i) = dot;
            fb->release();
        }

        if((i%1000)==0)
        cout << i << endl;
    }

    return result;

}


} // namespace compute
} // namespace eckit
