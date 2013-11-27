/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sstream>

#include "eckit/maths/Expression.h"
#include "eckit/maths/Value.h"
#include "eckit/maths/Scope.h"
#include "eckit/maths/Undef.h"
#include "eckit/maths/Param.h"
#include "eckit/maths/ParamDef.h"


namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

Expression::Expression()
{
}

Expression::Expression(const args_t &args) :
    args_(args)
{
}

Expression::~Expression()
{
}

ExpPtr Expression::eval(bool optimize) const
{
    Scope ctx("Expression::eval()");
    return ExpPtr( optimise(optimize)->evaluate(ctx));
}

ExpPtr Expression::eval(Scope& ctx) const
{
    // FIXME: We assume that the code is already optimised. Needs reviewing
    return /*optimise()->*/evaluate(ctx);
}

ExpPtr Expression::resolve(Scope & ctx) const
{
    return self();
}

ExpPtr Expression::optimise() const
{
    return self();
}

ExpPtr Expression::optimise(bool doit) const
{
    if(doit) {
         std::cout << "==============================" << std::endl;
        std::cout << "Optimise : " << *this << std::endl;

        // First check is the arguments can be option

        bool changed = false;
        args_t result;
        result.reserve(args_.size());
        for(size_t i = 0; i < args_.size(); ++i) {
            ExpPtr o = args_[i]->optimise(true);
            result.push_back(o);
            changed = changed || (o.get() != args_[i].get());
        }

        if(changed) {
            std::cout << "Arguments have changed" << std::endl;
            ExpPtr o = cloneWith(result);
            return o->optimise(true);
        }

        const Expression* p = this;
        std::cout << "Optimise : " << *this << std::endl;
        for(;;)
        {
            ExpPtr o = optimise();
            std::cout << "  - Optimise : " << *p << std::endl;
            if( o.get() == p ) {
                std::cout << "  -          : - " << std::endl;
                return o;
            }
            else {
                std::cout << "  -          : " << *o << std::endl;
                p = o.get();
            }
        }

    }
    return self();
}

bool Expression::optimiseArgs(args_t & result) const
{
    result.reserve(args_.size());
    bool changed = false;
    for(size_t i = 0; i < args_.size(); ++i) {
        ExpPtr o = args_[i]->optimise(true);
        result.push_back(o);
        changed = changed || (o.get() != args_[i].get());
    }
    return changed;
}

ExpPtr Expression::eval(ExpPtr e , bool optimize) const
{
    Scope ctx("Expression::eval( ExpPtr e )", e);
    return ExpPtr( optimise(optimize)->evaluate(ctx));
}

ExpPtr Expression::eval(ExpPtr a, ExpPtr b , bool optimize) const
{
    Scope ctx("Expression::eval(  ExpPtr a, ExpPtr b )", a, b);
    return ExpPtr( optimise(optimize)->evaluate(ctx));
}

ExpPtr Expression::param( size_t i) const
{
    ASSERT( i < args_.size() );
    ASSERT( args_[i] );
    return  args_[i];
}

ExpPtr Expression::param( size_t i, Scope& ctx ) const
{
    ASSERT( i < args_.size() );
    ASSERT( args_[i] );
    return  args_[i]->resolve(ctx);
}

string Expression::str() const
{
    std::ostringstream os;
    print(os);
    return os.str();
}


void Expression::printArgs(std::ostream& out) const
{
    size_t count = arity();
    for(size_t i = 0; i < count; ++i) {
        if(i) out << ", ";
        out << *args_[i];
    }
}

std::ostream& operator<<( std::ostream& os, const Expression& v)
{
    if(format(os) == maths::CodeFormat)
    {
        v.asCode(os);
    }
    else {
        v.print(os);
    }
    return os;
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
