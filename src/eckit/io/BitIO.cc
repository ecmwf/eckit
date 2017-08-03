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

BitIO::BitIO(DataHandle& handle):
    handle_(handle),
    used_(0),
    buffer_(0),
    count_(0),
    write_(false) {

}

BitIO::~BitIO() {
    if (write_) {
        flush();
    }
}

size_t BitIO::count() const {
    return count_;
}

static unsigned char masks[] =  {
    0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF,
};


void BitIO::write(size_t code, size_t nbits) {

    const size_t BITS = sizeof(buffer_) * 8;
    write_ = true;

    while (nbits) {

        if (used_ == BITS) {
            ASSERT(handle_.write(&buffer_, sizeof(buffer_)) == sizeof(buffer_));
            used_ = 0;
            buffer_ = 0;
        }

        size_t s = std::min(std::min(nbits, BITS - used_), size_t(8));

        buffer_ <<= s;
        buffer_ |= (code >> (nbits - s)) & masks[s];
        code <<= s;

        used_ += s;
        nbits -= s;

    }

}


void BitIO::flush() {
    while (used_ > 0)
    {
        unsigned char c =  (buffer_ >> (8L * sizeof(buffer_) - 8L));

        ASSERT(handle_.write(&c, 1) == 1);
        count_++;

        buffer_ <<= 8L;
        used_    -= std::min(size_t(8), used_);
    }
}


//=============================


size_t BitIO::read(size_t nbits, size_t EOF_MARKER) {

    size_t result  = 0;
    const size_t BITS = sizeof(buffer_) * 8;

    while (nbits) {

        if (used_ == 0) {

            long n = handle_.read(&buffer_, sizeof(buffer_));
            if (n <= 0) {
                if (EOF_MARKER) {
                    return EOF_MARKER;
                }
                std::ostringstream oss;
                oss << "Failed to read from " << handle_;
                throw eckit::ReadError(oss.str());
            }
            used_ = n * 8;
        }

        size_t s = std::min(std::min(nbits, used_), size_t(8));

        result <<= s;
        result |= (buffer_ >> (BITS - s)) & masks[s];
        buffer_ <<= s;

        used_ -= s;
        nbits -= s;

    }

    return result;


}


//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
