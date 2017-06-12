/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/types/Fraction.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/parser/Tokenizer.h"
#include "eckit/utils/Translator.h"
#include "eckit/utils/MD5.h"

#include <limits>
#include <cmath>

//-----------------------------------------------------------------------------

namespace eckit {

const long long max = std::sqrt(std::numeric_limits<long long>::max());


static long long gcd(long long a, long long b) {
    while (b != 0)
    {
        long long r = a % b;
        a = b;
        b = r;
    }
    return a;
}

//-----------------------------------------------------------------------------

Fraction::Fraction(long long top, long long bottom) {

    ASSERT(bottom != 0);

    long long sign = 1;
    if (top < 0) {
        top = -top;
        sign = -sign;
    }

    if (bottom < 0) {
        bottom = -bottom;
        sign = -sign;
    }

    long long g = gcd(top, bottom);
    top_ = sign * top / g;
    bottom_ = bottom / g;
}


Fraction::Fraction(double x) {
    long long sign = 1;
    if (x < 0) {
        sign = -sign;
        x = -x;
    }

    long long m00 = 1, m11 = 1, m01 = 0, m10 = 0;
    long long a = x;
    long long t2 = m10 * a + m11;

    while (t2 <= max) {

        long long t1  = m00 * a + m01;
        m01 = m00;
        m00 = t1;

        m11 = m10;
        m10 = t2;

        if (x == a) {
            break;
        }

        x = 1.0 / (x - a);

        if (x > std::numeric_limits<long long>::max()) {
            break;
        }

        a = x;
        t2 = m10 * a + m11;
    }

    top_ = sign * m00;
    bottom_ = m10;

}

Fraction::Fraction(const std::string& s) {
    static Tokenizer parse("/");

    std::vector<std::string> v;
    parse(s, v);
    if (v.size() > 1) {
        ASSERT(v.size() == 2);
        static Translator<std::string, long long> s2l;
        Fraction f(s2l(v[0]), s2l(v[1]));
        top_ = f.top_;
        bottom_ = f.bottom_;
        return;
    }

    static Translator<std::string, double> s2d;
    Fraction f(s2d(s));
    top_ = f.top_;
    bottom_ = f.bottom_;

}

Fraction::Fraction(const char* c) {
    std::string s(c);
    Fraction f(s);
    top_ = f.top_;
    bottom_ = f.bottom_;
}


void Fraction::print(std::ostream& out) const {
    if (bottom_ == 1) {
        out << top_;
    }
    else {
        out << top_ << '/' << bottom_;
    }
}

Fraction::operator long long() const {
    if (bottom_ == 1) {
        return top_;
    }
    std::ostringstream oss;
    oss << "Cannot convert fraction " << *this << " to integer";
    throw eckit::SeriousBug(oss.str());
}

void Fraction::hash(MD5& md5) const {
    md5 << top_;
    md5 << bottom_;
}

//-----------------------------------------------------------------------------

} // namespace eckit

