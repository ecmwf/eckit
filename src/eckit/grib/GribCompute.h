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


GribFieldSet mean(const GribFieldSet& in);
double maxvalue(const GribFieldSet& in);
double minvalue(const GribFieldSet& in);

GribFieldSet merge(const GribFieldSet& a, const GribFieldSet& b);

template<class OPERATOR>
void unop(const double& in, double& out) {
    out = OPERATOR()(in);
}

template<class OPERATOR>
void binop(const double& a, const double& b, double& out) {
    out = OPERATOR()(a, b);
}

template<class OPERATOR>
GribFieldSet unop(const GribFieldSet& in) {

    GribFieldSet out(in.count());

//    Log::info() << "unop(" << opname(OPERATOR()) << ")" << endl;

    size_t nfields = in.count();

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

    GribFieldSet out(a.count());

 //   Log::info() << "binop(" << opname(OPERATOR()) << ")" << endl;

    size_t nfields = a.count();
    ASSERT(b.count() == nfields);

    ASSERT(out.count() == 0);

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

    GribFieldSet out(a.count());

//    Log::info() << "binop(" << opname(OPERATOR()) << ")" << endl;

    size_t nfields = a.count();

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
    GribFieldSet out(a.count());

//   Log::info() << "binop(" << opname(OPERATOR()) << ")" << endl;

    size_t nfields = a.count();

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

GribFieldSet operator/(const GribFieldSet& a, double b) {
    return divides(a, b);
}

} // namespace
} // namespace

#endif
