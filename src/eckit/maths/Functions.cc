/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/maths/Functions.h"

namespace eckit {
namespace maths {

//----------------------------------------------------------------------------------------------------------------------

size_t roundToMultiple(size_t x, size_t n) {
    return ((x + n - 1) / n) * n;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

