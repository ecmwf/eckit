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
#include "eckit/serialisation/Stream.h"
#include "eckit/utils/MD5.h"

#include <limits>
#include <cmath>

//-----------------------------------------------------------------------------

namespace eckit {

const Fraction::value_type MAX_DENOM = std::sqrt(std::numeric_limits<Fraction::value_type>::max());


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

    normalise();
}

Fraction::Fraction(value_type top, value_type bottom, bool):
    top_(top),
    bottom_(bottom) {

    normalise();
}


Fraction::Fraction(double x) {
    value_type sign = 1;
    if (x < 0) {
        sign = -sign;
        x = -x;
    }

    value_type m00 = 1, m11 = 1, m01 = 0, m10 = 0;
    value_type a = x;
    value_type t2 = m10 * a + m11;

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
    }

    while (m10 >= MAX_DENOM || m00 >= MAX_DENOM) {
        m00 >>= 1;
        m10 >>= 1;
    }

    top_ = sign * m00;
    bottom_ = m10;

    normalise();

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

        normalise();

        return;
    }

    static Translator<std::string, double> s2d;
    Fraction f(s2d(s));
    top_ = f.top_;
    bottom_ = f.bottom_;

    normalise();

}

Fraction::Fraction(const char* c) {
    std::string s(c);
    Fraction f(s);
    top_ = f.top_;
    bottom_ = f.bottom_;
    normalise();
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
    normalise();
}

//======================================


inline Fraction::value_type mul(bool& overflow, Fraction::value_type a, Fraction::value_type b) {
    if (b > 0 && a >  std::numeric_limits<Fraction::value_type>::max() / b) {
        overflow = true;
    }
    return a * b;
}

inline Fraction::value_type add(bool& overflow, Fraction::value_type a, Fraction::value_type b) {
    // if (b > 0 && a >  std::numeric_limits<Fraction::value_type>::max() / b) {
    //     overflow(a, b, '*');
    // }
    return a + b;
}

inline Fraction::value_type sub(bool& overflow, Fraction::value_type a, Fraction::value_type b) {
    // if (b > 0 && a >  std::numeric_limits<Fraction::value_type>::max() / b) {
    //     overflow(a, b, '*');
    // }
    return a - b;
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

static Fraction::value_type compare(Fraction::value_type t1, Fraction::value_type b1, Fraction::value_type t2, Fraction::value_type b2) {
    Fraction::value_type s1 = 1;
    Fraction::value_type s2 = 1;

    if (t1 < 0) {
        s1 = -1;
        t1 = -t1;
    }

    if (t2 < 0) {
        s2 = -1;
        t2 = -t2;
    }

    if (s1 != s2) {

        if (s1 < s2) {
            return -1;
        }
        else {
            return 1;
        }
    }

    Fraction::value_type g;

    std::cout << t1 << "/" << b1 << " " << t2 << "/" << b2 << std::endl;

    bool more = true;
    while (more) {
        more = false;

        g = gcd(t1, t2);
        if (g != 1) {
            t1 = t1 / g;
            t2 = t2 / g;
            more = true;
        }
        g = gcd(b1, b2);
        if (g != 1) {
            b1 = b1 / g;
            b2 = b2 / g;
            more = true;
        }

        g = gcd(t1, b1);
        if (g != 1) {
            t1 = t1 / g;
            b1 = b1 / g;
            more = true;
        }
        g = gcd(t2, b2);
        if (g != 1) {
            t2 = t2 / g;
            b2 = b2 / g;
            more = true;
        }
    }

    std::cout << t1 << "/" << b1 << " " << t2 << "/" << b2 << std::endl;


}

bool Fraction::operator<(const Fraction& other) const {
    bool overflow = false;
    bool result = mul(overflow, top_, other.bottom_) < mul(overflow, bottom_, other.top_);
    if (overflow) {
        result = double(*this) < double(other);
    }
    return result;
}

bool Fraction::operator<=(const Fraction& other) const {
    bool overflow = false;
    bool result = mul(overflow, top_, other.bottom_) <= mul(overflow, bottom_, other.top_);
    if (overflow) {
        result = double(*this) <= double(other);
    }
    return result;
}

bool Fraction::operator!=(const Fraction& other) const {
    return !(*this == other);
}

bool Fraction::operator>(const Fraction& other) const {
    bool overflow = false;
    bool result = mul(overflow, top_, other.bottom_) > mul(overflow, bottom_, other.top_);
    if (overflow) {
        result = double(*this) > double(other);
    }
    return result;
}

bool Fraction::operator>=(const Fraction& other) const {
    bool overflow = false;
    bool result = mul(overflow, top_, other.bottom_) >= mul(overflow, bottom_, other.top_);
    if (overflow) {
        result = double(*this) >= double(other);
    }
    return result;
}



//======================================

void Fraction::normalise() {
    if (bottom_ > MAX_DENOM || std::abs(top_) > MAX_DENOM) {

        value_type sign = 1;

        if (top_ < 0) {
            sign = -sign;
            top_ = -top_;
        }

        while (bottom_ > MAX_DENOM || top_ > MAX_DENOM) {
            top_ >>= 1;
            bottom_ >>= 1;
        }

        value_type g = gcd(top_, bottom_);
        top_ =  top_ / g;
        bottom_ = bottom_ / g;

        top_ *= sign;
    }
}

//-----------------------------------------------------------------------------

} // namespace eckit

