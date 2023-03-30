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


#include "grit/pl/Octahedral.h"

#include <map>

#include "grit/exception.h"


namespace grit::pl {


const Pl::pl_type& Octahedral::pl(size_t N) {
    static Octahedral pls;

    const auto& pl = pls.get_pl(N);
    ASSERT(pl.size() == 2 * N);

    return pl;
}


const Pl::pl_type& Octahedral::get_pl(size_t N) const {
    ASSERT(N > 0);

    static std::map<size_t, pl_type> __cache;
    if (auto pl = __cache.find(N); pl != __cache.end()) {
        return pl->second;
    }

    pl_type pl(N * 2);

    pl_value_type p = 20;
    for (size_t i = 0, j = 2 * N - 1; i < N; ++i, --j) {
        pl[i] = pl[j] = p;
        p += 4;
    }

    __cache[N] = std::move(pl);
    return __cache[N];
}


const Pl::pl_type& Octahedral::get_pl_half(size_t N) const {
    NOTIMP;
}


}  // namespace grit::pl
