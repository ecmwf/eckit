/*
 * (C) Copyright 1996- ECMWF.
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
#include "eckit/serialisation/Stream.h"
#include "eckit/utils/MD5.h"

#include <cmath>
#include <limits>

//-----------------------------------------------------------------------------

namespace eckit {

const Fraction::value_type MAX_DENOM = std::sqrt(std::numeric_limits<Fraction::value_type>::max());
// const Fraction::value_type MAX_DENOM = std::numeric_limits<Fraction::value_type>::max() >> 1;


static Fraction::value_type gcd(Fraction::value_type a, Fraction::value_type b) {
    while (b != 0)
    {
        Fraction::value_type r = a % b;
        a = b;
        b = r;
    }
    return a;
}

//-----------------------------------------------------------------------------

Fraction::Fraction(value_type top, value_type bottom) {

    ASSERT(bottom != 0);

    value_type sign = 1;
    if (top < 0) {
        top = -top;
        sign = -sign;
    }

    if (bottom < 0) {
        bottom = -bottom;
        sign = -sign;
    }

    value_type g = gcd(top, bottom);
    top =  top / g;
    bottom = bottom / g;

    top_ = sign * top;
    bottom_ = bottom;

}

Fraction::Fraction(double x) {

    double value = x;

    ASSERT(!std::isnan(value));
    // ASSERT(fabs(value) < 1e30);


    value_type sign = 1;
    if (x < 0) {
        sign = -sign;
        x = -x;
    }

    value_type m00 = 1, m11 = 1, m01 = 0, m10 = 0;
    value_type a = x;
    value_type t2 = m10 * a + m11;

    size_t cnt = 0;

    while (t2 <= MAX_DENOM) {

        value_type t1  = m00 * a + m01;
        m01 = m00;
        m00 = t1;

        m11 = m10;
        m10 = t2;

        if (x == a) {
            break;
        }

        x = 1.0 / (x - a);

        if (x > std::numeric_limits<value_type>::max()) {
            break;
        }

        a = x;
        t2 = m10 * a + m11;

        if(cnt > 10000) {
            std::ostringstream oss;
            oss << "Cannot compute fraction from " << value << std::endl;
            throw eckit::SeriousBug(oss.str());
        }
    }

    while (m10 >= MAX_DENOM || m00 >= MAX_DENOM) {
        m00 >>= 1;
        m10 >>= 1;
    }

    value_type top = m00;
    value_type bottom = m10;

    value_type g = gcd(top, bottom);
    top =  top / g;
    bottom = bottom / g;

    top_ = sign * top;
    bottom_ = bottom;




}

Fraction::Fraction(const std::string& s) {
    static Tokenizer parse("/");

    std::vector<std::string> v;
    parse(s, v);
    if (v.size() > 1) {
        ASSERT(v.size() == 2);
        static Translator<std::string, value_type> s2l;
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

Fraction::operator value_type() const {
    if (bottom_ == 1) {
        return top_;
    }
    std::ostringstream oss;
    oss << "Cannot convert fraction " << *this << " (" << double(*this) << ") to integer";
    throw eckit::SeriousBug(oss.str());
}

void Fraction::hash(MD5& md5) const {
    md5 << top_;
    md5 << bottom_;
}


void Fraction::encode(Stream& s) const {
    s << top_;
    s << bottom_;
}

void Fraction::decode(Stream& s) {
    s >> top_;
    s >> bottom_;
}

//======================================


inline Fraction::value_type mul(bool& overflow, Fraction::value_type a, Fraction::value_type b) {
    // assumes signed 'a' and unsigned 'b'
    if (b > 0) {
        if ((a > 0 && a > std::numeric_limits<Fraction::value_type>::max() / b) ||
            (a < 0 && a < std::numeric_limits<Fraction::value_type>::min() / b)) {
            overflow = true;
        }
    }
    return a * b;
}

inline Fraction::value_type add(bool& overflow, Fraction::value_type a, Fraction::value_type b) {

    if (a >= 0 && b >= 0 && (a > std::numeric_limits<Fraction::value_type>::max() - b)) {
        overflow = true;
    }

    if (a <= 0 && b <= 0 && (-a > std::numeric_limits<Fraction::value_type>::max() + b)) {
        overflow = true;
    }

    return a + b;
}

inline Fraction::value_type sub(bool& overflow, Fraction::value_type a, Fraction::value_type b) {
    return add(overflow, a, -b);
}

Fraction Fraction::operator+(const Fraction& other) const {
    bool overflow = false;
    Fraction result = Fraction(
                          add(overflow,
                              mul(overflow, top_, other.bottom_),
                              mul(overflow, bottom_, other.top_)),
                          mul(overflow, bottom_, other.bottom_));
    if (overflow) {
        result = Fraction(double(*this) + double(other));
    }
    return result;
}

Fraction Fraction::operator-(const Fraction& other) const {
    bool overflow = false;
    Fraction result = Fraction(
                          sub(overflow,
                              mul(overflow, top_, other.bottom_),
                              mul(overflow, bottom_, other.top_)),
                          mul(overflow, bottom_, other.bottom_));
    if (overflow) {
        result = Fraction(double(*this) - double(other));
    }
    return result;
}

Fraction Fraction::operator/(const Fraction& other) const {
    bool overflow = false;
    Fraction result =  Fraction(mul(overflow, top_, other.bottom_),
                                mul(overflow, bottom_, other.top_));
    if (overflow) {
        result = Fraction(double(*this) / double(other));
    }
    return result;

}

Fraction Fraction::operator*(const Fraction& other) const {
    bool overflow = false;
    Fraction result = Fraction(mul(overflow, top_, other.top_),
                               mul(overflow, bottom_, other.bottom_));
    if (overflow) {
        result = Fraction(double(*this) * double(other));
    }
    return result;
}

bool Fraction::operator==(const Fraction& other) const {
    // Assumes canonical form
    return top_ == other.top_ && bottom_ == other.bottom_;
}

bool Fraction::operator!=(const Fraction& other) const {
    return !(*this == other);
}

bool Fraction::operator<(const Fraction& other) const {
    bool overflow = false;
    bool result = mul(overflow, top_, other.bottom_) < mul(overflow, other.top_, bottom_);
    if (overflow) {
        return double(*this) < double(other);
    }
    return result;
}

bool Fraction::operator<=(const Fraction& other) const {
    bool overflow = false;
    bool result = mul(overflow, top_, other.bottom_) <= mul(overflow, other.top_, bottom_);
    if (overflow) {
        return double(*this) <= double(other);
    }
    return result;
}

bool Fraction::operator>(const Fraction& other) const {
    bool overflow = false;
    bool result = mul(overflow, top_, other.bottom_) > mul(overflow, other.top_, bottom_);
    if (overflow) {
        return double(*this) > double(other);
    }
    return result;
}

bool Fraction::operator>=(const Fraction& other) const {
    bool overflow = false;
    bool result = mul(overflow, top_, other.bottom_) >= mul(overflow, other.top_, bottom_);
    if (overflow) {
        return double(*this) >= double(other);
    }
    return result;
}


} // namespace eckit

