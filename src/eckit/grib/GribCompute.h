// File GribCompute.h
// Baudouin Raoult - (c) ECMWF Nov 13

#ifndef GribCompute_H
#define GribCompute_H

#include "GribFieldSet.h"
#include "GribField.h"

namespace eckit {
namespace compute {

//====================================================================

inline const char *opname(const negate<double>&)         { return "-";}
inline const char *opname(const multiplies<double>&)     { return "*";}
inline const char *opname(const divides<double>&)        { return "/";}
inline const char *opname(const plus<double>&)           { return "+";}
inline const char *opname(const minus<double>&)          { return "-";}
inline const char *opname(const greater<double>&)        { return ">";}
inline const char *opname(const equal_to<double>&)       { return "==";}
inline const char *opname(const less<double>&)           { return "<";}
inline const char *opname(const greater_equal<double>&)  { return ">=";}
inline const char *opname(const less_equal<double>&)     { return "<=";}
inline const char *opname(const not_equal_to<double>&)   { return "!=";}
inline const char *opname(const logical_not<double>&)    { return "!";}
inline const char *opname(const logical_and<double>&)    { return "&&";}
inline const char *opname(const logical_or<double>&)     { return "||";}

//====================================================================


void mean(const GribFieldSet& in, GribFieldSet& out);

template<class OPERATOR>
void unop(const GribFieldSet& in, GribFieldSet& out) {

    Log::info() << "unop(" << opname(OPERATOR()) << ")" << endl;

    size_t nfields = in.count();


    ASSERT(out.count() == 0);

    if(!nfields) {
        return;
    }


    for(size_t j = 0; j < nfields; ++j) {
        GribField* f = in.willAdopt(j);

        size_t n;
        const double* v = f->getValues(n);

        double* result = new double[n];
        ASSERT(result);

        for(size_t i = 0; i < n; ++i) {
            result[i] = OPERATOR()(v[i]);
        }

        out.add(new GribField(f, result, n));

    }

}

template<class OPERATOR>
void binop(const GribFieldSet& a, const GribFieldSet& b, GribFieldSet& out) {

    Log::info() << "binop(" << opname(OPERATOR()) << ")" << endl;

    size_t nfields = a.count();
    ASSERT(b.count() == nfields);


    ASSERT(out.count() == 0);

    if(!nfields) {
        return;
    }


    for(size_t j = 0; j < nfields; ++j) {
        GribField* fa = a.willAdopt(j);
        const GribField* fb = b.get(j);

        size_t na, nb;
        const double* va = fa->getValues(na);
        const double* vb = fb->getValues(nb);

        ASSERT(na == nb);

        double* result = new double[na];
        ASSERT(result);

        for(size_t i = 0; i < na; ++i) {
            result[i] = OPERATOR()(va[i], vb[i]);
        }

        out.add(new GribField(fa, result, na));

    }

}

void multiplies(const GribFieldSet& a, const GribFieldSet& b, GribFieldSet& out) {
    binop<std::multiplies<double> >(a, b, out);
}

void divides(const GribFieldSet& a, const GribFieldSet& b, GribFieldSet& out) {
    binop<std::divides<double> >(a, b, out);
}

void plus(const GribFieldSet& a, const GribFieldSet& b, GribFieldSet& out) {
    binop<std::plus<double> >(a, b, out);
}

void minus(const GribFieldSet& a, const GribFieldSet& b, GribFieldSet& out) {
    binop<std::minus<double> >(a, b, out);
}

void greater(const GribFieldSet& a, const GribFieldSet& b, GribFieldSet& out) {
    binop<std::greater<double> >(a, b, out);
}

void equal_to(const GribFieldSet& a, const GribFieldSet& b, GribFieldSet& out) {
    binop<std::equal_to<double> >(a, b, out);
}

void less(const GribFieldSet& a, const GribFieldSet& b, GribFieldSet& out) {
    binop<std::less<double> >(a, b, out);
}

void greater_equal(const GribFieldSet& a, const GribFieldSet& b, GribFieldSet& out) {
    binop<std::greater_equal<double> >(a, b, out);
}

void not_equal_to(const GribFieldSet& a, const GribFieldSet& b, GribFieldSet& out) {
    binop<std::not_equal_to<double> >(a, b, out);
}

void less_equal(const GribFieldSet& a, const GribFieldSet& b, GribFieldSet& out) {
    binop<std::less_equal<double> >(a, b, out);
}

void logical_and(const GribFieldSet& a, const GribFieldSet& b, GribFieldSet& out) {
    binop<std::logical_and<double> >(a, b, out);
}

void logical_or(const GribFieldSet& a, const GribFieldSet& b, GribFieldSet& out) {
    binop<std::logical_or<double> >(a, b, out);
}

void negate(const GribFieldSet& in, GribFieldSet& out) {
    unop<std::negate<double> >(in, out);
}

void logical_not(const GribFieldSet& in, GribFieldSet& out) {
    unop<std::logical_not<double> >(in, out);
}


} // namespace
} // namespace

#endif
