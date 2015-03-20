/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Countable.h
/// @author Florian Rathgeber
/// @date March 2015

#ifndef eckit_xpr_Countable_h
#define eckit_xpr_Countable_h

#include "eckit/xpr/Iterable.h"

namespace eckit {
namespace xpr {

//-----------------------------------------------------------------------------

/// Iterator wrapper for finite Expressions, implementing the iterable interface

class Countable : public Iterable {

public: // methods

    Countable( ExpPtr expr );

    virtual ExpPtr next();

private: // members
    ExpPtr expr_;
    size_t counter_;
};

//-----------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
