// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstdint>
#include <iosfwd>
#include <string>

namespace eckit::codec {

struct Bytes {
public:

    explicit Bytes(size_t bytes) : bytes_(bytes) {}

    explicit operator size_t() const { return bytes_; }

    std::string str(int decimals = 2, int width = 7) const;

    void print(std::ostream& out, int decimals = 2, int width = 7) const;

    friend std::ostream& operator<<(std::ostream&, const Bytes&);

private:

    size_t bytes_;
};

}  // namespace eckit::codec
