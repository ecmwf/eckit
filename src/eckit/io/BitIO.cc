/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/io/BitIO.h"
#include "eckit/io/DataHandle.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static unsigned char masks[] =  {
    0x00,
    0x80,
    0xc0,
    0xe0,
    0xf0,
    0xf8,
    0xfc,
    0xfe,
};

BitIO::BitIO(DataHandle& handle):
    handle_(handle),
    buffer_(0),
    used_(0),
    bits_(0),
    count_(0) {

}

size_t BitIO::count() const {
    return count_;
}

void BitIO::write(unsigned long code, size_t nbits) {
    if (nbits + used_ > sizeof(buffer_) * 8L)
    {
        while (used_ >= 8L)
        {
            unsigned char c =  (buffer_ >> (8L * sizeof(buffer_) - 8L));

            ASSERT(handle_.write(&c, 1) == 1);
            count_++;

            buffer_ <<= 8L;
            used_    -= 8L;
        }
    }

    code <<= (sizeof(code) * 8L - nbits - used_);
    buffer_ += code;
    used_   += nbits;
}

void BitIO::flush() {
    while (used_ > 0)
    {
        unsigned char c =  (buffer_ >> (8L * sizeof(buffer_) - 8L));

        ASSERT(handle_.write(&c, 1) == 1);
        count_++;

        buffer_ <<= 8L;
        used_    -= 8L;
    }
}


//=============================

unsigned long BitIO::read(size_t nbits, unsigned long EOF_MARKER) {
    unsigned long result = 0;
    size_t m = (1 << nbits) - 1;

    while (used_ > 0 && nbits > 0) {
        size_t s = std::min(used_, nbits);

        result = (bits_ & masks[s]) >> (8 - s);

        nbits -= s;
        used_ -= s;
        bits_ <<= s;
    }

    while (nbits >= 8 ) {

        unsigned char c;
        if (handle_.read(&c, 1) <= 0) {
            return EOF_MARKER;
        }

        result <<= 8;
        result |= c;
        nbits -= 8;
    }

    if (nbits > 0) {
        ASSERT(nbits < 8);
        ASSERT(used_ == 0);


        if (handle_.read(&bits_, 1) <= 0) {
            return EOF_MARKER;
        }

        result <<= nbits;
        result |= ((bits_ & masks[nbits] ) >> (8 - nbits));

        used_ = 8 - nbits;
        bits_ <<= nbits;

    }

    ASSERT(result <= m);

    return result;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
