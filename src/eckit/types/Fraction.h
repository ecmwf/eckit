/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Fraction.h
// Baudouin Raoult - ECMWF Mar 16

#ifndef eckit_Fraction_h
#define eckit_Fraction_h

#include <string>
#include "eckit/exception/Exceptions.h"
#include <limits>
#include "eckit/types/FloatCompare.h"


//-----------------------------------------------------------------------------

namespace eckit {

class MD5;
class Stream;

//-----------------------------------------------------------------------------


class Fraction {
public:
    typedef long long value_type;

    // typedef __int128 value_type;


private:

    Fraction(value_type top, value_type bottom, bool);
    void normalise();

public: // methods


// -- Contructors

    Fraction(): top_(0), bottom_(1) {}

    // template<class T>
    // explicit Fraction(T top): top_(top), bottom_(1) {}

    Fraction(value_type top, value_type bottom);

    explicit Fraction(double);
    // Fraction(const Fraction& other):
    //     top_(other.top_), bottom_(other.bottom_) {}

    explicit Fraction(const std::string&);
    explicit Fraction(const char*);

    bool integer() const {
        return bottom_ == 1;
    }

public: // operators

    operator double() const {
        return double(top_) / double(bottom_);
    }

    operator value_type() const;

    Fraction operator-()  const {
        return Fraction(-top_, bottom_, true);
    }

    value_type integralPart() const {
        return top_ / bottom_;
    }

    Fraction decimalPart() const {
        return *this - integralPart();
    }

    static void overflow(value_type a, value_type b, char op);

    static value_type mul(bool& overflow, value_type a, value_type b) {
        if (b > 0 && a >  std::numeric_limits<value_type>::max() / b) {
            overflow = true;
        }
        return a * b;
    }

    static value_type add(bool& overflow, value_type a, value_type b) {
        // if (b > 0 && a >  std::numeric_limits<value_type>::max() / b) {
        //     overflow(a, b, '*');
        // }
        return a + b;
    }

    static value_type sub(bool& overflow, value_type a, value_type b) {
        // if (b > 0 && a >  std::numeric_limits<value_type>::max() / b) {
        //     overflow(a, b, '*');
        // }
        return a - b;
    }

    Fraction operator+(const Fraction& other) const {
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

    Fraction operator-(const Fraction& other) const {
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

    Fraction operator/(const Fraction& other) const {
        bool overflow = false;
        Fraction result =  Fraction(mul(overflow, top_, other.bottom_),
                                    mul(overflow, bottom_, other.top_));
        if (overflow) {
            result = Fraction(double(*this) / double(other));
        }
        return result;

    }

    Fraction operator*(const Fraction& other) const {
        bool overflow = false;
        Fraction result = Fraction(mul(overflow, top_, other.top_),
                                   mul(overflow, bottom_, other.bottom_));
        if (overflow) {
            result = Fraction(double(*this) * double(other));
        }
        return result;
    }

    bool operator==(const Fraction& other) const {
        // Assumes canonical form
        return top_ == other.top_ && bottom_ == other.bottom_;
    }

    bool operator<(const Fraction& other) const {
        bool overflow = false;
        bool result = mul(overflow, top_, other.bottom_) < mul(overflow, bottom_, other.top_);
        if (overflow) {
            result = double(*this) < double(other);
        }
        return result;
    }

    bool operator<=(const Fraction& other) const {
        bool overflow = false;
        bool result = mul(overflow, top_, other.bottom_) <= mul(overflow, bottom_, other.top_);
        if (overflow) {
            result = double(*this) <= double(other);
        }
        return result;
    }

    bool operator!=(const Fraction& other) const {
        return !(*this == other);
    }

    bool operator>(const Fraction& other) const {
        bool overflow = false;
        bool result = mul(overflow, top_, other.bottom_) > mul(overflow, bottom_, other.top_);
        if (overflow) {
            result = double(*this) > double(other);
        }
        return result;
    }

    bool operator>=(const Fraction& other) const {
        bool overflow = false;
        bool result = mul(overflow, top_, other.bottom_) >= mul(overflow, bottom_, other.top_);
        if (overflow) {
            result = double(*this) >= double(other);
        }
        return result;
    }

    Fraction& operator+=(const Fraction& other) {
        *this = (*this) + other;
        return *this;
    }

    Fraction& operator-=(const Fraction& other) {
        *this = (*this) - other;
        return *this;
    }

    Fraction& operator/=(const Fraction& other) {
        *this = (*this) / other;
        return *this;
    }

    Fraction& operator*=(const Fraction& other) {
        *this = (*this) * other;
        return *this;
    }

    template<class T>
    Fraction operator+(T other) const {
        return *this + Fraction(other);
    }

    template<class T>
    Fraction operator-(T other) const {
        return *this - Fraction(other);
    }

    template<class T>
    Fraction operator/(T other) const {
        return *this / Fraction(other);
    }

    template<class T>
    Fraction operator*(T other) const {
        return *this * Fraction(other);
    }


    template<class T>
    bool operator==(T other) const {
        return *this == Fraction(other);
    }

    template<class T>
    bool operator<(T other) const {
        return *this < Fraction(other);
    }

    template<class T>
    bool operator<=(T other) const {
        return *this <= Fraction(other);
    }

    template<class T>
    bool operator!=(T other) const {
        return *this != Fraction(other);
    }

    template<class T>
    bool operator>(T other) const {
        return *this > Fraction(other);
    }

    template<class T>
    bool operator>=(T other) const {
        return *this >= Fraction(other);
    }

    template<class T>
    Fraction& operator+=(T other) {
        return (*this) += Fraction(other);
    }

    template<class T>
    Fraction& operator-=(T other) {
        return (*this) -= Fraction(other);
    }

    template<class T>
    Fraction& operator/=(T other) {
        return (*this) /= Fraction(other);
    }

    template<class T>
    Fraction& operator*=(T other) {
        return (*this) *= Fraction(other);
    }

    void hash(eckit::MD5&) const;

private: // members

    value_type top_;
    value_type bottom_;

    void print(std::ostream& out) const;
    void encode(Stream& out) const;
    void decode(Stream& out);


    friend std::ostream& operator<<(std::ostream& s, const Fraction& x) {
        x.print(s);
        return s;
    }


    friend Stream& operator<<(Stream& s, const Fraction& x) {
        x.encode(s);
        return s;
    }

    friend Stream& operator>>(Stream& s, Fraction& x) {
        x.decode(s);
        return s;
    }

};

template<class T>
Fraction operator+(T n, const Fraction& f)
{
    return Fraction(n) + f;
}

template<class T>
Fraction operator-(T n, const Fraction& f)
{
    return Fraction(n) - f;
}

template<class T>
Fraction operator/(T n, const Fraction& f)
{
    return Fraction(n) / f;
}

template<class T>
Fraction operator*(T n, const Fraction& f)
{
    return Fraction(n) * f;
}

template<class T>
bool operator==(T n, const Fraction& f)
{
    return Fraction(n) == f;
}

template<class T>
bool operator<(T n, const Fraction& f)
{
    return Fraction(n) < f;
}

template<class T>
bool operator<=(T n, const Fraction& f)
{
    return Fraction(n) <= f;
}

template<class T>
bool operator!=(T n, const Fraction& f)
{
    return Fraction(n) != f;
}

template<class T>
bool operator>(T n, const Fraction& f)
{
    return Fraction(n) > f;
}

template<class T>
bool operator>=(T n, const Fraction& f)
{
    return Fraction(n) >= f;
}

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
