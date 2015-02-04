/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/xpr/IfElse.h"
#include "eckit/xpr/Boolean.h"
#include "eckit/xpr/Scope.h"


namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

IfElse::IfElse( ExpPtr f,  ExpPtr i, ExpPtr e ) : Function()
{
    push_back(f);
    push_back(i);
    push_back(e);
}

IfElse::IfElse(args_t& a) : Function(a)
{
    ASSERT( a.size() == 3 );
}

IfElse::IfElse(Stream &s) : Function(s) {}

ExpPtr IfElse::evaluate( Scope &ctx ) const
{
    //std::cout << "evalute " << *this << " with " << ctx << std::endl;

    ExpPtr v = args(0, ctx, true);

    bool b = Boolean::extract( ctx, v );

    if(b) {
        return args(1, ctx, true);
    }
    else {
        return args(2, ctx, true);
    }
}

ExpPtr IfElse::cloneWith(args_t& a) const
{
    return ExpPtr(new IfElse(a));
}

//--------------------------------------------------------------------------------------------

ClassSpec IfElse::classSpec_ = {
    &Function::classSpec(),
    IfElse::nodeName().c_str(),
};

Reanimator< IfElse > IfElse::reanimator_;

//--------------------------------------------------------------------------------------------

ExpPtr ifelse( ExpPtr f,  ExpPtr i, ExpPtr e )
{
    return ExpPtr( new IfElse(f, i, e) );
}

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
