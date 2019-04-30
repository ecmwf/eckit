/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "Statistics.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iomanip>

#include "eckit/log/BigNum.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Seconds.h"
#include "eckit/serialisation/Stream.h"

namespace eckit {

const size_t WIDTH = 34;

Timer Statistics::timer_;

//----------------------------------------------------------------------------------------------------------------------

void Statistics::reportCount(std::ostream& out, const char* title, size_t value, const char* indent, bool always) {
    if (value || always) {
        out << indent << title << std::setw(WIDTH - strlen(title)) << " : " << eckit::BigNum(value) << std::endl;
    }
}

void Statistics::reportUnit(std::ostream& out, const char* title, const char* unit, double value, const char* indent,
                            bool always) {
    if (value || always) {
        out << indent << title << std::setw(WIDTH - strlen(title)) << " : " << value << " " << unit << std::endl;
    }
}


void Statistics::reportRate(std::ostream& out, const char* title, unsigned long long value, const char* indent,
                            bool always) {
    if (value || always) {
        out << indent << title << std::setw(WIDTH - strlen(title)) << " : " << eckit::BigNum(value) << " bytes/s ("
            << eckit::Bytes(value) << "/s)" << std::endl;
    }
}

void Statistics::reportRate(std::ostream& out, const std::string& title, size_t bytes, double elapsed,
                            const char* indent, bool always) {

    if (bytes || always) {

        double rate = 0;
        if (bytes != 0 and elapsed > 0) {
            rate = bytes / elapsed;
        }

        out << indent << title << std::setw(WIDTH - title.length()) << " : " << BigNum(size_t(rate)) << " bytes/s"
            << " (" << Bytes(rate) << " per second)" << std::endl;
    }
}

void Statistics::reportBytes(std::ostream& out, const char* title, unsigned long long value, const char* indent,
                             bool always) {
    if (value || always) {
        out << indent << title << std::setw(WIDTH - strlen(title)) << " : " << eckit::BigNum(value) << " ("
            << eckit::Bytes(value) << ")" << std::endl;
    }
}

void Statistics::reportBytesStats(std::ostream& out, const std::string& title, size_t count, size_t bytes,
                                  size_t sumsquared, const char* indent, bool always) {

    if (count || always) {

        double average      = 0;
        double stdDeviation = 0;
        if (count != 0) {
            average      = bytes / count;
            stdDeviation = std::sqrt(std::max((count * sumsquared) - (bytes * bytes), size_t(0))) / count;
        }

        out << indent << title << std::setw(WIDTH - title.length()) << " (tot, avg, std dev) : " << BigNum(bytes)
            << " (" << Bytes(bytes) << ")"
            << ", " << BigNum(size_t(average)) << " (" << Bytes(average) << ")"
            << ", " << BigNum(size_t(stdDeviation)) << " (" << Bytes(stdDeviation) << ")" << std::endl;
    }
}


void Statistics::reportTime(std::ostream& out, const char* title, const Timing& value, const char* indent,
                            bool always) {
    if (value.updates_ || always) {
        out << indent << title << std::setw(WIDTH - strlen(title)) << " : " << eckit::Seconds(value.elapsed_) << " ("
            << eckit::Seconds(value.cpu_) << " CPU). Updates: " << eckit::BigNum(value.updates_) << std::endl;
    }
}

void Statistics::reportTime(std::ostream& out, const char* title, double value, const char* indent, bool always) {
    if (value || always) {
        out << indent << title << std::setw(WIDTH - strlen(title)) << " : " << eckit::Seconds(value) << std::endl;
    }
}

void Statistics::reportTimeStats(std::ostream& out, const std::string& title, size_t count, double sum_times,
                                 double sum_times_squared, const char* indent, bool always) {

    if (count || always) {

        double average      = 0;
        double stdDeviation = 0;
        if (count != 0) {
            average      = sum_times / count;
            stdDeviation = std::sqrt(std::max((count * sum_times_squared) - (sum_times * sum_times), 0.0)) / count;
        }

        out << indent << title << std::setw(WIDTH - title.length()) << " (tot, avg, std dev) : " << sum_times << " s"
            << ", " << average << " s"
            << ", " << stdDeviation << " s" << std::endl;
    }
}

//----------------------------------------------------------------------------------------------------------------------

Timing& Timing::operator+=(const Timing& other) {
    elapsed_ += other.elapsed_;
    cpu_ += other.cpu_;
    updates_ += other.updates_;
    return *this;
}

Timing& Timing::operator-=(const Timing& other) {
    elapsed_ -= other.elapsed_;
    cpu_ -= other.cpu_;
    // We don't remove the number of update because
    // we use this operator to remove sub-timers
    // updates_ -= other.updates_;
    return *this;
}


Timing& Timing::operator/=(size_t n) {
    elapsed_ /= n;
    cpu_ /= n;
    if (updates_) {
        updates_ /= n;
        if (!updates_) {
            updates_ = 1;
        }
    }
    return *this;
}

Timing Timing::operator-(const Timing& other) const {
    return Timing(elapsed_ - other.elapsed_, cpu_ - other.cpu_, 1);
}


Stream& operator<<(Stream& s, const Timing& t) {
    s << t.elapsed_;
    s << t.cpu_;
    s << t.updates_;
    return s;
}

Stream& operator>>(Stream& s, Timing& t) {
    s >> t.elapsed_;
    s >> t.cpu_;
    s >> t.updates_;
    return s;
}

std::ostream& operator<<(std::ostream& s, const Timing& t) {
    s << t.elapsed_;
    return s;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
