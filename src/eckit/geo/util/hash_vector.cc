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


#include <vector>

#include "eckit/eckit_config.h"
#include "eckit/utils/MD5.h"

#if eckit_LITTLE_ENDIAN
#else
#include "eckit/utils/ByteSwap.h"
#endif


namespace eckit::geo::util {


template <typename T>
void hash_vector(MD5& hash, const std::vector<T>& v) {
    const auto len = static_cast<long>(v.size() * sizeof(T));

#if eckit_LITTLE_ENDIAN
    hash.add(v.data(), len);
#else
    auto w = v;
    byteswap(w);
    hash.add(w.data(), len);
#endif
}


void hash_vector_double(MD5& hash, const std::vector<double>& v) {
    hash_vector(hash, v);
}


void hash_vector_size_t(MD5& hash, const std::vector<size_t>& v) {
    hash_vector(hash, v);
}


}  // namespace eckit::geo::util
