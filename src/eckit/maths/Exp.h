/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Exp.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date November 2013

#ifndef eckit_maths_Exp_h
#define eckit_maths_Exp_h

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

#include "eckit/eckit.h"

//-----------------------------------------------------------------------------

#include <algorithm>
#include <iostream>
#include <sstream>
#include <iterator>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <functional>

#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include "eckit/exception/Exceptions.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

#if 1
#define DBG     std::cout << Here() << std::endl;
#define DBGX(x) std::cout << Here() << " " << #x << " -> " << x << std::endl;
#else
#define DBG
#define DBGX(x)
#endif

//--------------------------------------------------------------------------------------------

typedef double scalar_t;

class Value;
class Expression;

typedef boost::shared_ptr<Value> ValPtr;
typedef boost::shared_ptr<Expression> ExpPtr;

typedef std::vector< ExpPtr > args_t;

ExpPtr add( ExpPtr, ExpPtr );
ExpPtr prod( ExpPtr, ExpPtr );
ExpPtr prod_add( ExpPtr, ExpPtr, ExpPtr );
ExpPtr linear( ExpPtr, ExpPtr, ExpPtr, ExpPtr );

//--------------------------------------------------------------------------------------------

/// Expression Error
class Error: public eckit::Exception {
public:
    Error(const eckit::CodeLocation& loc, const string& s) : Exception( s, loc ) {}
};

//--------------------------------------------------------------------------------------------

class Expression :
    public boost::enable_shared_from_this<Expression>,
    private boost::noncopyable {

public: // methods

    static std::string class_name() { return "Exp"; }

    virtual ~Expression();

    ExpPtr self() { return shared_from_this(); }

    template< typename T >
    boost::shared_ptr<T> as() { return boost::dynamic_pointer_cast<T,Expression>( shared_from_this() ); }

    friend std::ostream& operator<<( std::ostream& os, const Expression& v) { v.print(os); return os; }

    ValPtr eval()
    {
        return reduce()->evaluate();
    }

public: // virtual methods

    virtual std::string type_name() const = 0;
    virtual ValPtr evaluate() = 0;
    virtual ExpPtr reduce() = 0;
    virtual void print( std::ostream& ) const = 0;
    virtual std::string signature() const = 0;

    virtual size_t arity() const = 0;
    virtual ExpPtr param( const size_t& ) const = 0;
    virtual void param( const size_t&, ExpPtr p ) = 0;

};

//--------------------------------------------------------------------------------------------

class Value : public Expression {
public: // methods

    static std::string class_name() { return "Value"; }

    Value(){}

    virtual ExpPtr reduce() { return shared_from_this(); }

    virtual ValPtr evaluate()
    {
        return boost::static_pointer_cast<Value>( shared_from_this() );
    }
};

//--------------------------------------------------------------------------------------------

#define SCALAR_SIG "s"

class Scalar : public Value {
public: // methods

    typedef scalar_t value_t;

    static std::string sig() { return SCALAR_SIG; }
    static std::string class_name() { return "Scalar"; }
    static bool is ( const ExpPtr& e ) { return e->signature() == SCALAR_SIG; }
    static scalar_t extract ( const ExpPtr& e )
    {
        assert( Scalar::is(e) );
        return e->as<Scalar>()->value();
    }

    Scalar( const ExpPtr& e) : v_( Scalar::extract( e->evaluate() ) )
    {
    }

    Scalar( const scalar_t& v ) : v_(v) {}

    /// @returns the size of the data, one since it is a scalar
    size_t size() const { return 1; }
    /// @returns the value of the scalar
    value_t value() const { return v_; }
    /// @returns a reference to the scalar
    value_t& ref_value() { return v_; }

public: // virtual methods

//    virtual ValPtr evaluate()
//    {
//        return ValPtr(new Scalar(v_));
//    }

    virtual std::string type_name() const { return Scalar::class_name(); }
    virtual std::string signature() const { return sig(); }

    virtual size_t arity() const { return 0; }
    virtual ExpPtr param( const size_t& ) const  { NOTIMP; return ExpPtr(); }
    virtual void param( const size_t&, ExpPtr p ) {}

    virtual void print( std::ostream& o ) const { o << "s(" << v_ << ")"; }

protected: // members
    value_t v_;
};

ExpPtr scalar( const scalar_t& s  ) { return ExpPtr( new Scalar(s) ); }

//--------------------------------------------------------------------------------------------

#define VECTOR_SIG "v"

class Vector : public Value {
public: // types

    typedef scalar_t elemt_t;
    typedef std::vector<elemt_t> value_t;

public: // methods

    static std::string sig() { return VECTOR_SIG; }
    static std::string class_name() { return "Vector"; }
    static bool is ( const ExpPtr& e ) { return e->signature() == VECTOR_SIG; }
    static value_t& extract ( const ExpPtr& e )
    {
        assert( Vector::is(e) );
        return e->as<Vector>()->ref_value();
    }

    Vector( const size_t& s, const scalar_t& v = scalar_t() ) : v_(s,v) {}
    Vector( const value_t& v ) : v_(v) {}

    /// @returns the size of the internal vector
    size_t size() const { return v_.size(); }
    /// @returns a copy of the internal vector
    value_t value() const { return v_; }
    /// @returns a reference to the internal vector
    value_t& ref_value() { return v_; }

public: // virtual methods

    virtual std::string type_name() const { return Vector::class_name(); }
    virtual std::string signature() const { return sig(); }

    virtual size_t arity() const { return 0; }
    virtual ExpPtr param( const size_t& ) const  { return ExpPtr(); }
    virtual void param( const size_t&, ExpPtr p ) {}

    virtual void print( std::ostream& o ) const
    {
        o << "[";
        for( size_t i = 0; i < v_.size(); ++i )
        {
            if(i) o << ", ";
            o << v_[i];
        }
        o << "]";
    }

protected: // members
    value_t v_;
};

ExpPtr vector( const size_t& sz, const scalar_t& v = scalar_t()  ) { return ExpPtr( new Vector(sz,v) ); }
ExpPtr vector( const Vector::value_t& v  ) { return ExpPtr( new Vector(v) ); }

//--------------------------------------------------------------------------------------------

/// Represents a Function with some arguments
class Func : public Expression {

public: // types

    typedef std::string key_t;
    typedef boost::function< ValPtr ( const args_t& ) > func_t;

    typedef std::map< key_t, func_t > dispatcher_t;

protected: // members

    args_t params_; ///< parameters of this expression

protected: // methods

    /// Empty contructor is usually used by derived classes that
    /// handle the setup of the parameters themselves
    Func() {}

    /// Contructor taking a list of parameters
    /// handle the setup of the parameters themselves
    Func( const args_t& p ) : params_(p) {}

public: // methods

    virtual ~Func() {}

    static dispatcher_t& dispatcher() { static dispatcher_t d; return d; }

    static std::string class_name() { return "Func"; }

public: // virtual methods

    virtual ValPtr evaluate()
    {
        std::string sig = signature();

        for( args_t::iterator i = params_.begin(); i != params_.end(); ++i )
        {
            (*i) = (*i)->reduce();
        }

        dispatcher_t& d = dispatcher();
        dispatcher_t::iterator itr = d.find( sig );
        if( itr == d.end() )
        {
            ostringstream msg;
            msg << "could not dispatch to function with signature: " << sig;
            throw Error( Here(), msg.str() );
        }

        return ((*itr).second)( params_ );
    }

    virtual ExpPtr reduce();

    virtual std::string signature() const
    {
        return signature_args( params_ );
    }

    std::string signature_args( const args_t& args ) const
    {
        std::ostringstream o;
        o << type_name() << "(";
        for( size_t i = 0; i < args.size(); ++i )
        {
            if(i) o << ",";
            o << args[i]->signature();
        }
        o << ")";
        return o.str();
    }

    virtual size_t arity() const { return params_.size(); }

    virtual ExpPtr param( const size_t& i ) const
    {
        assert( i < params_.size() );
        assert( params_[i] );
        return params_[i];
    }

    virtual void param( const size_t& i, ExpPtr p )
    {
        DBGX( *p );
        assert( i < params_.size() );
        if( p != params_[i] )
            params_[i] = p;
    }

    virtual void print( std::ostream& o ) const
    {
        o << type_name() << "(";
        for( size_t i = 0; i < arity(); ++i )
        {
            if(i) o << ", ";
            o << *param(i);
        }
        o << ")";
    }
};

//--------------------------------------------------------------------------------------------

typedef multiplies<scalar_t>  Multiply;
typedef divides<scalar_t>     Divide;
typedef plus<scalar_t>        Plus;
typedef minus<scalar_t>       Minus;

inline const char *opname(const Multiply&)     { return "Mult";  }
inline const char *opname(const Divide&)       { return "Divide";}
inline const char *opname(const Plus&)         { return "Plus";  }
inline const char *opname(const Minus&)        { return "Minus"; }

//--------------------------------------------------------------------------------------------

template <class T> ValPtr apply( T op, const Scalar::value_t& a, const Scalar::value_t& b )
{
    return ValPtr( new Scalar( op( a , b ) ) );
}

template <class T> ValPtr apply( T op, const Scalar::value_t& a, const Vector::value_t& v )
{
    Vector* res = new Vector( v.size() );
    Vector::value_t& rv = res->ref_value();

    for( size_t i = 0; i < rv.size(); ++i )
        rv[i] = op( a , v[i] );

    return ValPtr( res );
}

template <class T> ValPtr apply( T op, const Vector::value_t& v, const Scalar::value_t& a )
{
    return apply(op,a,v);
}

template <class T> ValPtr apply( T op, const Vector::value_t& v1, const Vector::value_t& v2 )
{
    assert( v1.size() == v2.size() );

    Vector* res = new Vector( v1.size() );
    Vector::value_t& rv = res->ref_value();

    for( size_t i = 0; i < rv.size(); ++i )
        rv[i] = op( v1[i] , v2[i] );

    return ValPtr( res );
}

//--------------------------------------------------------------------------------------------

/// Generates a expressions
template <class T>
class BinaryFunc  {
public:

    static std::string class_name() { return opname( T() ); }

    /// The actual function expression
    struct Op : public Func
    {
        Op( const args_t& args ) : Func( args ) {}
        virtual std::string type_name() const { return BinaryFunc<T>::class_name(); }
    };

    /// Expression generator function
    ExpPtr operator() ( ExpPtr p0, ExpPtr p1 )
    {
        args_t args;
        args.push_back( p0 );
        args.push_back( p1 );
        return ExpPtr( new Op(args) );
    }
};

template <class T, class U, class V > // , class S = Generic >
class BinaryFuncT {
public:

    static std::string class_name() { return opname( T() ) + string("(") + U::sig() + string(",") + V::sig() + string(")"); }

    BinaryFuncT()
    {
       Func::dispatcher()[ class_name() ] = &compute;
    }

    virtual std::string type_name() const { return BinaryFuncT<T,U,V>::class_name(); }

    static ValPtr compute( const args_t& p )
    {
        T op;
        typename U::value_t a = U::extract(p[0]);
        typename V::value_t b = V::extract(p[1]);

        return apply(op,a,b);  // return S::apply(op,a,b);
    }
};

static BinaryFuncT<Plus,Scalar,Scalar> pss;
static BinaryFuncT<Plus,Scalar,Vector> psv;
static BinaryFuncT<Plus,Vector,Scalar> pvs;
static BinaryFuncT<Plus,Vector,Vector> pvv;

static BinaryFuncT<Multiply,Scalar,Scalar> mss;
static BinaryFuncT<Multiply,Scalar,Vector> msv;
static BinaryFuncT<Multiply,Vector,Scalar> mvs;
static BinaryFuncT<Multiply,Vector,Vector> mvv;

//--------------------------------------------------------------------------------------------

template< class T >
ExpPtr make_expr( ExpPtr l, ExpPtr r )
{
    args_t args;
    args.push_back(l);
    args.push_back(r);
    return ExpPtr( new T(args) );
}

// version with stand alone functions

#define GEN_BINFUNC( f, c )                                                                  \
ExpPtr f( ExpPtr l, ExpPtr r )           { return ExpPtr( c()(l,r) ); }                      \
ExpPtr f( Expression& l, ExpPtr r )      { return ExpPtr( c()(l.self(),r) ); }               \
ExpPtr f( ExpPtr l, Expression& r )      { return ExpPtr( c()(l,r.self()) ); }               \
ExpPtr f( Expression& l, Expression& r ) { return ExpPtr( c()(l.self(),r.self()) ); }

GEN_BINFUNC(mul,BinaryFunc<Multiply>)
GEN_BINFUNC(div,BinaryFunc<Divide>)
GEN_BINFUNC(prod,BinaryFunc<Multiply>)
GEN_BINFUNC(add,BinaryFunc<Plus>)
GEN_BINFUNC(sub,BinaryFunc<Minus>)

#undef GEN_BINFUNC



//--------------------------------------------------------------------------------------------

/// Generates a Linear combination of vectors
class Linear : public Func {
public: // methods

    Linear( ExpPtr e ) : Func()
    {
        assert( e->arity() == 2 );

        ExpPtr left  = e->param(0);
        ExpPtr right = e->param(1);

        assert( left->arity() == 2 );
        assert( right->arity() == 2 );

        params_.push_back( left->param(0) );
        params_.push_back( left->param(1) );
        params_.push_back( right->param(0) );
        params_.push_back( right->param(1) );

        /// @todo should check vectors of same size
    }

    virtual ~Linear() {}

    static std::string class_name() { return "Linear"; }

    virtual std::string type_name() const { return Linear::class_name(); }

    virtual size_t arity() const { return 4; }

    virtual ValPtr evaluate( const args_t& p )
    {
        DBG;

        scalar_t a = Scalar::extract( p[0] );

        Vector::value_t& v1 = Vector::extract( p[1] );

        scalar_t b = Scalar::extract( p[2] );

        Vector::value_t& v2 = Vector::extract( p[3] );

        assert( v1.size() == v2.size() );

        Vector* res = new Vector( v1.size() );
        Vector::value_t& rv = res->ref_value();

        for( size_t i = 0; i < rv.size(); ++i )
            rv[i] = a * v1[i] + b * v2[i];

        return ValPtr(res);
    }

};

//--------------------------------------------------------------------------------------------

class Reducer {
public:

    typedef std::map<std::string,Reducer*> reducers_t;

    static ExpPtr apply( ExpPtr e )
    {
        std::string signature = e->signature();

        reducers_t& reducer = reducers();
        std::map<std::string,Reducer*>::const_iterator itr = reducer.find(signature);
        if( itr == reducer.end() )
            return e;
        return (*itr).second->reduce(e);
    }

protected:

    Reducer( const std::string& signature )
    {
        reducers()[signature] = this;
    }

    virtual ExpPtr reduce( ExpPtr ) const = 0;

    static reducers_t& reducers()
    {
        static reducers_t reducers_;
        return reducers_;
    }
};

template< typename T>
class ReducerT : public Reducer {
public:

    ReducerT( const std::string& signature ) : Reducer(signature) {}

private:

    ExpPtr reduce( ExpPtr e ) const
    {
        return ExpPtr( new T(e) );
    }
};

//--------------------------------------------------------------------------------------------

static ReducerT<Scalar> optimConst("Plus(s,s)");
static ReducerT<Linear> optimLinear("Plus(Mult(s,v),Mult(s,v))");

//--------------------------------------------------------------------------------------------

/// Generates a ProdAdd expressions
class ProdAdd {
public:

    static std::string class_name() { return "ProdAdd"; }

    /// Represents a ProdAdd expression
    struct Op : public Func
    {
        Op( const args_t& args ) : Func( args ) {}
        virtual std::string type_name() const { return ProdAdd::class_name(); }
    };

    static ValPtr eval_svv( const args_t& p )
    {
        assert( p.size() == 3 );

        scalar_t a = Scalar::extract( p[0] );
        Vector::value_t& v1 = Vector::extract( p[1] );
        Vector::value_t& v2 = Vector::extract( p[2] );

        assert( v1.size() == v2.size() );

        Vector* res = new Vector( v1.size() );
        Vector::value_t& rv = res->ref_value();

        for( size_t i = 0; i < rv.size(); ++i )
            rv[i] = a * ( v1[i] + v2[i] );

        return ValPtr(res);
    }

    static ValPtr eval_vvv( const args_t& p )
    {
        assert( p.size() == 3 );

        Vector::value_t& v0 = Vector::extract( p[0] );
        Vector::value_t& v1 = Vector::extract( p[1] );
        Vector::value_t& v2 = Vector::extract( p[2] );

        assert( v1.size() == v2.size() );

        Vector* res = new Vector( v1.size() );
        Vector::value_t& rv = res->ref_value();

        for( size_t i = 0; i < rv.size(); ++i )
            rv[i] = v0[i] * ( v1[i] + v2[i] );

        return ValPtr(res);
    }

    /// Generic prod_add implementation based on calling first add() then prod()
    static ValPtr eval_ggg( const args_t& p )
    {
        return prod(p[0],add(p[1],p[2]))->eval();
    }

    /// Expression generator function
    ExpPtr operator() ( ExpPtr p0, ExpPtr p1, ExpPtr p2 )
    {
        args_t args;
        args.push_back( p0 );
        args.push_back( p1 );
        args.push_back( p2 );
        return ExpPtr( new Op(args) );
    }

    struct Register
    {
        Register()
        {
            Func::dispatcher()[ class_name() + "(s,s,s)" ] = &eval_ggg;
            Func::dispatcher()[ class_name() + "(s,v,s)" ] = &eval_ggg;
            Func::dispatcher()[ class_name() + "(s,s,v)" ] = &eval_ggg;

            Func::dispatcher()[ class_name() + "(s,v,v)" ] = &eval_svv;

            Func::dispatcher()[ class_name() + "(v,s,s)" ] = &eval_ggg;
            Func::dispatcher()[ class_name() + "(v,s,v)" ] = &eval_ggg;
            Func::dispatcher()[ class_name() + "(v,v,s)" ] = &eval_ggg;

            Func::dispatcher()[ class_name() + "(v,v,v)" ] = &eval_vvv;
        }
    };

};

static ProdAdd::Register prodadd_register;

// version with stand alone functions

ExpPtr prod_add( ExpPtr p0, ExpPtr p1, ExpPtr p2 ) { return ExpPtr( ProdAdd()(p0,p1,p2) ); }

//--------------------------------------------------------------------------------------------

//ExpPtr operator+ ( ValPtr p1, ValPtr p2 ) { return add( p1, p2 ); }
//ExpPtr operator+ ( ValPtr p1, ExpPtr p2 ) { return add( p1, p2 ); }
//ExpPtr operator+ ( ExpPtr p1, ValPtr p2 ) { return add( p1, p2 ); }
//ExpPtr operator+ ( ExpPtr p1, ExpPtr p2 ) { return add( p1, p2 ); }

//ExpPtr operator* ( ValPtr p1, ValPtr p2 ) { return prod( p1, p2 ); }
//ExpPtr operator* ( ValPtr p1, ExpPtr p2 ) { return prod( p1, p2 ); }
//ExpPtr operator* ( ExpPtr p1, ValPtr p2 ) { return prod( p1, p2 ); }
//ExpPtr operator* ( ExpPtr p1, ExpPtr p2 ) { return prod( p1, p2 ); }

//--------------------------------------------------------------------------------------------

ExpPtr Func::reduce()
{
    DBGX( *this );
    DBGX( signature() );

    /// now try to reduce self
    return Reducer::apply( shared_from_this() );
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
