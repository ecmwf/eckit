/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/xpr/Accumulate.h"
#include "eckit/xpr/Real.h"
#include "eckit/xpr/Vector.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

struct Mean {};

static const char *opname(const Add& ) { return "xpr::Sum"; }
static const char *opname(const Mul& ) { return "xpr::Product"; }
static const char *opname(const Mean&) { return "xpr::Mean"; }

static const char *opfactory(const Add& ) { return "xpr::sum"; }
static const char *opfactory(const Mul& ) { return "xpr::product"; }
static const char *opfactory(const Mean&) { return "xpr::mean"; }

static const real_t opinit(const Add&) { return 0.; }
static const real_t opinit(const Mul&) { return 1.; }

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

//--------------------------------------------------------------------------------------------

// explicit template instantiations

template class Accumulate<Add>;
template class Accumulate<Mul>;
template class Accumulate<Mean>;

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

struct AccumulateMeanComputer
{
    AccumulateMeanComputer() {
        Function::dispatcher()["xpr::Mean(v)"] = &compute;
    }

    static ExpPtr compute(Scope&, const args_t &p)
    {
        Vector::value_t v = Vector::extract(p[0]);
        return real( std::accumulate(v.begin(), v.end(), 0.) / v.size() );
    }
};

static AccumulateMeanComputer mean_v;

//-----------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
