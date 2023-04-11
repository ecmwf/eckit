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
#include <utility>

#include "grit/exception.h"
#include "grit/types.h"


namespace grit::util {


const pl_type& reduced_gaussian_octahedral_pl(size_t N) {
    ASSERT(N > 0);

    static std::map<size_t, pl_type> __cache;
    if (auto pl = __cache.find(N); pl != __cache.end()) {
        return pl->second;
    }

    pl_type pl(N * 2);

    pl_type::value_type p = 20;
    for (size_t i = 0, j = 2 * N - 1; i < N; ++i, --j) {
        pl[i] = pl[j] = p;
        p += 4;
    }

    __cache[N] = std::move(pl);
    return __cache[N];
}


}  // namespace grit::util
