/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <bitset>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/BitIO.h"
#include "eckit/io/DataHandle.h"


namespace eckit {


//----------------------------------------------------------------------------------------------------------------------

BitIO::BitIO(DataHandle& handle, bool padded) :
    handle_(handle), buffer_(0), used_(0), bits_(0), write_(false), eof_(false), padded_(padded), opened_(false) {}

BitIO::~BitIO() {
    if (write_) {
        flush();
    }
    if (opened_) {
        handle_.close();
    }
}

static unsigned char masks[] = {
    0x00,
    0x01,
    0x03,
    0x07,
    0x0F,
    0x1F,
    0x3F,
    0x7F,
    0xFF,
};


void BitIO::write(size_t code, size_t nbits) {
    const size_t BITS = sizeof(buffer_) * 8;
    write_            = true;

    if (!opened_) {
        handle_.openForWrite(0);
        opened_ = true;
    }

    while (nbits) {
        if (used_ == BITS) {
            flush();
            ASSERT(used_ == 0);
            ASSERT(buffer_ == 0);
        }

        size_t s = std::min(std::min(nbits, BITS - used_), size_t(8));

        buffer_ <<= s;
        buffer_ |= (code >> (nbits - s)) & masks[s];
        // code <<= s;

        used_ += s;
        nbits -= s;
        bits_ += s;
    }
}


void BitIO::flush() {
    // const size_t BITS = sizeof(buffer_) * 8;


    // buffer_ <<= (BITS - used_);

    // std::cout << "flush " << used_
    // << " " << std::bitset<BITS>(buffer_) << std::endl;
    // std::cout << "bits " << bitCount() << std::endl;

    size_t nbits = used_;
    // ccc-cccc.cccc
    while (nbits) {
        size_t s = std::min(nbits, size_t(8));

        unsigned char c = (buffer_ >> (nbits - s)) & masks[s];

        c <<= (8 - s);

        // std::cout << "write " << std::bitset<8>(c);
        // if(::isprint(c)) {
        // std::cout << " " << char(c);

        // }
        // std::cout <<  ' ' << s <<std::endl;

        ASSERT(handle_.write(&c, 1) == 1);

        // buffer_ <<= s;
        nbits -= s;
    }

    buffer_ = 0;
    used_   = 0;
}


//----------------------------------------------------------------------------------------------------------------------


size_t BitIO::read(size_t nbits, size_t EOF_MARKER) {
    if (!opened_) {
        handle_.openForRead();
        opened_ = true;
    }

    size_t result     = 0;
    const size_t BITS = sizeof(buffer_) * 8;
    size_t count      = 0;
    size_t asked      = nbits;

    while (nbits) {
        if (used_ == 0) {
            if (!eof_) {
                buffer_ = 0;

                for (size_t i = 0; i < sizeof(buffer_); i++) {
                    unsigned char c;
                    if (handle_.read(&c, 1) <= 0) {
                        eof_ = true;
                        break;
                    }
                    buffer_ <<= 8;
                    buffer_ |= c;
                    used_ += 8;
                    bits_ += 8;
                }
            }

            if (used_ == 0) {
                if (padded_ && count > 0) {
                    padded_ = false;
                    return result << (asked - count);
                }

                if (EOF_MARKER) {
                    return EOF_MARKER;
                }
                std::ostringstream oss;
                oss << "Failed to read from " << handle_;
                throw eckit::ReadError(oss.str());
            }

            buffer_ <<= (BITS - used_);
        }

        size_t s = std::min(std::min(nbits, used_), size_t(8));

        result <<= s;
        result |= (buffer_ >> (BITS - s)) & masks[s];
        buffer_ <<= s;

        used_ -= s;
        nbits -= s;
        count += s;
    }

    return result;
}

size_t BitIO::bitCount() const {
    return bits_;
}


size_t BitIO::byteCount() const {
    return (bits_ + 7) / 8;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
