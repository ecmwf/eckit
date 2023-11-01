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


#include "eckit/codec/detail/Checksum.h"

#include <string>

#include "eckit/codec/detail/Defaults.h"
#include "eckit/utils/Hash.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit::codec {

Checksum::Checksum(const std::string& checksum) {
    std::vector<std::string> tokens;
    Tokenizer tokenizer(':');
    Tokenizer{':'}(checksum, tokens);
    if (tokens.size() == 1) {
        algorithm_ = "none";
        checksum_  = "";
    }
    else {
        algorithm_ = tokens[0];
        checksum_  = tokens[1];
    }
}

bool Checksum::available() const {
    return !checksum_.empty() && algorithm_ != "none";
}

std::string Checksum::str() const {
    if (algorithm_.empty()) {
        return "";
    }
    return algorithm_ + ":" + checksum_;
}

std::string Checksum::str(size_t size) const {
    if (algorithm_.empty()) {
        return "";
    }
    return algorithm_ + ":" + checksum_.substr(0, std::min(size, checksum_.size()));
}

std::string checksum(const void* buffer, size_t size, const std::string& algorithm) {
    auto is_available = [](const std::string& alg) -> bool { return HashFactory::instance().has(alg); };

    auto hash = [&](const std::string& alg) -> std::string {
        std::unique_ptr<Hash> hasher(HashFactory::instance().build(alg));
        return alg + ":" + hasher->compute(buffer, static_cast<long>(size));
    };

    auto alg = algorithm.empty() ? defaults::checksum_algorithm() : algorithm;

    return hash(is_available(alg) ? alg : "none");
}


}  // namespace eckit::codec
