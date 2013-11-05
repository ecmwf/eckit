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

#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

//-----------------------------------------------------------------------------

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

typedef double scalar_t;

class Var;
class Exp;

typedef boost::shared_ptr<Var> VarPtr;
typedef boost::shared_ptr<Exp> ExpPtr;

ExpPtr add( ExpPtr, ExpPtr );
ExpPtr prod( ExpPtr, ExpPtr );
ExpPtr prod_add( ExpPtr, ExpPtr, ExpPtr );

//--------------------------------------------------------------------------------------------

class Exp : public boost::enable_shared_from_this<Exp>,
            private boost::noncopyable {
public:

    static std::string class_name() { return "Exp"; }

    typedef boost::shared_ptr<Exp> Ptr;

    enum Type { SCALAR, VECTOR, OP };

    virtual Exp::Type type() const = 0;
    virtual std::string type_name() const = 0;

    virtual bool isTerminal() const = 0;

    virtual ~Exp();

    virtual VarPtr eval() = 0;
    virtual ExpPtr optimise() = 0;
    virtual ExpPtr reduce();

    virtual size_t arity() const = 0;
    virtual ExpPtr param( const size_t& ) const = 0;
    virtual void param( const size_t&, ExpPtr p ) { assert(false); }

    ExpPtr self() { return shared_from_this(); }

    bool isVar() const { return isTerminal(); }
    bool isOpr() const { return ! isTerminal(); }

    template< typename T >
    boost::shared_ptr<T> as() { return boost::dynamic_pointer_cast<T,Exp>( shared_from_this() ); }

    virtual void print( std::ostream& ) const = 0;
    virtual std::string signature() const = 0;

    friend std::ostream& operator<<( std::ostream& os, const Exp& v) { v.print(os); return os; }

};

//--------------------------------------------------------------------------------------------

class Var : public Exp {
public:

    static std::string class_name() { return "Var"; }

    virtual size_t arity() const { return 0; }
    virtual ExpPtr param( const size_t& ) const { assert( false ); } // should not be called

    virtual bool isTerminal() const { return true; }

    virtual VarPtr eval() { return boost::static_pointer_cast<Var>( shared_from_this() ); }
    virtual ExpPtr optimise() { return shared_from_this(); }

    virtual size_t size() const = 0;

};

//--------------------------------------------------------------------------------------------

class Scalar : public Var {
public: // methods

    static std::string class_name() { return "Scalar"; }

    Scalar( const scalar_t& v ) : v_(v) {}

    virtual Exp::Type type() const { return Exp::SCALAR; }
    virtual std::string type_name() const { return Scalar::class_name(); }

    virtual size_t size() const { return 1; }
    scalar_t value() const { return v_; }

    /// returns a reference to the scalar
    scalar_t& ref_value() { return v_; }

    virtual void print( std::ostream& o ) const { o << v_; }

    virtual std::string signature() const { return "s"; }

protected:
    scalar_t v_;
};

VarPtr scalar( const scalar_t& s  ) { return VarPtr( new Scalar(s) ); }

//--------------------------------------------------------------------------------------------

class Vector : public Var {
public: // types

    typedef scalar_t value_t;
    typedef std::vector<scalar_t> storage_t;

public: // methods

    static std::string class_name() { return "Vector"; }

    Vector( const size_t& s, const scalar_t& v = scalar_t() ) : v_(s,v) {}
    Vector( const storage_t& v ) : v_(v) {}

    virtual Exp::Type type() const { return Exp::VECTOR; }
    virtual std::string type_name() const { return Vector::class_name(); }

    virtual size_t size() const { return v_.size(); }

    /// returns a copy of the internal vector
    storage_t value() const { return v_; }

    /// returns a reference to the internal vector
    storage_t& ref_value() { return v_; }

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

    virtual std::string signature() const { return "v"; }

protected:
    storage_t v_;
};

VarPtr vector( const size_t& sz, const scalar_t& v = scalar_t()  ) { return VarPtr( new Vector(sz,v) ); }
VarPtr vector( const Vector::storage_t& v  ) { return VarPtr( new Vector(v) ); }

//--------------------------------------------------------------------------------------------

class Opr : public Exp {
public: // methods

    static std::string class_name() { return "Opr"; }

    virtual bool isTerminal() const { return false; }

    virtual Exp::Type type() const { return Exp::OP; }
    virtual std::string type_name() const { return Opr::class_name(); }

    virtual std::string opName() const = 0;

    virtual void print( std::ostream& o ) const
    {
        o << opName() << "(";
        for( size_t i = 0; i < arity(); ++i )
        {
            if(i) o << ", ";
            o << *param(i);
        }
        o << ")";
    }

    virtual std::string signature() const
    {
        std::ostringstream o;
        o << opName() << "(";
        for( size_t i = 0; i < arity(); ++i )
        {
            if(i) o << ",";
            o << param(i)->signature();
        }
        o << ")";
        return o.str();
    }

    virtual ExpPtr reduce()
    {
        for( size_t i = 0; i < arity(); ++i )
        {
            ExpPtr e = param(i);
            param(i,e->reduce() );
        }
        return shared_from_this();
    }

};

typedef boost::shared_ptr<Opr> OprPtr;

//--------------------------------------------------------------------------------------------

class BinOp : public Opr {

public: // types

    typedef boost::tuple< Exp::Type, Exp::Type, std::string > key_t;
    typedef boost::function< VarPtr ( ExpPtr& , ExpPtr& ) >   value_t;
    typedef std::map< key_t, value_t > dispatcher_t;

    typedef boost::function< ExpPtr ( ExpPtr ) > optim_t;
    typedef std::map< key_t, optim_t > optimiser_t;

public: // methods

    BinOp( ExpPtr p1, ExpPtr p2 ) : p1_(p1), p2_(p2) {}

    virtual std::string type_name() const { return BinOp::class_name(); }

    VarPtr eval()
    {
        BinOp::key_t k = boost::make_tuple( p1_->type(), p2_->type(), opName() );

        BinOp::dispatcher_t& d = dispatcher();
        BinOp::dispatcher_t::iterator itr = d.find(k);
        if( itr != d.end() )
            return ((*itr).second)( p1_, p2_ );

        assert( p1_->isOpr() || p2_->isOpr() ); // either one is an Op

        p1_ = p1_->eval(); // possibly creates temporary
        p2_ = p2_->eval(); // possibly creates temporary

        return eval(); // recall eval
    }

    ExpPtr optimise()
    {
        p1_ = p1_->optimise();
        p2_ = p2_->optimise();

        BinOp::key_t k = boost::make_tuple( p1_->type(), p2_->type(), opName() );

        BinOp::optimiser_t& d = optimiser();
        BinOp::optimiser_t::iterator itr = d.find(k);
        if( itr != d.end() )
            return ((*itr).second)( shared_from_this() );

        return shared_from_this();
    }

    static dispatcher_t& dispatcher() { static dispatcher_t d; return d; }
    static optimiser_t& optimiser() { static optimiser_t o; return o; }

    virtual size_t arity() const { return 2; }
    virtual ExpPtr param( const size_t& i ) const
    {
        assert( i < 2 );
        if( i ) return p2_;
        return p1_;
    }

    virtual void param( const size_t& i, ExpPtr p )
    {
        assert( i < 2 );
        if( i )
            p2_ = p;
        else
            p1_ = p;
    }

protected:

    ExpPtr p1_;
    ExpPtr p2_;

};

//--------------------------------------------------------------------------------------------

class TriOp : public Opr {

public: // types

    typedef boost::tuple< Exp::Type, Exp::Type, Exp::Type, std::string >   key_t;
    typedef boost::function< VarPtr ( ExpPtr&, ExpPtr&, ExpPtr& ) >        value_t;
    typedef std::map< key_t, value_t > dispatcher_t;

    typedef boost::function< ExpPtr ( ExpPtr ) > optim_t;
    typedef std::map< key_t, optim_t > optimiser_t;

public: // methods

    TriOp( ExpPtr p1, ExpPtr p2, ExpPtr p3 ) : p1_(p1), p2_(p2), p3_(p3) {}

    VarPtr eval()
    {
        TriOp::key_t k = boost::make_tuple( p1_->type(),
                                            p2_->type(),
                                            p3_->type(),
                                            opName() );

        TriOp::dispatcher_t& d = dispatcher();
        TriOp::dispatcher_t::iterator itr = d.find(k);
        if( itr != d.end() )
            return ((*itr).second)( p1_, p2_, p3_ );

        assert( p1_->isOpr() || p2_->isOpr() || p3_->isOpr() ); // either one is an Op

        /// @todo optimize this dispatch by looking into possible reduction of temporaries

        p1_ = p1_->eval(); // possibly creates temporary
        p2_ = p2_->eval(); // possibly creates temporary
        p3_ = p3_->eval(); // possibly creates temporary

        return eval(); // recall eval
    }

    ExpPtr optimise()
    {
        p1_ = p1_->optimise();
        p2_ = p2_->optimise();
        p3_ = p3_->optimise();

        TriOp::key_t k = boost::make_tuple( p1_->type(),
                                            p2_->type(),
                                            p3_->type(),
                                            opName() );

        TriOp::optimiser_t& d = optimiser();
        TriOp::optimiser_t::iterator itr = d.find(k);
        if( itr != d.end() )
            return ((*itr).second)( shared_from_this() );

        return shared_from_this();
    }

    virtual std::string type_name() const { return "TriOp"; }

    ExpPtr& p1() { return p1_; }
    ExpPtr& p2() { return p2_; }
    ExpPtr& p3() { return p3_; }

    static dispatcher_t& dispatcher() { static dispatcher_t d; return d; }
    static optimiser_t& optimiser() { static optimiser_t o; return o; }

    virtual size_t arity() const { return 3; }
    virtual ExpPtr param( const size_t& i ) const
    {
        assert( i < arity() );
        if( i == 0 ) return p1_;
        if( i == 1 ) return p2_;
        return p3_;
    }

protected:

    ExpPtr p1_;
    ExpPtr p2_;
    ExpPtr p3_;

};

//--------------------------------------------------------------------------------------------

/// Generates a Add expressions
class Add {
public:

    static std::string class_name() { return "Add"; }

    struct Op : public BinOp
    {
        Op( ExpPtr lhs, ExpPtr rhs ) : BinOp(lhs,rhs) {}
        virtual std::string opName() const { return Add::class_name(); }
    };

    static VarPtr eval_scalar_scalar( ExpPtr& lhs, ExpPtr& rhs )
    {
        assert( lhs->type() == Exp::SCALAR );
        assert( rhs->type() == Exp::SCALAR );
        return maths::scalar( lhs->as<Scalar>()->value() + rhs->as<Scalar>()->value() );
    }

    static VarPtr eval_vector_scalar( ExpPtr& v, ExpPtr& s )
    {
        return eval_scalar_vector(s,v);
    }

    static VarPtr eval_scalar_vector( ExpPtr& s, ExpPtr& v )
    {
        assert( s->type() == Exp::SCALAR );
        assert( v->type() == Exp::VECTOR );

        scalar_t lhs = s->as<Scalar>()->value();                     /// @todo could this be a static_cast?
        Vector::storage_t& rhs = v->as<Vector>()->ref_value();       /// @todo could this be a static_cast?

        Vector* res = new Vector( rhs.size() );
        Vector::storage_t& rv = res->ref_value();

        for( size_t i = 0; i < rv.size(); ++i )
            rv[i] = lhs + rhs[i];

        return VarPtr(res);
    }

    static VarPtr eval_vector_vector( ExpPtr& v1, ExpPtr& v2 )
    {
        assert( v1->type() == Exp::VECTOR );
        assert( v2->type() == Exp::VECTOR );

        Vector::storage_t& lhs = v1->as<Vector>()->ref_value();
        Vector::storage_t& rhs = v2->as<Vector>()->ref_value();

        assert( lhs.size() == rhs.size() );

        Vector* res = new Vector( rhs.size() );
        Vector::storage_t& rv = res->ref_value();

        for( size_t i = 0; i < rv.size(); ++i )
            rv[i] = lhs[i] + rhs[i];

        return VarPtr(res);
    }

    ExpPtr operator() ( ExpPtr p1, ExpPtr p2 ) { return ExpPtr( new Add::Op(p1,p2) ); }

    struct Register
    {
        Register()
        {
            std::string add( Add::class_name() );
            BinOp::dispatcher()[ boost::make_tuple( Exp::SCALAR, Exp::SCALAR, add ) ] = &(Add::eval_scalar_scalar);
            BinOp::dispatcher()[ boost::make_tuple( Exp::SCALAR, Exp::VECTOR, add ) ] = &(Add::eval_scalar_vector);
            BinOp::dispatcher()[ boost::make_tuple( Exp::VECTOR, Exp::SCALAR, add ) ] = &(Add::eval_vector_scalar);
            BinOp::dispatcher()[ boost::make_tuple( Exp::VECTOR, Exp::VECTOR, add ) ] = &(Add::eval_vector_vector);
        }
    };
};

// version with stand alone functions

ExpPtr add( ExpPtr l, ExpPtr r ) { return ExpPtr( new Add::Op(l,r) ); }
ExpPtr add( Exp&   l, ExpPtr r ) { return ExpPtr( new Add::Op(l.self(),r) ); }
ExpPtr add( ExpPtr l, Exp&   r ) { return ExpPtr( new Add::Op(l,r.self()) ); }
ExpPtr add( Exp&   l, Exp&   r ) { return ExpPtr( new Add::Op(l.self(),r.self()) ); }

static Add::Register add_register;

//--------------------------------------------------------------------------------------------

class ProdOptimser {
public:
    static ExpPtr optimise_var_op( ExpPtr x )
    {
        assert( x->isOpr() );
        Opr& op = *(x->as<Opr>());

        assert( op.opName() == "Prod" );
        assert( op.arity() == 2 );

        ExpPtr p0 = op.param(0);
        ExpPtr p1 = op.param(1);

        assert( p0->isTerminal() );
        assert( p1->isOpr()  );

        OprPtr op1 = p1->as<Opr>();

        if( op1->opName()  == "Add" )
        {
            assert( op1->arity() == 2);
            return prod_add( p0, op1->param(0), op1->param(1) );
        }

        return x; // no optimisation was possible
    }
};

//--------------------------------------------------------------------------------------------

/// Generates a Prod expressions
class Prod {
public:

    static std::string class_name() { return "Prod"; }

    /// Represents a Prod expression
    struct Op : public BinOp
    {
        Op( ExpPtr lhs, ExpPtr rhs ) : BinOp(lhs,rhs) {}
        virtual std::string opName() const { return Prod::class_name(); }
    };

    static VarPtr eval_scalar_scalar( ExpPtr& lhs, ExpPtr& rhs )
    {
        assert( lhs->type() == Exp::SCALAR );
        assert( rhs->type() == Exp::SCALAR );
        return maths::scalar( lhs->as<Scalar>()->value() * rhs->as<Scalar>()->value() );
    }

    static VarPtr eval_scalar_vector( ExpPtr& s, ExpPtr& v )
    {
        assert( s->type() == Exp::SCALAR );
        assert( v->type() == Exp::VECTOR );

        scalar_t lhs = s->as<Scalar>()->value();
        Vector::storage_t& rhs = v->as<Vector>()->ref_value();

        Vector* res = new Vector( rhs.size() );
        Vector::storage_t& rv = res->ref_value();

        for( size_t i = 0; i < rv.size(); ++i )
            rv[i] = lhs * rhs[i];

        return VarPtr(res);
    }

    static VarPtr eval_vector_scalar( ExpPtr& v, ExpPtr& s )
    {
        return eval_scalar_vector(s,v);
    }

    static VarPtr eval_vector_vector( ExpPtr& v1, ExpPtr& v2 )
    {
        assert( v1->type() == Exp::VECTOR );
        assert( v2->type() == Exp::VECTOR );

        Vector::storage_t& lhs = v1->as<Vector>()->ref_value();
        Vector::storage_t& rhs = v2->as<Vector>()->ref_value();

        assert( lhs.size() == rhs.size() );

        Vector* res = new Vector( rhs.size() );
        Vector::storage_t& rv = res->ref_value();

        for( size_t i = 0; i < rv.size(); ++i )
            rv[i] = lhs[i] * rhs[i];

        return VarPtr(res);
    }

    ExpPtr operator() ( ExpPtr l, ExpPtr r ) { return ExpPtr( new Prod::Op(l,r) ); }

    struct Register
    {
        Register()
        {
            std::string prod( Prod::class_name() );

            BinOp::dispatcher()[ boost::make_tuple( Exp::SCALAR, Exp::SCALAR, prod ) ] = &(Prod::eval_scalar_scalar);
            BinOp::dispatcher()[ boost::make_tuple( Exp::SCALAR, Exp::VECTOR, prod ) ] = &(Prod::eval_scalar_vector);
            BinOp::dispatcher()[ boost::make_tuple( Exp::VECTOR, Exp::SCALAR, prod ) ] = &(Prod::eval_vector_scalar);
            BinOp::dispatcher()[ boost::make_tuple( Exp::VECTOR, Exp::VECTOR, prod ) ] = &(Prod::eval_vector_vector);

            BinOp::optimiser()[ boost::make_tuple( Exp::SCALAR, Exp::OP, prod ) ] = &(ProdOptimser::optimise_var_op);
            BinOp::optimiser()[ boost::make_tuple( Exp::VECTOR, Exp::OP, prod ) ] = &(ProdOptimser::optimise_var_op);
        }
    };
};

static Prod::Register prod_register;

// version with stand alone functions

ExpPtr prod( ExpPtr l, ExpPtr r ) { return ExpPtr( new Prod::Op(l,r) ); }
ExpPtr prod( Exp&   l, ExpPtr r ) { return ExpPtr( new Prod::Op(l.self(),r) ); }
ExpPtr prod( ExpPtr l, Exp&   r ) { return ExpPtr( new Prod::Op(l,r.self()) ); }
ExpPtr prod( Exp&   l, Exp&   r ) { return ExpPtr( new Prod::Op(l.self(),r.self()) ); }

//--------------------------------------------------------------------------------------------

/// Generates a Linear combination of vectors
class Linear : public Var {
public:

    Linear( ExpPtr e )
    {
        assert( e->arity() == 2 );

        ExpPtr left  = e->param(0);
        ExpPtr right = e->param(1);

        assert( left->arity() == 2 );
        assert( right->arity() == 2 );

        s1_ = left->param(0);
        v1_ = left->param(1);
        s2_ = right->param(0);
        v2_ = right->param(1);

        /// should check vectors of smae size
    }

    virtual void print( std::ostream& o ) const
    {
        o << "linear(" << *s1_ << "," << *v1_ << "," << *s2_ << "," << *v2_ << ")";
    }

    virtual std::string signature() const
    {
        return "l";
    }

    virtual size_t size() const { assert( false ); return 0; }

    virtual Exp::Type type() const { assert( false ); }
    virtual std::string type_name() const { assert( false ); }

    virtual bool isTerminal() const { assert( false ); }

private:

    ExpPtr s1_;
    ExpPtr v1_;
    ExpPtr s2_;
    ExpPtr v2_;

};

class Reducer {
public:

    typedef std::map<std::string,Reducer*> reducers_t;

    static ExpPtr build( ExpPtr e )
    {
        std::string signature = e->signature();

        reducers_t& reducer = reducers();
        std::map<std::string,Reducer*>::const_iterator itr = reducer.find(signature);
        if( itr == reducer.end() )
            return e;
        return (*itr).second->make(e);
    }


protected:

    Reducer( const std::string& signature )
    {
        reducers()[signature] = this;
    }

    virtual ExpPtr make( ExpPtr ) const = 0;

    static reducers_t& reducers()
    {
        static reducers_t reducers_;
        return reducers_;
    }
};

template< typename T>
class Optimiser : public Reducer {
public:
    Optimiser( const std::string& signature ) : Reducer(signature) {}
 private:
    ExpPtr make( ExpPtr e ) const
    {
        return ExpPtr( new T(e) );
    }
};

static Optimiser<Linear> optimLinear("Add(Prod(s,v),Prod(s,v))");

//--------------------------------------------------------------------------------------------

/// Generates a ProdAdd expressions
class ProdAdd {
public:

    static std::string class_name() { return "ProdAdd"; }

    /// Represents a ProdAdd expression
    struct Op : public TriOp
    {
        Op( ExpPtr p1, ExpPtr p2, ExpPtr p3 ) : TriOp(p1,p2,p3) {}
        virtual std::string opName() const { return ProdAdd::class_name(); }
    };

    static VarPtr eval_svv( ExpPtr& ps, ExpPtr& pv1, ExpPtr& pv2  )
    {
        assert( ps->type() == Exp::SCALAR );
        assert( pv1->type() == Exp::VECTOR );
        assert( pv2->type() == Exp::VECTOR );

        scalar_t s = ps->as<Scalar>()->value();

        Vector::storage_t& v1 = pv1->as<Vector>()->ref_value();
        Vector::storage_t& v2 = pv2->as<Vector>()->ref_value();

        assert( v1.size() == v2.size() );

        Vector* res = new Vector( v1.size() );
        Vector::storage_t& rv = res->ref_value();

        for( size_t i = 0; i < rv.size(); ++i )
            rv[i] = s * ( v1[i] + v2[i] );

        return VarPtr(res);
    }

    static VarPtr eval_vvv( ExpPtr& pv1, ExpPtr& pv2, ExpPtr& pv3  )
    {
        assert( pv1->type() == Exp::VECTOR );
        assert( pv2->type() == Exp::VECTOR );
        assert( pv3->type() == Exp::VECTOR );

        Vector::storage_t& v1 = pv1->as<Vector>()->ref_value();
        Vector::storage_t& v2 = pv2->as<Vector>()->ref_value();
        Vector::storage_t& v3 = pv3->as<Vector>()->ref_value();

        assert( v1.size() == v2.size() );
        assert( v1.size() == v3.size() );

        Vector* res = new Vector( v1.size() );
        Vector::storage_t& rv = res->ref_value();

        std::cout << "running optimized prod_add svv" << std::endl;

        for( size_t i = 0; i < rv.size(); ++i )
            rv[i] = v1[i] * ( v2[i] + v3[i] );

        return VarPtr(res);
    }

    /// Generic prod_add implementation based on calling first add() then prod()
    static VarPtr eval_ggg( ExpPtr& p1, ExpPtr& p2, ExpPtr& p3  )
    {
        return prod(p1,add(p2,p3))->eval();
    }

    ExpPtr operator() ( ExpPtr p1, ExpPtr p2, ExpPtr p3 ) { return ExpPtr( new ProdAdd::Op(p1,p2,p3) ); }

    struct Register
    {
        Register()
        {
            std::string op( ProdAdd::class_name() );

            // generic cases
            TriOp::dispatcher()[ boost::make_tuple( Exp::SCALAR, Exp::SCALAR, Exp::SCALAR, op ) ] = &(ProdAdd::eval_ggg);
            TriOp::dispatcher()[ boost::make_tuple( Exp::SCALAR, Exp::SCALAR, Exp::VECTOR, op ) ] = &(ProdAdd::eval_ggg);

            TriOp::dispatcher()[ boost::make_tuple( Exp::SCALAR, Exp::VECTOR, Exp::SCALAR, op ) ] = &(ProdAdd::eval_ggg);
//            TriOp::dispatcher()[ boost::make_tuple( Exp::SCALAR, Exp::VECTOR, Exp::VECTOR, op ) ] = &(ProdAdd::eval_ggg);

            TriOp::dispatcher()[ boost::make_tuple( Exp::VECTOR, Exp::SCALAR, Exp::SCALAR, op ) ] = &(ProdAdd::eval_ggg);
            TriOp::dispatcher()[ boost::make_tuple( Exp::VECTOR, Exp::SCALAR, Exp::VECTOR, op ) ] = &(ProdAdd::eval_ggg);

            TriOp::dispatcher()[ boost::make_tuple( Exp::VECTOR, Exp::VECTOR, Exp::SCALAR, op ) ] = &(ProdAdd::eval_ggg);
//            TriOp::dispatcher()[ boost::make_tuple( Exp::VECTOR, Exp::VECTOR, Exp::VECTOR, op ) ] = &(ProdAdd::eval_ggg);

            // special cases
            TriOp::dispatcher()[ boost::make_tuple( Exp::SCALAR, Exp::VECTOR, Exp::VECTOR, op ) ] = &(ProdAdd::eval_svv);
            TriOp::dispatcher()[ boost::make_tuple( Exp::VECTOR, Exp::VECTOR, Exp::VECTOR, op ) ] = &(ProdAdd::eval_vvv);
        }
    };

};

static ProdAdd::Register prodadd_register;

// version with stand alone functions

ExpPtr prod_add( ExpPtr p1, ExpPtr p2, ExpPtr p3 ) { return ExpPtr( new ProdAdd::Op(p1,p2,p3) ); }

//--------------------------------------------------------------------------------------------

ExpPtr operator+ ( VarPtr p1, VarPtr p2 ) { return add( p1, p2 ); }
ExpPtr operator+ ( VarPtr p1, ExpPtr p2 ) { return add( p1, p2 ); }
ExpPtr operator+ ( ExpPtr p1, VarPtr p2 ) { return add( p1, p2 ); }
ExpPtr operator+ ( ExpPtr p1, ExpPtr p2 ) { return add( p1, p2 ); }

ExpPtr operator* ( VarPtr p1, VarPtr p2 ) { return prod( p1, p2 ); }
ExpPtr operator* ( VarPtr p1, ExpPtr p2 ) { return prod( p1, p2 ); }
ExpPtr operator* ( ExpPtr p1, VarPtr p2 ) { return prod( p1, p2 ); }
ExpPtr operator* ( ExpPtr p1, ExpPtr p2 ) { return prod( p1, p2 ); }

//--------------------------------------------------------------------------------------------

ExpPtr Exp::reduce(){ return Reducer::build( shared_from_this() ); }

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
