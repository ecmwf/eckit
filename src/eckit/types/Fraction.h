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

//-----------------------------------------------------------------------------

namespace eckit {

class MD5;
class Stream;

//-----------------------------------------------------------------------------


class Fraction {
public:
    typedef long long value_type;
    typedef long precision_type;

private:

    Fraction(value_type top, value_type bottom, bool);


public: // methods


// -- Contructors

    Fraction(): top_(0), bottom_(1) {}

    template<class T>
    Fraction(T top): top_(top), bottom_(1) {}

    Fraction(value_type top, value_type bottom);

    Fraction(double);
    Fraction(const std::string&);
    Fraction(const char*);

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

    static value_type mul(value_type a, value_type b) {
        if (b > 0 && a >  std::numeric_limits<value_type>::max() / b) {
            overflow(a, b, '*');
        }
        return a * b;
    }

     static value_type add(value_type a, value_type b) {
        // if (b > 0 && a >  std::numeric_limits<value_type>::max() / b) {
        //     overflow(a, b, '*');
        // }
        return a + b;
    }

    static value_type sub(value_type a, value_type b) {
        // if (b > 0 && a >  std::numeric_limits<value_type>::max() / b) {
        //     overflow(a, b, '*');
        // }
        return a - b;
    }

    Fraction operator+(const Fraction& other) const {
        return Fraction(add(mul(top_, other.bottom_), mul(bottom_, other.top_)),
                        mul(bottom_, other.bottom_));
    }

    Fraction operator-(const Fraction& other) const {
        return Fraction(sub(mul(top_, other.bottom_), mul(bottom_, other.top_)),
                        mul(bottom_, other.bottom_));
    }

    Fraction operator/(const Fraction& other) const {
        return  Fraction(mul(top_,other.bottom_), mul(bottom_,other.top_));

    }

    Fraction operator*(const Fraction& other) const {
        return Fraction(mul(top_, other.top_), mul(bottom_, other.bottom_));
    }

    bool operator==(const Fraction& other) const {
        return top_ == other.top_ && bottom_ == other.bottom_;
    }

    bool operator<(const Fraction& other) const {
        return mul(top_, other.bottom_) < mul(bottom_, other.top_);
    }

    bool operator<=(const Fraction& other) const {
        return mul(top_, other.bottom_) <= mul(bottom_, other.top_);
    }

    bool operator!=(const Fraction& other) const {
        return mul(top_, other.bottom_) != mul(bottom_, other.top_);
    }

    bool operator>(const Fraction& other) const {
        return mul(top_, other.bottom_) > mul(bottom_, other.top_);
    }

    bool operator>=(const Fraction& other) const {
        return mul(top_, other.bottom_) >= mul(bottom_, other.top_);
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
