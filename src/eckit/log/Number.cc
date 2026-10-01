// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <sstream>

#include "eckit/log/Number.h"

namespace eckit::log {

//----------------------------------------------------------------------------------------------------------------------

std::string Number::printBinary(W n) {
    std::stringstream ss;
    printBinary(ss, n);
    return ss.str();
}

std::string Number::printHexadecimal(W n) {
    std::stringstream ss;
    printHexadecimal(ss, n);
    return ss.str();
}

void Number::printHexadecimal(std::ostream& ss, W n) {
    ss << std::hex << n << std::dec;
}

void Number::printBinary(std::ostream& ss, W n) {
    unsigned char* s = reinterpret_cast<unsigned char*>(&n);

    bool oneSeen = false;

    int endianTest = 1;
    if (*reinterpret_cast<char*>(&endianTest)) {
        for (int i = sizeof(W) - 1; i >= 0; --i) {
            unsigned char c = s[i];
            for (unsigned char mask = 1 << 7; mask; mask >>= 1) {
                if (c & mask) {
                    ss << '1';
                    oneSeen = true;
                }
                else {
                    if (oneSeen) {
                        ss << '0';
                    }
                }
            }
        }
    }
    else {
        for (size_t i = 0; i < sizeof(W); ++i) {
            unsigned char c = s[i];
            for (unsigned char mask = 1 << 7; mask; mask >>= 1) {
                if (c & mask) {
                    ss << '1';
                    oneSeen = true;
                }
                else {
                    if (oneSeen) {
                        ss << '0';
                    }
                }
            }
        }
    }

    if (!oneSeen) {
        ss << '0';
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::log
