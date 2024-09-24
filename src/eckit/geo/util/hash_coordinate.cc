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


#pragma once

#include <vector>

#include "eckit/utils/ByteSwap.h"
#include "eckit/utils/MD5.h"


namespace eckit::geo::util {


void hash_coordinate(MD5& hash, const std::vector<double>& c, bool _byteswap) {
    if (const auto len = static_cast<long>(c.size() * sizeof(double)); _byteswap) {
        auto d = c;
        byteswap(d);
        hash.add(d.data(), len);
    }
    else {
        hash.add(c.data(), len);
    }
}


}  // namespace eckit::geo::util
