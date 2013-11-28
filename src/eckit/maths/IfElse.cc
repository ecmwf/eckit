/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/maths/IfElse.h"
#include "eckit/maths/Boolean.h"
#include "eckit/maths/Scope.h"


namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

IfElse::IfElse( ExpPtr f,  ExpPtr i, ExpPtr e ) : Function()
{
    push_back(f);
    push_back(i);
    push_back(e);
}

string IfElse::returnSignature() const
{
    string i = args(1)->returnSignature();
    string e = args(2)->returnSignature();

    if(i == e) {
        return i;
    }
    else {
        return "?";
    }
}

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


void IfElse::asCode(std::ostream&o) const
{
    o << "maths::ifelse("; printArgs(o); o <<")";
}

ExpPtr IfElse::cloneWith(args_t &a) const {
    ASSERT(a.size() == 3);
    return ExpPtr(new IfElse(a[0], a[1], a[2]));
}
//--------------------------------------------------------------------------------------------

ExpPtr ifelse( ExpPtr f,  ExpPtr i, ExpPtr e )
{
    return ExpPtr( new IfElse(f, i, e) );
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
