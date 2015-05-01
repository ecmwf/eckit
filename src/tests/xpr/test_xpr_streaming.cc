/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath> // ceil()

#include "eckit/eckit_config.h"

#define BOOST_TEST_MODULE test_eckit_xpr_streaming

#include "ecbuild/boost_test_framework.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/serialisation/FileStream.h"
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
#include "eckit/xpr/Real.h"
#include "eckit/xpr/UnaryOperator.h"
#include "eckit/xpr/Vector.h"
#include "eckit/xpr/ZipWith.h"
#include "eckit/xpr/Bind.h"

using namespace std;
using namespace eckit;
using namespace eckit::xpr;

namespace eckit_test {

//-----------------------------------------------------------------------------

class F : public Vector  {

public: // methods

    static std::string sig() { return "f"; }
    static const char * nodeName() { return "F"; }
    
    static bool is ( const ExpPtr& e ) { return e->typeName() == nodeName(); }

    F( const string& n, const double& v ) : Vector(32, v), n_(n) {}

    F( const string& n, const value_t& v ) : n_(n), Vector(v) {}

    F(Stream& s) : Vector(s), n_("") {
        s >> n_;
    }

    virtual void encode( Stream& s ) const {
        Vector::encode(s);
        s << n_;
    }

    const std::string& fname() const { return n_; }

    const value_t fvec() const { return v_; }

public: // virtual methods

    virtual const char * typeName() const { return nodeName(); }
    virtual std::string signature() const { return sig(); }
    virtual std::string returnSignature() const { return sig(); }

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

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec() { return classSpec_; }

private:

    std::string n_;

    static  ClassSpec classSpec_;
    static  Reanimator<F> reanimator_;
};

ClassSpec F::classSpec_ = {
    &Vector::classSpec(),
    F::nodeName(),
};

Reanimator<F> F::reanimator_;

ExpPtr fld( const string& n, const double& v) { return ExpPtr( new F(n,v) ); }

//-----------------------------------------------------------------------------

/// Creates a list of Fields

class FSrc : public Function {

public: // methods

    static std::string sig() { return "fsrc"; }
    static const char * nodeName() { return "FSrc"; }

    FSrc( const ExpPtr& e ) : Function()
    {
        push_back(e);
    }

    FSrc( const args_t& args ) : Function(args)
    {

        ASSERT( args.size() == 1 );
    }

    FSrc(Stream& s) : Function(s) {}

    virtual void encode(Stream& s) const {
        Function::encode(s);
    }

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec() { return classSpec_; }

public: // virtual methods

    virtual const char * typeName() const { return nodeName(); }
    virtual std::string signature() const { return sig(); }
    virtual std::string returnSignature() const { NOTIMP; }

    virtual void print( std::ostream& o ) const
    {
        o << "FSrc("; printArgs(o); o << ")";
    }
    virtual void asCode( std::ostream& o ) const { NOTIMP; }
    virtual ExpPtr cloneWith(args_t& a) const { NOTIMP; }

    virtual ExpPtr evaluate( Scope& ctx ) const
    {
        ExpPtr s = args(0, ctx, true);

        long n = (long) ::ceil( Real::extract( s ) );

        ExpPtr lst = xpr::list();

        for( long i = 0; i < n; ++i )
        {
            std::string fn = "n" + Translator<long,std::string>()(i);

            std::cout << "creating " << fn << std::endl;

            lst = xpr::list( lst, fld( fn, (double) i ) );
        }

        return lst;
    }

private: // static members

    static  ClassSpec classSpec_;
    static  Reanimator<FSrc> reanimator_;
};

ClassSpec FSrc::classSpec_ = {
    &Function::classSpec(),
    FSrc::nodeName(),
};

Reanimator< FSrc > FSrc::reanimator_;

ExpPtr fsrc( const long& n ) { return ExpPtr( new FSrc( xpr::real(n) ) ); }

//-----------------------------------------------------------------------------

/// Outputs fields (accepts isolated or list of fields )

class FSnk : public eckit::xpr::Function {
public: // methods

    static std::string sig() { return "fsnk"; }
    static const char * nodeName() { return "FSnk"; }

    FSnk( const ExpPtr& e ) : Function()
    {
        push_back(e);
    }

    FSnk( const args_t& args ) : Function(args)
    {

        ASSERT( args.size() == 1 );
    }

    FSnk(Stream& s) : Function(s) {}

    virtual void encode(Stream& s) const {
        Function::encode(s);
    }

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec() { return classSpec_; }

public: // virtual methods

    virtual const char * typeName() const { return nodeName(); }
    virtual std::string signature() const { return sig(); }
    virtual std::string returnSignature() const { return F::sig(); }

    virtual void print( std::ostream& o ) const
    {
        o << "FSnk("; printArgs(o); o << ")";
    }
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

private: // static members

    static  ClassSpec classSpec_;
    static  Reanimator<FSnk> reanimator_;
};

ClassSpec FSnk::classSpec_ = {
    &Function::classSpec(),
    FSnk::nodeName(),
};

Reanimator< FSnk > FSnk::reanimator_;

ExpPtr fsnk( const ExpPtr& e ) { return ExpPtr( new FSnk( e ) ); }

//-----------------------------------------------------------------------------

/// Averages a list of fields and produces a field

class FAvg  : public Function  {

public: // methods

    static std::string sig() { return "favg"; }
    static const char * nodeName() { return "FAvg"; }

    FAvg( const ExpPtr& e ) : Function()
    {
        push_back(e);
    }

    FAvg( const args_t& args ) : Function(args)
    {
        ASSERT( args.size() == 1 );
    }

    FAvg(Stream& s) : Function(s) {}

    virtual void encode(Stream& s) const {
        Function::encode(s);
    }

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec() { return classSpec_; }

public: // virtual methods

    virtual const char * typeName() const { return nodeName(); }
    virtual std::string signature() const { return sig(); }
    virtual std::string returnSignature() const { return F::sig(); } //< returns a field

    virtual void print( std::ostream& o ) const
    {
        o << "FAvg("; printArgs(o); o << ")";
    }
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

            for( int i = 0; i < rv.size(); ++i )
                std::cout << rv[i] << std::endl;

            return ExpPtr( new F("avg", rv) );
        }
        else
        {
            ExpPtr f = fs->resolve(ctx)->eval(ctx);
            ASSERT( F::is(f) );
            return f;
        }
    }

private: // static members

    static  ClassSpec classSpec_;
    static  Reanimator<FAvg> reanimator_;
};

ClassSpec FAvg::classSpec_ = {
    &Function::classSpec(),
    FAvg::nodeName(),
};

Reanimator< FAvg> FAvg::reanimator_;

ExpPtr favg( ExpPtr f = undef() ) { return ExpPtr( new FAvg( f ) ); }

//-----------------------------------------------------------------------------

/// Expects only one field at a time

class FInterp : public Function  {

public: // methods

    static std::string sig() { return "interp"; }
    static const char * nodeName() { return "FInterp"; }

    FInterp( const ExpPtr& e ) : Function()
    {
        push_back(e);
    }

    FInterp( const args_t& args ) : Function(args)
    {
        ASSERT( args.size() == 1 );
    }

    FInterp(Stream& s) : Function(s) {}

    virtual void encode(Stream& s) const {
        Function::encode(s);
    }

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec() { return classSpec_; }

public: // virtual methods

    virtual const char * typeName() const { return nodeName(); }
    virtual std::string signature() const { return sig(); }
    virtual std::string returnSignature() const { return F::sig(); } //< returns a field

    virtual void print( std::ostream& o ) const
    {
        o << "FInterp("; printArgs(o); o << ")";
    }
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

private: // static members

    static  ClassSpec classSpec_;
    static  Reanimator<FInterp> reanimator_;
};

ClassSpec FInterp::classSpec_ = {
    &Function::classSpec(),
    FInterp::nodeName(),
};

Reanimator< FInterp > FInterp::reanimator_;

ExpPtr finterp( ExpPtr f = undef() ) { return ExpPtr( new FInterp( f ) ); }

}

//-----------------------------------------------------------------------------

using namespace eckit_test;

BOOST_AUTO_TEST_SUITE( test_eckit_xpr_streaming )

BOOST_AUTO_TEST_CASE( test_eckit_xpr_reanimate )
{
    Xpr f = fsnk(
                favg(
                    xpr::map( finterp(), fsrc( 10 ) )
                    )
                );

    PathName filename = PathName::unique( "data" );
    std::string filepath = filename.asString();
    {
        BOOST_TEST_MESSAGE("Streaming to file:");
        BOOST_TEST_MESSAGE("  " << f);
        FileStream sout( filepath.c_str(), "w" );
        sout << *(f.expr());
    }
    {
        BOOST_TEST_MESSAGE("Streaming from file:");
        FileStream sin( filepath.c_str(), "r" );
        Xpr f2( Reanimator<FSnk>::reanimate(sin)->self() );
        BOOST_TEST_MESSAGE("  " << f2);
        f2.eval();
    }
    if (filename.exists()) filename.unlink();
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

