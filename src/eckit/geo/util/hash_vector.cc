// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <cstddef>
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
