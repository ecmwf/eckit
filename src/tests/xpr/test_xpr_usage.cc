/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath> // ceil()

#include "eckit/eckit_config.h"

#define BOOST_TEST_MODULE test_eckit_xpr

#include "ecbuild/boost_test_framework.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/utils/Translator.h"

#include "eckit/xpr/BinaryOperator.h"
#include "eckit/xpr/BinaryPredicate.h"
#include "eckit/xpr/Count.h"
#include "eckit/xpr/Expression.h"
#include "eckit/xpr/Map.h"
#include "eckit/xpr/Filter.h"
#include "eckit/xpr/Reduce.h"
#include "eckit/xpr/List.h"
#include "eckit/xpr/Value.h"
#include "eckit/xpr/Scalar.h"
#include "eckit/xpr/UnaryOperator.h"
#include "eckit/xpr/Vector.h"
#include "eckit/xpr/ZipWith.h"
#include "eckit/xpr/Bind.h"

using namespace std;
using namespace eckit;
using namespace eckit::xpr;

namespace eckit_test {

//-----------------------------------------------------------------------------

class F : public eckit::xpr::Value  {

public: // methods

    typedef std::vector<double> value_t;

    static std::string sig() { return "f"; }
    static std::string className() { return "F"; }
    
    static bool is ( const ExpPtr& e ) { return e->typeName() == F::className(); }

    F( const string& n, const double& v ) : n_(n)
    {
        f_.resize(32,v);
    }

    F( const string& n, const std::vector<double>& v ) : n_(n), f_(v) {}

    const std::string& fname() const { return n_; }

    const std::vector<double> fvec() const { return f_; }

public: // virtual methods

    virtual std::string typeName() const { return F::className(); }
    virtual std::string signature() const { return F::sig(); }
    virtual std::string returnSignature() const { return F::sig(); }

    virtual void print( std::ostream& o ) const
    {
        o << "F(" << fname() << ",";
        const F::value_t& v = fvec();
        for( size_t i = 0; i < v.size(); ++i )
            ( i != v.size()-1 ) ? o << v[i] << "," : o << v[i];
        o << ")" << std::endl;
    }

    virtual void asCode( std::ostream& o ) const { NOTIMP; }
    virtual ExpPtr cloneWith(args_t& a) const { NOTIMP; }

private:

    std::string n_;
    value_t f_;

};

ExpPtr fld( const string& n, const double& v) { return ExpPtr( new F(n,v) ); }

//-----------------------------------------------------------------------------

/// Creates a list of Fields

class FSrc : public eckit::xpr::Function {
public: // methods

    static std::string sig() { return "fsrc"; }
    static std::string className() { return "FSrc"; }

    FSrc( const ExpPtr& e ) : Function()
    {
        push_back(e);
    }

    FSrc( const args_t& args ) : Function(args)
    {

        ASSERT( args.size() == 1 );
    }

public: // virtual methods

    virtual std::string typeName() const { return FSrc::className(); }
    virtual std::string signature() const { return FSrc::sig(); }
    virtual std::string returnSignature() const { NOTIMP; }

    virtual void print( std::ostream& o ) const { o << "FSrc("; printArgs(o); o << ")"; }
    virtual void asCode( std::ostream& o ) const { NOTIMP; }
    virtual ExpPtr cloneWith(args_t& a) const { NOTIMP; }

    virtual ExpPtr evaluate( Scope& ctx ) const
    {
        ExpPtr s = args(0, ctx, true);

        long n = (long) ::ceil( Scalar::extract( s ) );

        ExpPtr lst = xpr::list();

        for( long i = 0; i < n; ++i )
        {
            std::string fn = "n" + Translator<long,std::string>()(i);

            std::cout << "creating " << fn << std::endl;

            lst = xpr::list( lst, fld( fn, (double) i ) );
        }

        return lst;
    }

};

ExpPtr fsrc( const long& n ) { return ExpPtr( new FSrc( xpr::scalar(n) ) ); }

//-----------------------------------------------------------------------------

/// Outputs fields (accepts isolated or list of fields )

class FSnk : public eckit::xpr::Function {
public: // methods

    static std::string sig() { return "fsnk"; }
    static std::string className() { return "FSnk"; }

    FSnk( const ExpPtr& e ) : Function()
    {
        push_back(e);
    }

    FSnk( const args_t& args ) : Function(args)
    {

        ASSERT( args.size() == 1 );
    }

public: // virtual methods

    virtual std::string typeName() const { return FSnk::className(); }
    virtual std::string signature() const { return FSnk::sig(); }
    virtual std::string returnSignature() const { return F::sig(); }

    virtual void print( std::ostream& o ) const { o << "FSnk("; printArgs(o); o << ")"; }
    virtual void asCode( std::ostream& o ) const { NOTIMP; }
    virtual ExpPtr cloneWith(args_t& a) const { NOTIMP; }

    virtual ExpPtr evaluate( Scope& ctx ) const
    {
        ExpPtr fs = args(0, ctx, true);

        if( List::is(fs) )
        {
            const List::value_t& list = List::extract( ctx, fs);
            const size_t nlist = list.size();

            for( size_t i = 0; i < nlist; ++i )
            {
                ExpPtr f = list[i]->resolve(ctx)->eval(ctx);
                ASSERT( F::is(f) );
                sink( *f->as<F>() );
            }
        }
        else
        {
            ExpPtr f = fs->resolve(ctx)->eval(ctx);

            ASSERT( F::is(f) );

            sink( *f->as<F>() );
        }

        return boolean(true);
    }

private:

    void sink( const F& f ) const
    {
        std::cout << f << std::endl;
    }
};

ExpPtr fsnk( const ExpPtr& e ) { return ExpPtr( new FSnk( e ) ); }

//-----------------------------------------------------------------------------

/// Averages a list of fields and produces a field

class FAvg  : public eckit::xpr::Function  {

public: // methods

    static std::string sig() { return "favg"; }
    static std::string className() { return "FAvg"; }

    FAvg( const ExpPtr& e ) : Function()
    {
        push_back(e);
    }

    FAvg( const args_t& args ) : Function(args)
    {
        ASSERT( args.size() == 1 );
    }

public: // virtual methods

    virtual std::string typeName() const { return FAvg::className(); }
    virtual std::string signature() const { return FAvg::sig(); }
    virtual std::string returnSignature() const { return F::sig(); } //< returns a field

    virtual void print( std::ostream& o ) const { o << "FAvg("; printArgs(o); o << ")"; }
    virtual void asCode( std::ostream& o ) const { NOTIMP; }
    virtual ExpPtr cloneWith(args_t& a) const { NOTIMP; }

private:

    virtual ExpPtr evaluate( Scope& ctx ) const
    {
        ExpPtr fs = args(0, ctx, true);

        if( List::is(fs) )
        {
            const List::value_t& list = List::extract( ctx, fs);
            const size_t n = list.size();

            if( !n ) return xpr::undef();

            ExpPtr f0 = list[0]->resolve(ctx)->eval(ctx);

            std::cout << *f0 << std::endl;

            F::value_t rv = f0->as<F>()->fvec();

            for( size_t i = 1; i < n; ++i )
            {
                ExpPtr f = list[i]->resolve(ctx)->eval(ctx);
                std::cout << *f << std::endl;
                ASSERT( F::is(f) );
                const F::value_t& ri = f->as<F>()->fvec();
                ASSERT( rv.size() == ri.size() );
                for( int i = 0; i < rv.size(); ++i )
                    rv[i] += ri[i];
            }

            // divide by nlist

            std::transform(rv.begin(), rv.end(), rv.begin(),
                               [=](double x) { return x / (double) n; } );

            for( int i = 0; i < rv.size(); ++i ) std::cout << rv[i] << std::endl;

            return ExpPtr( new F("avg", rv) );
        }
        else
        {
            ExpPtr f = fs->resolve(ctx)->eval(ctx);
            ASSERT( F::is(f) );
            return f;
        }
    }

};

ExpPtr favg( ExpPtr f = undef() ) { return ExpPtr( new FAvg( f ) ); }

//-----------------------------------------------------------------------------

/// Expects only one field at a time

class FInterp : public eckit::xpr::Function  {

public: // methods

    static std::string sig() { return "interp"; }
    static std::string className() { return "FInterp"; }

    FInterp( const ExpPtr& e ) : Function()
    {
        push_back(e);
    }

    FInterp( const args_t& args ) : Function(args)
    {
        ASSERT( args.size() == 1 );
    }

public: // virtual methods

    virtual std::string typeName() const { return FInterp::className(); }
    virtual std::string signature() const { return FInterp::sig(); }
    virtual std::string returnSignature() const { return F::sig(); } //< returns a field

    virtual void print( std::ostream& o ) const { o << "FInterp("; printArgs(o); o << ")"; }
    virtual void asCode( std::ostream& o ) const { NOTIMP; }
    virtual ExpPtr cloneWith(args_t& a) const { NOTIMP; }

private:

    virtual ExpPtr evaluate( Scope& ctx ) const
    {
        ExpPtr f = args(0, ctx, true);

        ASSERT( F::is(f) );

//        std::cout << *f << std::endl;

        const F::value_t fv = f->as<F>()->fvec();
        const std::string& fname = f->as<F>()->fname();

        F::value_t rv( fv.size() / 2 );

        for( int i = 0; i < rv.size(); ++i )
            rv[i] = 2 * fv[i];

        ExpPtr r ( new F(fname,rv) );

//        std::cout << *r << std::endl;
        return r;
    }

};

ExpPtr finterp( ExpPtr f = undef() ) { return ExpPtr( new FInterp( f ) ); }

}

//-----------------------------------------------------------------------------

using namespace eckit_test;

BOOST_AUTO_TEST_SUITE( test_eckit_xpr_usage )

BOOST_AUTO_TEST_CASE( test_eckit_xpr_usage )
{
    Xpr f = fsnk(
                favg(
                    xpr::map( finterp(), fsrc( 10 ) )
                    )
                );

    std::cout << f << std::endl;

    f.eval();

//    Xpr m =  src );

    // Xpr avg = FAvg( m );

    // Xpr f = FSnk( avg );

    // f.eval();
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

