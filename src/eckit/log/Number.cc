/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sstream>

#include "eckit/log/Number.h"

namespace eckit {
namespace log {

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
    if (*reinterpret_cast<char*>(&endianTest))
        for (int i = sizeof(W) - 1; i >= 0; --i) {
            unsigned char c = s[i];
            for (unsigned char mask = 1 << 7; mask; mask >>= 1) {
                if (c & mask) {
                    ss << '1';
                    oneSeen = true;
                }
                else {
                    if (oneSeen)
                        ss << '0';
                }
            }
        }
    else
        for (size_t i = 0; i < sizeof(W); ++i) {
            unsigned char c = s[i];
            for (unsigned char mask = 1 << 7; mask; mask >>= 1) {
                if (c & mask) {
                    ss << '1';
                    oneSeen = true;
                }
                else {
                    if (oneSeen)
                        ss << '0';
                }
            }
        }

    if (!oneSeen)
        ss << '0';
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace log
}  // namespace eckit
