// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>
#include <string>

namespace eckit::codec {

class Checksum {
public:

    Checksum() = default;
    explicit Checksum(const std::string& checksum);
    bool available() const;
    std::string str() const;
    std::string str(size_t size) const;
    std::string algorithm() const { return algorithm_; }

private:

    std::string algorithm_;
    std::string checksum_;
};

std::string checksum(const void* buffer, size_t size, const std::string& algorithm = "");


}  // namespace eckit::codec
