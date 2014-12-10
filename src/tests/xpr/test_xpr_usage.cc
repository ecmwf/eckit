/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/eckit_config.h"

#define BOOST_TEST_MODULE test_eckit_xpr

#include "ecbuild/boost_test_framework.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"

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

//-----------------------------------------------------------------------------

namespace eckit_test {

class Fld : public eckit::xpr::Value  {
public: // methods

    static std::string sig() { return "f"; }
    static std::string className() { return "Fld"; }
    
    Fld(ExpPtr e);
    Fld( const std::string& n ) : n_(n) {}

public: // virtual methods

    virtual std::string typeName() const { return Fld::className(); }
    virtual std::string signature() const { return Fld::sig(); }
    virtual std::string returnSignature() const { return Fld::sig(); }

    virtual void print( std::ostream& o ) const { o << "Fld(" << n_ << ")"; }
    virtual void asCode( std::ostream& o ) const { NOTIMP; }
    virtual ExpPtr cloneWith(args_t& a) const { NOTIMP; }

private:
    std::string n_;    
};

ExpPtr fld( const std::string& n ) { return ExpPtr( new Fld(n) ); }

class FldSrc : public eckit::xpr::Expression 
{

    // void evaluate()
    // {
    //     for( int i = 0; i < 4; ++i )

    //         xpr::list( f0 , f1, f2, f3 );
    // Xpr f0 = Fld("z");
    // Xpr f1 = Fld("u");
    // Xpr f2 = Fld("v");
    // Xpr f3 = Fld("t");
    // }

};

class FldSnk : public eckit::xpr::Expression 
{

};

class FldAvg : public eckit::xpr::Expression 
{

};

class FldInterp : public eckit::xpr::Expression  {
public: // methods

    static std::string sig() { return "interp"; }
    static std::string className() { return "FldInterp"; }

    FldInterp( const args_t& args );

public: // virtual methods

    virtual std::string typeName() const { return FldInterp::className(); }
    virtual std::string signature() const { return FldInterp::sig(); }
    virtual std::string returnSignature() const { return Fld::sig(); } //< returns a field

    virtual void print( std::ostream& o ) const { o << "FldInterp()"; }
    virtual void asCode( std::ostream& o ) const { NOTIMP; }
    virtual ExpPtr cloneWith(args_t& a) const { NOTIMP; }

private:

    virtual ExpPtr evaluate( Scope& ) const
    {
        ExpPtr f = args(0, ctx, false);
    }

private:

    ExpPtr f_;

};

ExpPtr interp( ExpPtr f ) { return ExpPtr( new FldInterp( f ) ); }

}

//-----------------------------------------------------------------------------

using namespace eckit_test;

BOOST_AUTO_TEST_SUITE( test_eckit_xpr_usage )

BOOST_AUTO_TEST_CASE( test_eckit_xpr_usage )
{
    Xpr f = fld("z");

    Xpr m = xpr::map( FldldInterp(), FldSrc() );

    // Xpr avg = FldAvg( m );

    // Xpr f = FldSnk( avg );

    // f.eval();
}

BOOST_AUTO_TEST_SUITE_END()

