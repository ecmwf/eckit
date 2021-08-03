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

#include <cmath>
#include <limits>

#include "eckit/exception/Exceptions.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/utils/MD5.h"
#include "eckit/utils/Tokenizer.h"
#include "eckit/utils/Translator.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

static Fraction::value_type gcd(Fraction::value_type a, Fraction::value_type b) {
    while (b != 0) {
        Fraction::value_type r = a % b;
        a                      = b;
        b                      = r;
    }
    return a;
}

//----------------------------------------------------------------------------------------------------------------------

Fraction::Fraction(value_type top, value_type bottom) {

    ASSERT(bottom != 0);

    /// @note in theory we also assume that numerator and denominator are both representable in
    ///       double without loss
    //    ASSERT(top == value_type(double(top)));
    //    ASSERT(bottom == value_type(double(bottom)));

    value_type sign = 1;
    if (top < 0) {
        top  = -top;
        sign = -sign;
    }

    if (bottom < 0) {
        bottom = -bottom;
        sign   = -sign;
    }

    value_type g = gcd(top, bottom);
    top          = top / g;
    bottom       = bottom / g;

    top_    = sign * top;
    bottom_ = bottom;
}

const Fraction::value_type MAX_DENOM = std::sqrt(std::numeric_limits<Fraction::value_type>::max());

Fraction::value_type Fraction::max_denominator() {
    return MAX_DENOM;
}

Fraction::Fraction(double x) {

    double value = x;

    ASSERT(!std::isnan(value));
    // ASSERT(fabs(value) < 1e30);


    value_type sign = 1;
    if (x < 0) {
        sign = -sign;
        x    = -x;
    }

    value_type m00 = 1, m11 = 1, m01 = 0, m10 = 0;
    value_type a  = x;
    value_type t2 = m10 * a + m11;

    size_t cnt = 0;

    while (t2 <= MAX_DENOM) {

        value_type t1 = m00 * a + m01;
        m01           = m00;
        m00           = t1;

        m11 = m10;
        m10 = t2;

        if (x == a) {
            break;
        }

        x = 1.0 / (x - a);

        if (x > std::numeric_limits<Fraction::value_type>::max()) {
            break;
        }

        a  = x;
        t2 = m10 * a + m11;

        if (cnt++ > 10000) {
            std::ostringstream oss;
            oss << "Cannot compute fraction from " << value << std::endl;
            throw eckit::BadValue(oss.str());
        }
    }

    while (m10 >= MAX_DENOM || m00 >= MAX_DENOM) {
        m00 >>= 1;
        m10 >>= 1;
    }

    value_type top    = m00;
    value_type bottom = m10;

    value_type g = gcd(top, bottom);
    top          = top / g;
    bottom       = bottom / g;

    top_    = sign * top;
    bottom_ = bottom;
}

Fraction::Fraction(double n, const Fraction& precision) {
    value_type bottom = precision.inverse().integralPart();
    value_type top    = value_type(std::round(bottom * n));

    Fraction f(top, bottom);
    top_    = f.top_;
    bottom_ = f.bottom_;
}

Fraction::Fraction(const std::string& s) {
    static Tokenizer parse("/");

    std::vector<std::string> v;
    parse(s, v);
    if (v.size() > 1) {
        ASSERT(v.size() == 2);
        static Translator<std::string, value_type> s2l;
        Fraction f(s2l(v[0]), s2l(v[1]));
        top_    = f.top_;
        bottom_ = f.bottom_;


        return;
    }

    static Translator<std::string, double> s2d;
    Fraction f(s2d(s));
    top_    = f.top_;
    bottom_ = f.bottom_;
}

Fraction::Fraction(const char* c) {
    std::string s(c);
    Fraction f(s);
    top_    = f.top_;
    bottom_ = f.bottom_;
}

Fraction Fraction::abs(const Fraction& f) {
    return {f.top_ < 0 ? -f.top_ : f.top_, f.bottom_};
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

eckit::Fraction Fraction::inverse() const {
    if (top_ == 0) {
        std::ostringstream oss;
        oss << "Cannot compute inverse of " << *this << std::endl;
        throw eckit::BadValue(oss.str());
    }
    return {bottom_, top_};
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

//----------------------------------------------------------------------------------------------------------------------

inline Fraction::value_type mul(bool& overflow, Fraction::value_type a, Fraction::value_type b) {

    if (overflow or (b != 0 and std::abs(a) > (std::numeric_limits<Fraction::value_type>::max() / std::abs(b)))) {
        overflow = true;  // report overflow
        return Fraction::value_type();
    }

    return a * b;
}

inline Fraction::value_type add(bool& overflow, Fraction::value_type a, Fraction::value_type b) {

    if (overflow or (b > 0 ? a > std::numeric_limits<Fraction::value_type>::max() - b : a < std::numeric_limits<Fraction::value_type>::lowest() - b)) {
        overflow = true;  // report overflow
        return Fraction::value_type();
    }

    return a + b;
}

inline Fraction::value_type sub(bool& overflow, Fraction::value_type a, Fraction::value_type b) {
    return add(overflow, a, -b);
}

Fraction Fraction::operator+(const Fraction& other) const {

    bool overflow = false;

    value_type top = add(overflow, mul(overflow, top_, other.bottom_), mul(overflow, bottom_, other.top_));

    value_type bottom = mul(overflow, bottom_, other.bottom_);

    if (!overflow) {
        return Fraction(top, bottom);
    }

    return Fraction(double(*this) + double(other));
}

Fraction Fraction::operator-(const Fraction& other) const {

    bool overflow = false;

    value_type top = sub(overflow, mul(overflow, top_, other.bottom_), mul(overflow, bottom_, other.top_));

    value_type bottom = mul(overflow, bottom_, other.bottom_);

    if (!overflow) {
        return Fraction(top, bottom);
    }

    return Fraction(double(*this) - double(other));
}

Fraction Fraction::operator/(const Fraction& other) const {
    bool overflow = false;

    value_type top = mul(overflow, top_, other.bottom_);

    value_type bottom = mul(overflow, bottom_, other.top_);

    if (!overflow) {
        return Fraction(top, bottom);
    }

    return Fraction(double(*this) / double(other));
}

Fraction Fraction::operator*(const Fraction& other) const {

    bool overflow = false;

    value_type top = mul(overflow, top_, other.top_);

    value_type bottom = mul(overflow, bottom_, other.bottom_);

    if (!overflow) {
        return Fraction(top, bottom);
    }

    return Fraction(double(*this) * double(other));
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
    bool result   = mul(overflow, top_, other.bottom_) < mul(overflow, other.top_, bottom_);
    if (overflow) {
        return double(*this) < double(other);
    }
    return result;
}

bool Fraction::operator<=(const Fraction& other) const {
    bool overflow = false;
    bool result   = mul(overflow, top_, other.bottom_) <= mul(overflow, other.top_, bottom_);
    if (overflow) {
        return double(*this) <= double(other);
    }
    return result;
}

bool Fraction::operator>(const Fraction& other) const {
    bool overflow = false;
    bool result   = mul(overflow, top_, other.bottom_) > mul(overflow, other.top_, bottom_);
    if (overflow) {
        return double(*this) > double(other);
    }
    return result;
}

bool Fraction::operator>=(const Fraction& other) const {
    bool overflow = false;
    bool result   = mul(overflow, top_, other.bottom_) >= mul(overflow, other.top_, bottom_);
    if (overflow) {
        return double(*this) >= double(other);
    }
    return result;
}

//----------------------------------------------------------------------------------------------------------------------


Fraction Fraction::fromString(const std::string& s) {

    long numerator   = 0;
    long denumerator = 1;
    int sgn          = 1;
    size_t err       = 0;
    bool decimal     = false;

    for (size_t i = 0; i < s.length(); ++i) {

        switch (s[i]) {
            case '-':
                if (i != 0) {
                    err++;
                }
                sgn = -1;
                break;

            case '.':
                if (decimal) {
                    err++;
                }
                decimal = true;
                break;

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                numerator *= 10;
                numerator += s[i] - '0';
                if (decimal) {
                    denumerator *= 10;
                }
                break;

            default:
                break;
        }
    }

    if (err) {
        throw eckit::UserError("Fraction::fromString: invalid value [" + s + "]");
    }

    return eckit::Fraction(sgn * numerator, denumerator);
}

Fraction Fraction::stableVersion(size_t max) const {
    Fraction x(*this);

    size_t n = 0;
    for (;;) {
        Fraction y = Fraction(double(x));
        if (y == x) {
            //             std::cout << "STABLE =========== " << n << " "
            //             << double(*this) << " " << *this
            // << " -> " << double(x) << " " << x <<
            //             std::endl;
            break;
        }
        x = y;
        n++;
        if (n >= max) {
            std::ostringstream oss;
            oss << "Fraction::stableVersion(" << *this << ") did not converge after " << max
                << " iterations. Last value: " << x;
            throw eckit::SeriousBug(oss.str());
        }
    }

    return x;
}

//----------------------------------------------------------------------------------------------------------------------

template <>
bool Fraction::operator==(double other) const {
    return double(*this) == other;
}

template <>
bool Fraction::operator<(double other) const {
    return double(*this) < other;
}

template <>
bool Fraction::operator<=(double other) const {
    return double(*this) <= other;
}

template <>
bool Fraction::operator!=(double other) const {
    return double(*this) != other;
}

template <>
bool Fraction::operator>(double other) const {
    return double(*this) > other;
}

template <>
bool Fraction::operator>=(double other) const {
    return double(*this) >= other;
}

}  // namespace eckit
