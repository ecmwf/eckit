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

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


class Fraction {
private:

    Fraction(long long top, long long bottom, bool): top_(top), bottom_(bottom) {}


public: // methods

// -- Contructors

    Fraction(): top_(0), bottom_(1) {}

    template<class T>
    Fraction(T top): top_(top), bottom_(1) {}

    Fraction(long long top, long long bottom);

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

    operator long long() const;

    Fraction operator-()  const {
        return Fraction(-top_, bottom_, true);
    }

    long long integralPart() const {
        return top_ / bottom_;
    }

    Fraction decimalPart() const {
        return *this - integralPart();
    }

    Fraction operator+(const Fraction& other) const {
        return Fraction(top_ * other.bottom_ + bottom_ * other.top_,
                        bottom_ * other.bottom_);
    }

    Fraction operator-(const Fraction& other) const {
        return Fraction(top_ * other.bottom_ - bottom_ * other.top_,
                        bottom_ * other.bottom_);
    }

    Fraction operator/(const Fraction& other) const {
        return Fraction(top_ * other.bottom_, bottom_ * other.top_);
    }

    Fraction operator*(const Fraction& other) const {
        return Fraction(top_ * other.top_, bottom_ * other.bottom_);
    }

    bool operator==(const Fraction& other) const {
        return top_ == other.top_ && bottom_ == other.bottom_;
    }

    bool operator<(const Fraction& other) const {
        return top_ * other.bottom_ < bottom_ * other.top_;
    }

    bool operator<=(const Fraction& other) const {
        return top_ * other.bottom_ <= bottom_ * other.top_;
    }

    bool operator!=(const Fraction& other) const {
        return top_ * other.bottom_ != bottom_ * other.top_;
    }

    bool operator>(const Fraction& other) const {
        return top_ * other.bottom_ > bottom_ * other.top_;
    }

    bool operator>=(const Fraction& other) const {
        return top_ * other.bottom_ >= bottom_ * other.top_;
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

private: // members

    long long top_;
    long long bottom_;

    void print(std::ostream& out) const;


    friend std::ostream& operator<<(std::ostream& s, const Fraction& x) {
        x.print(s);
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
