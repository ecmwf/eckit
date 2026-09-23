// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/net/NetMask.h"

#include <arpa/inet.h>

#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/net/IPAddress.h"
#include "eckit/utils/Tokenizer.h"
#include "eckit/utils/Translator.h"

// Cray C++ compiler should not try to optimize this code
#if _CRAYC
#pragma _CRI noopt
#endif

namespace eckit::net {

static Translator<std::string, size_t> s2l;

// Not the most efficient or elegant, but should not affect performances
static std::string parse(const std::string& cidr, size_t n) {
    static Tokenizer parse("/");
    std::vector<std::string> v;
    parse(cidr, v);
    ASSERT(v.size() == 2);
    return v[n];
}

NetMask::NetMask(const std::string& cidr) : network_(parse(cidr, 0)), bits_(s2l(parse(cidr, 1))) {

    ASSERT(bits_ > 0);
    ASSERT(bits_ < 32);
}


bool NetMask::contains(const IPAddress& addr) const {
    return !((addr.address().s_addr ^ network_.address().s_addr) & htonl(0xFFFFFFFFu << (32 - bits_)));
}

void NetMask::print(std::ostream& s) const {
    s << network_ << '/' << bits_;
}

}  // namespace eckit::net
