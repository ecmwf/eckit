/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/xpr/Iterator.h"

namespace eckit {
namespace xpr {

//-----------------------------------------------------------------------------

Iterator::Iterator(ExpPtr expr)
    : it_( dynamic_cast<Iterable*>(expr.get()) ), free_( false )
{
    // If expr is not itself Iterable, wrap it into a Countable iterator
    if (!it_) {
        it_ = new Countable( expr );
        free_ = true;
    }
}

Iterator::~Iterator()
{
    if (free_)
        delete it_;
}

ExpPtr Iterator::next()
{
    return it_->next();
}

//-----------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
