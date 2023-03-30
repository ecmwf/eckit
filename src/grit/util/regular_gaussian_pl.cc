/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <map>

#include "grit/exception.h"
#include "grit/util.h"


namespace grit::util {


pl_type::value_type regular_gaussian_pl(size_t N) {
    ASSERT(N > 0);

    return 4 * N;
}


}  // namespace grit::util
