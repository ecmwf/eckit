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

#include <string>

#include "eckit/config/Resource.h"

namespace eckit::codec::defaults {

[[maybe_unused]] static const std::string& checksum_algorithm() {
    static const auto checksum = Resource<std::string>("eckit.codec.checksum.algorithm;$ECKIT_CODEC_CHECKSUM", "xxh64");
    return checksum;
}

[[maybe_unused]] static bool checksum_read() {
    static const auto checksum = Resource<bool>("eckit.codec.checksum.read;$ECKIT_CODEC_CHECKSUM_READ", true);
    return checksum;
}

[[maybe_unused]] static bool checksum_write() {
    static const auto checksum = Resource<bool>("eckit.codec.checksum.write;$ECKIT_CODEC_CHECKSUM_WRITE", true);
    return checksum;
}

[[maybe_unused]] static const std::string& compression_algorithm() {
    static const auto compression = Resource<std::string>("eckit.codec.compression;$ECKIT_CODEC_COMPRESSION", "none");
    return compression;
}


}  // namespace eckit::codec::defaults
