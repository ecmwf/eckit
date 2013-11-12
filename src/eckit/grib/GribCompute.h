// File GribCompute.h
// Baudouin Raoult - (c) ECMWF Nov 13

#ifndef GribCompute_H
#define GribCompute_H

#include <cmath>

#include "GribFieldSet.h"
#include "GribField.h"
#include "Matrix.h"

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


GribFieldSet mean(const GribFieldSet& in);
double maxvalue(const GribFieldSet& in);
double minvalue(const GribFieldSet& in);

double accumulate(const GribFieldSet& in);

GribFieldSet normalise1(const GribFieldSet& in); // Between 0 and 1
GribFieldSet normalise2(const GribFieldSet& in); // With respect to average
GribFieldSet normalise3(const GribFieldSet& in); // With respect to average and stdev

GribFieldSet max(const GribFieldSet& in);
GribFieldSet min(const GribFieldSet& in);

GribFieldSet merge(const GribFieldSet& a, const GribFieldSet& b);

double dot(const GribFieldSet& a, const GribFieldSet& b);
Matrix kernel(const GribFieldSet& in);

template<class OPERATOR>
GribFieldSet unop(const GribFieldSet& in) {

    GribFieldSet out(in.size());

    size_t nfields = in.size();

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

    return out;

}

template<class OPERATOR>
GribFieldSet binop(const GribFieldSet& a, const GribFieldSet& b) {

    GribFieldSet out(a.size());

    size_t nfields = a.size();
    ASSERT(b.size() == nfields);

    ASSERT(out.size() == 0);

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

    return out;

}

template<class OPERATOR>
GribFieldSet binop(const GribFieldSet& a, const double& b) {

    GribFieldSet out(a.size());

    size_t nfields = a.size();

    for(size_t j = 0; j < nfields; ++j) {
        GribField* fa = a.willAdopt(j);

        size_t na;
        const double* va = fa->getValues(na);

        double* result = new double[na];
        ASSERT(result);

        for(size_t i = 0; i < na; ++i) {
            result[i] = OPERATOR()(va[i], b);
        }

        out.add(new GribField(fa, result, na));

    }
    return out;
}

template<class OPERATOR>
GribFieldSet binop(const double& b, const GribFieldSet& a) {
    GribFieldSet out(a.size());

//   Log::info() << "binop(" << opname(OPERATOR()) << ")" << endl;

    size_t nfields = a.size();

    for(size_t j = 0; j < nfields; ++j) {
        GribField* fa = a.willAdopt(j);

        size_t na, nb;
        const double* va = fa->getValues(na);

        ASSERT(na == nb);

        double* result = new double[na];
        ASSERT(result);

        for(size_t i = 0; i < na; ++i) {
            result[i] = OPERATOR()(b, va[i]);
        }

        out.add(new GribField(fa, result, na));

    }

    return out;
}


template<class A, class B>
A multiplies(const A& a, const B& b) {
    return binop<std::multiplies<double> >(a, b);
}

template<class A, class B>
A divides(const A& a, const B& b) {
    return binop<std::divides<double> >(a, b);
}

template<class A, class B>
A plus(const A& a, const B& b) {
    return binop<std::plus<double> >(a, b);
}

template<class A, class B>
A minus(const A& a, const B& b) {
    return binop<std::minus<double> >(a, b);
}

template<class A, class B>
A greater(const A& a, const B& b) {
    return binop<std::greater<double> >(a, b);
}

template<class A, class B>
A equal_to(const A& a, const B& b) {
    return binop<std::equal_to<double> >(a, b);
}

template<class A, class B>
void less(const A& a, const B& b) {
    return binop<std::less<double> >(a, b);
}

template<class A, class B>
A greater_equal(const A& a, const B& b) {
    return binop<std::greater_equal<double> >(a, b);
}

template<class A, class B>
A not_equal_to(const A& a, const B& b) {
    return binop<std::not_equal_to<double> >(a, b);
}

template<class A, class B>
A less_equal(const A& a, const B& b) {
    return binop<std::less_equal<double> >(a, b);
}

template<class A, class B>
A logical_and(const A& a, const B& b) {
   return  binop<std::logical_and<double> >(a, b);
}

template<class A, class B>
A logical_or(const A& a, const B& b) {
    return binop<std::logical_or<double> >(a, b);
}

template<class A>
A negate(const A& in, A& out) {
     return unop<std::negate<double> >(in);
}

template<class A>
A logical_not(const A& in, A& out) {
    return unop<std::logical_not<double> >(in);
}

GribFieldSet operator-(const GribFieldSet& a, double b) {
    return minus(a, b);
}

GribFieldSet operator-(const GribFieldSet& a, const GribFieldSet& b) {
    return minus(a, b);
}

GribFieldSet operator/(const GribFieldSet& a, double b) {
    return divides(a, b);
}

GribFieldSet operator*(const GribFieldSet& a, const GribFieldSet& b) {
    return multiplies(a, b);
}

struct sqrt_op {
    double operator()(double x) { return std::sqrt(x); }
};
GribFieldSet sqrt(const GribFieldSet& x) {
    return unop<sqrt_op>(x);
}

} // namespace
} // namespace

#endif
