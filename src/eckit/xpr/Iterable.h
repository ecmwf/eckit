/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Iterable.h
/// @author Florian Rathgeber
/// @date March 2015

#ifndef eckit_xpr_Iterable_h
#define eckit_xpr_Iterable_h

#include "eckit/xpr/Expression.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

/// Abstract iterable interface for Expressions

class Iterable {

public: // methods

  virtual ~Iterable();
  virtual ExpPtr next() = 0;

};

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
