/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/xpr/Countable.h"
#include "eckit/xpr/Undef.h"

namespace eckit {
namespace xpr {

//-----------------------------------------------------------------------------

Countable::Countable(ExpPtr expr) : expr_( expr ), counter_( 0 ) {}

ExpPtr Countable::next()
{
    if (counter_ >= expr_->arity())
        return ExpPtr( new Undef() );
    return expr_->args( counter_++ );
}

//-----------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
