/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Iterator.h
/// @author Florian Rathgeber
/// @date March 2015

#ifndef eckit_xpr_Iterator_h
#define eckit_xpr_Iterator_h

#include "eckit/xpr/Iterable.h"
#include "eckit/xpr/Undef.h"

namespace eckit {
namespace xpr {

//-----------------------------------------------------------------------------

/// Iterator adaptor for iterable and countable Expressions

class Iterator : public Iterable {

    /// Iterator wrapper for finite Expressions, implementing the iterable interface

    struct Countable : Iterable {

        Countable( ExpPtr expr ) : expr_( expr ), counter_( 0 ) {}

        virtual ExpPtr next()
        {
            if (counter_ >= expr_->arity())
                return ExpPtr( new Undef() );
            return expr_->args( counter_++ );
        }

    private: // members
        ExpPtr expr_;
        size_t counter_;
    };

public: // methods

    Iterator( ExpPtr expr );

    virtual ~Iterator();

    virtual ExpPtr next();

private:
    Iterable* it_;
    bool free_;
};

//-----------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
