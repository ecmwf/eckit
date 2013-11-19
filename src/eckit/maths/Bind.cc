/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <strstream>

#include "eckit/maths/Bind.h"
#include "eckit/maths/List.h"
#include "eckit/maths/Boolean.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

Bind::Bind( size_t i, ExpPtr f, ExpPtr e ) : Func(),
    idx_(i)
{
    ASSERT( i > 0 );
    ASSERT( i <= f->arity() );

    args_.push_back(f);
    args_.push_back(e);
}

string Bind::type_name() const
{
    std::ostringstream os;
    os << Bind::class_name() << "<" << idx_ << ">";
    return os.str();
}

string Bind::ret_signature() const
{
    return args_[0]->ret_signature();
}

ValPtr Bind::evaluate( context_t& ctx )
{    
    ExpPtr f = param(0,&ctx);
    ExpPtr e = param(1,&ctx);

    f->param( (idx_ - 1), e );

    return f->evaluate(ctx);
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
