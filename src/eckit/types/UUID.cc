/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <algorithm>
#include <cctype>


#include "eckit/exception/Exceptions.h"
#include "eckit/memory/Zero.h"
#include "eckit/types/UUID.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static char to_char(size_t i) {
    if (i <= 9)
        return static_cast<char>('0' + i);
    else
        return static_cast<char>('a' + (i - 10));
}

//----------------------------------------------------------------------------------------------------------------------

UUID::UUID() {
    zero(data_);
}

UUID::UUID(const std::string& s) {
    ASSERT(s.size() == hexSize());
    fromString(s);
}

UUID::~UUID() {}

bool UUID::isNil() const {
    for (size_t i = 0; i < size(); ++i) {
        if (data_[i] != 0U)
            return false;
    }
    return true;
}

std::string UUID::asString() const {
    std::string result;
    result.reserve(hexSize());

    std::size_t i = 0;
    for (UUID::const_iterator it = begin(); it != end(); ++it, ++i) {
        const size_t hi = ((*it) >> 4) & 0x0F;
        result += to_char(hi);

        const size_t lo = (*it) & 0x0F;
        result += to_char(lo);
    }
    return result;
}

void UUID::fromString(const std::string& s) {
    std::stringstream str(s);
    str >> *this;
}

void UUID::print(std::ostream& s) const {
    s << asString();
}

UUID::operator std::string() const {
    return asString();
}

std::istream& operator>>(std::istream& is, UUID& u) {
    typedef std::istream::char_type char_type;

    const std::istream::sentry ok(is);
    if (ok) {
        unsigned char data[16];

        char_type xdigits[]          = "0123456789ABCDEF";
        char_type* const xdigits_end = xdigits + 16;

        char_type c;
        for (std::size_t i = 0; i < u.size() && is; ++i) {
            is >> c;
            c = toupper(c);

            char_type* f = std::find(xdigits, xdigits_end, c);
            if (f == xdigits_end) {
                is.setstate(std::ios_base::failbit);
                break;
            }

            unsigned char byte = static_cast<unsigned char>(std::distance(&xdigits[0], f));

            is >> c;
            c = toupper(c);
            f = std::find(xdigits, xdigits_end, c);
            if (f == xdigits_end) {
                is.setstate(std::ios_base::failbit);
                break;
            }

            byte <<= 4;
            byte |= static_cast<unsigned char>(std::distance(&xdigits[0], f));

            data[i] = byte;
        }

        if (is) {
            std::copy(data, data + 16, u.begin());
        }
    }
    return is;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
