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
#include <numeric>
#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/utils/MD5.h"
#include "eckit/utils/Tokenizer.h"
#include "eckit/utils/Translator.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

Fraction::Fraction(value_type top, value_type bottom) {
    ASSERT(bottom != 0);

    const auto abs_top    = std::abs(top);
    const auto abs_bottom = std::abs(bottom);

    const value_type sgn = top < 0 == bottom < 0 ? 1 : -1;
    const value_type g   = std::gcd(abs_top, abs_bottom);

    top_    = abs_top / g * sgn;
    bottom_ = abs_bottom / g;
}

constexpr auto VALUE_TYPE_MAX = std::numeric_limits<Fraction::value_type>::max();
constexpr auto VALUE_TYPE_MIN = std::numeric_limits<Fraction::value_type>::lowest();

const auto MAX_DENOM = static_cast<Fraction::value_type>(std::sqrt(VALUE_TYPE_MAX));

Fraction::value_type Fraction::max_denominator() {
    return MAX_DENOM;
}

Fraction::Fraction(double value) : top_(1), bottom_(0) {
    ASSERT(!std::isnan(value));

    // Implementation of the continued fraction algorithm, with
    // - protection overflows, by limiting to sqrt(value_type maximum)
    // - limited interations
    // - increased tolerance on non-convergence (top/bottom greater than value_type maximum)

    value_type m00 = 1;
    value_type m10 = 0;

    for (auto eps : {std::numeric_limits<double>::epsilon(), 1e-9, 1e-8, 1e-7, 1e-6}) {
        m00 = 1;
        m10 = 0;

        value_type m11 = 1;
        value_type m01 = 0;

        auto x  = std::abs(value);
        auto a  = static_cast<value_type>(x);
        auto t2 = m10 * a + m11;

        size_t cnt = 0;

        while (t2 <= MAX_DENOM) {
            const auto t1 = m00 * a + m01;

            m01 = m00;
            m00 = t1;

            m11 = m10;
            m10 = t2;

            if (const auto dx = x - static_cast<double>(a);
                std::abs(dx) < eps || (x = 1. / dx) > static_cast<double>(VALUE_TYPE_MAX)) {
                break;
            }

            a  = static_cast<value_type>(x);
            t2 = m10 * a + m11;

            if (cnt++ > 10000) {
                throw BadValue("Cannot compute fraction from " + std::to_string(value));
            }
        }

        while (m10 >= MAX_DENOM || m00 >= MAX_DENOM) {
            m00 >>= 1;
            m10 >>= 1;
        }

        if (m10 != 0) {
            const value_type sgn = value < 0 ? -1 : 1;
            const value_type g   = std::gcd(m00, m10);

            top_    = m00 / g * sgn;
            bottom_ = m10 / g;
            return;
        }
    }

    throw BadValue("Cannot compute fraction from " + std::to_string(value));
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
    out << to_string();
}

Fraction::operator value_type() const {
    if (bottom_ == 1) {
        return top_;
    }

    throw SeriousBug("Cannot convert fraction " + to_string() + " (" + std::to_string(operator double()) +
                     ") to integer");
}

Fraction Fraction::inverse() const {
    if (top_ != 0) {
        return {bottom_, top_};
    }

    throw BadValue("Cannot compute inverse of " + to_string());
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

std::string Fraction::to_string() const {
    return bottom_ == 1 ? std::to_string(top_) : (std::to_string(top_) + '/' + std::to_string(bottom_));
}

//----------------------------------------------------------------------------------------------------------------------

inline Fraction::value_type mul(bool& overflow, Fraction::value_type a, Fraction::value_type b) {
    overflow = overflow or (b != 0 and std::abs(a) > (VALUE_TYPE_MAX / std::abs(b)));
    return overflow ? 0 : a * b;
}

inline Fraction::value_type add(bool& overflow, Fraction::value_type a, Fraction::value_type b) {
    overflow = overflow or (b > 0 ? a > VALUE_TYPE_MAX - b : a < VALUE_TYPE_MIN - b);
    return overflow ? 0 : a + b;
}

inline Fraction::value_type sub(bool& overflow, Fraction::value_type a, Fraction::value_type b) {
    return add(overflow, a, -b);
}

Fraction Fraction::operator+(const Fraction& other) const {
    bool overflow = false;
    if (value_type top = add(overflow, mul(overflow, top_, other.bottom_), mul(overflow, bottom_, other.top_)),
        bottom         = mul(overflow, bottom_, other.bottom_);
        !overflow) {
        return {top, bottom};
    }

    return Fraction(operator double() + other.operator double());
}

Fraction Fraction::operator-(const Fraction& other) const {
    bool overflow = false;
    if (value_type top = sub(overflow, mul(overflow, top_, other.bottom_), mul(overflow, bottom_, other.top_)),
        bottom         = mul(overflow, bottom_, other.bottom_);
        !overflow) {
        return {top, bottom};
    }

    return Fraction(operator double() - other.operator double());
}

Fraction Fraction::operator/(const Fraction& other) const {
    bool overflow = false;
    if (value_type top = mul(overflow, top_, other.bottom_), bottom = mul(overflow, bottom_, other.top_); !overflow) {
        return {top, bottom};
    }

    return Fraction(operator double() / other.operator double());
}

Fraction Fraction::operator*(const Fraction& other) const {
    bool overflow = false;
    if (value_type top = mul(overflow, top_, other.top_), bottom = mul(overflow, bottom_, other.bottom_); !overflow) {
        return {top, bottom};
    }

    return Fraction(operator double() * other.operator double());
}

bool Fraction::operator==(const Fraction& other) const {
    // Assumes canonical form
    return top_ == other.top_ && bottom_ == other.bottom_;
}

bool Fraction::operator!=(const Fraction& other) const {
    return !operator==(other);
}

bool Fraction::operator<(const Fraction& other) const {
    if (bool overflow = false, result = mul(overflow, top_, other.bottom_) < mul(overflow, other.top_, bottom_);
        !overflow) {
        return result;
    }

    return operator double() < other.operator double();
}

bool Fraction::operator<=(const Fraction& other) const {
    if (bool overflow = false, result = mul(overflow, top_, other.bottom_) <= mul(overflow, other.top_, bottom_);
        !overflow) {
        return result;
    }

    return operator double() <= other.operator double();
}

bool Fraction::operator>(const Fraction& other) const {
    if (bool overflow = false, result = mul(overflow, top_, other.bottom_) > mul(overflow, other.top_, bottom_);
        !overflow) {
        return result;
    }

    return operator double() > other.operator double();
}

bool Fraction::operator>=(const Fraction& other) const {
    if (bool overflow = false, result = mul(overflow, top_, other.bottom_) >= mul(overflow, other.top_, bottom_);
        !overflow) {
        return result;
    }

    return operator double() >= other.operator double();
}

//----------------------------------------------------------------------------------------------------------------------


Fraction Fraction::fromString(const std::string& s) {
    long numerator   = 0;
    long denominator = 1;
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
                    denominator *= 10;
                }
                break;

            default:
                break;
        }
    }

    if (err != 0) {
        throw UserError("Fraction::fromString: invalid value [" + s + "]");
    }

    return {sgn * numerator, denominator};
}

Fraction Fraction::stableVersion(size_t max) const {
    Fraction x(*this);

    size_t n = 0;
    for (Fraction y(static_cast<double>(x)); x != y; x = y, y = Fraction{static_cast<double>(x)}) {
        if (n++; n >= max) {
            throw SeriousBug("Fraction::stableVersion(" + to_string() + ") did not converge after " +
                             std::to_string(max) + " iterations. Last value: " + x.to_string());
        }
    }

    return x;
}

//----------------------------------------------------------------------------------------------------------------------

template <>
bool Fraction::operator==(double other) const {
    return operator double() == other;
}

template <>
bool Fraction::operator<(double other) const {
    return operator double() < other;
}

template <>
bool Fraction::operator<=(double other) const {
    return operator double() <= other;
}

template <>
bool Fraction::operator!=(double other) const {
    return operator double() != other;
}

template <>
bool Fraction::operator>(double other) const {
    return operator double() > other;
}

template <>
bool Fraction::operator>=(double other) const {
    return operator double() >= other;
}

}  // namespace eckit
