/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include <cmath>

#include "eckit/xpr/Accumulate.h"
#include "eckit/xpr/Real.h"
#include "eckit/xpr/Vector.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

struct Mean {};
struct Variance {};
struct StdDev {};

static const char *opname(const Add& )     { return "xpr::Sum"; }
static const char *opname(const Mul& )     { return "xpr::Product"; }
static const char *opname(const Mean&)     { return "xpr::Mean"; }
static const char *opname(const Variance&) { return "xpr::Variance"; }
static const char *opname(const StdDev&)   { return "xpr::StdDev"; }

static const char *opfactory(const Add& )     { return "xpr::sum"; }
static const char *opfactory(const Mul& )     { return "xpr::product"; }
static const char *opfactory(const Mean&)     { return "xpr::mean"; }
static const char *opfactory(const Variance&) { return "xpr::variance"; }
static const char *opfactory(const StdDev&)   { return "xpr::stddev"; }

static real_t opinit(const Add&) { return 0.; }
static real_t opinit(const Mul&) { return 1.; }

//-----------------------------------------------------------------------------

template < class T >
Accumulate<T>::Accumulate(ExpPtr v) : Function()
{
    push_back(v);
}

template < class T >
Accumulate<T>::Accumulate( args_t& a ) : Function(a)
{
    ASSERT( a.size() == 1 );
}

template < class T >
Accumulate<T>::Accumulate(Stream &s) : Function(s) {}

template < class T >
const ClassSpec& Accumulate<T>::classSpec()
{
     static ClassSpec myClassSpec = {
         &Function::classSpec(),
         Accumulate<T>::nodeName(),
     };
     return myClassSpec;
}

template < class T >
std::string Accumulate<T>::factoryName() const
{
    return opfactory( T() );
}

template < class T >
const char * Accumulate<T>::typeName() const
{
    return Accumulate<T>::nodeName();
}

template < class T >
const char * Accumulate<T>::nodeName()
{
    return opname( T() );
}

//-----------------------------------------------------------------------------

template < class T >
Reanimator< Accumulate<T> > Accumulate<T>::reanimator_;

//-----------------------------------------------------------------------------

ExpPtr sum( ExpPtr v )
{
    return ExpPtr( new Accumulate<Add>(v) );
}

ExpPtr product( ExpPtr v )
{
    return ExpPtr( new Accumulate<Mul>(v) );
}

ExpPtr mean( ExpPtr v )
{
    return ExpPtr( new Accumulate<Mean>(v) );
}

ExpPtr variance( ExpPtr v )
{
    return ExpPtr( new Accumulate<Variance>(v) );
}

ExpPtr stddev( ExpPtr v )
{
    return ExpPtr( new Accumulate<StdDev>(v) );
}

//--------------------------------------------------------------------------------------------

// explicit template instantiations

template class Accumulate<Add>;
template class Accumulate<Mul>;
template class Accumulate<Mean>;
template class Accumulate<Variance>;
template class Accumulate<StdDev>;

//-----------------------------------------------------------------------------

template < class T >
struct AccumulateComputer
{
    AccumulateComputer() {
        Function::dispatcher()[opname( T() ) + std::string("(v)")] = &compute;
    }

    static ExpPtr compute(Scope&, const args_t &p)
    {
        Vector::value_t v = Vector::extract(p[0]);
        return real( std::accumulate(v.begin(), v.end(), opinit(T()), T()) );
    }
};

static AccumulateComputer<Add> add_v;
static AccumulateComputer<Mul> mul_v;

struct StatsComputer
{
    StatsComputer() {
        Function::dispatcher()["xpr::Mean(v)"]     = &compute_mean;
        Function::dispatcher()["xpr::Variance(v)"] = &compute_variance;
        Function::dispatcher()["xpr::StdDev(v)"]   = &compute_stddev;
    }

    static inline real_t mean(const Vector::value_t& v)
    {
        return std::accumulate(v.begin(), v.end(), 0.) / v.size();
    }

    static inline real_t variance(const Vector::value_t& v)
    {
        const real_t m = mean(v);
        real_t var = 0.;
        for(real_t e : v)
            var += (e - m) * (e - m);
        return var / v.size();
    }

    static inline real_t stddev(const Vector::value_t& v)
    {
        return std::sqrt(variance(v));
    }

    static ExpPtr compute_mean(Scope&, const args_t &p)
    {
        return real( mean(Vector::extract(p[0])) );
    }

    static ExpPtr compute_variance(Scope&, const args_t &p)
    {
        return real( variance(Vector::extract(p[0])) );
    }

    static ExpPtr compute_stddev(Scope&, const args_t &p)
    {
        return real( stddev(Vector::extract(p[0])) );
    }
};

static StatsComputer stats_v;

//-----------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
