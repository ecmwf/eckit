/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Statistics.h"

#include "eckit/log/BigNum.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Seconds.h"
#include "eckit/serialisation/Stream.h"

namespace eckit {

const size_t WIDTH = 32;

//----------------------------------------------------------------------------------------------------------------------
void Statistics::reportCount(std::ostream &out, const char *title, size_t value, const char *indent) {
    out << indent << title << std::setw(WIDTH - strlen(title)) << " : "  << eckit::BigNum(value) << std::endl;

}

void Statistics::reportBytes(std::ostream &out, const char *title, eckit::Length value, const char *indent) {
    out << indent << title << std::setw(WIDTH - strlen(title)) << " : "  << eckit::BigNum(value) << " (" << eckit::Bytes(value) << ")" << std::endl;

}

void Statistics::reportTime(std::ostream &out, const char *title, const Timing &value, const char *indent) {
    out << indent << title << std::setw(WIDTH - strlen(title)) << " : "  << eckit::Seconds(value.elapsed_) << " (" << eckit::Seconds(value.cpu_) << " CPU)" << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

Timing &Timing::operator+=(const Timing &other) {
    elapsed_ += other.elapsed_;
    cpu_ += other.cpu_;
    return *this;
}

Timing Timing::operator-(const Timing &other) {
    return Timing(elapsed_ - other.elapsed_, cpu_ - other.cpu_);
}

Stream &operator<<(Stream &s, const Timing& t) {
    s << t.elapsed_;
    s << t.cpu_;
    return s;
}

Stream &operator>>(Stream &s, Timing &t) {
    s >> t.elapsed_;
    s >> t.cpu_;
    return s;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
