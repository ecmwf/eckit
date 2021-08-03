/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>
#include <limits>
#include <utility>

#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Timer.h"

namespace eckit {


//----------------------------------------------------------------------------------------------------------------------

/// @note supports only until Yottabyte :-)

static const char magnitudes[] = {' ', 'K', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y'};

Bytes::Bytes(double bytes) :
    bytes_(bytes), rate_(false) {}

Bytes::Bytes(double bytes, Timer& timer) :
    bytes_(rate(bytes, timer.elapsed())), rate_(true) {}

Bytes::Bytes(double bytes, double elapsed) :
    bytes_(rate(bytes, elapsed)), rate_(true) {}


int Bytes::sign() const {
    return (bytes_ >= 0) ? 1 : -1;
}

double Bytes::rate(double num, double den) {

    if (num == 0.)
        return 0.;

    if (den == 0.)
        return num * std::numeric_limits<double>::infinity();  // must be after, num gives sign to inf

    return num / den;
}

double Bytes::value() const {
    return bytes_;
}

std::pair<double, char> Bytes::reduceTo1024() const {

    double x = std::abs(bytes_);

    size_t n = 0;
    while (x >= 1024. && n < NUMBER(magnitudes)) {
        x /= 1024.;
        n++;
    }

    return std::make_pair(sign() * x, magnitudes[n]);
}


static const double yotta = 1024. * 1024. * 1024. * 1024. * 1024. * 1024. * 1024. * 1024.;
std::pair<double, char> Bytes::reduceTo100() const {

    double x = std::abs(bytes_);

    if (x > yotta || std::isinf(x)) {
        return std::make_pair(sign() * 99., 'Y');
    }

    size_t n = 0;
    while (x > 100 && n < NUMBER(magnitudes)) {
        x /= 1024.;
        n++;
    }

    x = (x >= 10) ? long(x + 0.5) : long(x * 10 + 0.5) / 10.0;

    return std::make_pair(sign() * x, magnitudes[n]);
}

std::string Bytes::shorten() const {

    std::pair<double, char> r = reduceTo100();

    std::ostringstream s;
    s << r.first << r.second;
    return s.str();
}

std::ostream& operator<<(std::ostream& s, const Bytes& b) {
    std::pair<double, char> r = b.reduceTo1024();

    if (b.sign() < 0)
        s << '-';

    s << r.first << ' ';

    if (r.second != ' ')
        s << r.second;

    s << "byte";

    if (r.first != 1.)
        s << 's';

    if (b.rate_)
        s << " per second";

    return s;
}

Bytes::operator std::string() const {
    std::ostringstream s;
    s << *this;
    return s.str();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
