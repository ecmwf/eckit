/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define ECKIT_NO_DEPRECATION_WARNINGS

#include "eckit/memory/SharedPtr.h"
#include "eckit/exception/Exceptions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

namespace detail {
void assert_valid_SharedPtr(bool is_null) {
    ASSERT(!is_null);
}
}  // namespace detail

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
