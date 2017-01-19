/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   May 2016

#ifndef eckit_Functions_h
#define eckit_Functions_h

#include <cctype>
#include <cstdlib>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// @returns rounds x to multiple of n
size_t round(size_t x, size_t n);

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
