/*
 * (C) Copyright 1996- ECMWF.
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

#include <iosfwd>
#include <string>


//-----------------------------------------------------------------------------

namespace eckit {

class MD5;
class Stream;

//-----------------------------------------------------------------------------


class Fraction {
public:

    using value_type = long long;

    Fraction() : top_(0), bottom_(1) {}

    Fraction(value_type top, value_type bottom);

    explicit Fraction(double n);
    explicit Fraction(int n) : top_(n), bottom_(1) {}
    explicit Fraction(short n) : top_(n), bottom_(1) {}
    explicit Fraction(long n) : top_(n), bottom_(1) {}
    explicit Fraction(long long n) : top_(n), bottom_(1) {}

    explicit Fraction(unsigned int n) : top_(n), bottom_(1) {}
    explicit Fraction(unsigned short n) : top_(n), bottom_(1) {}
    explicit Fraction(unsigned long n);
    explicit Fraction(unsigned long long n);

    explicit Fraction(const std::string&);
    explicit Fraction(const char*);

    bool integer() const { return bottom_ == 1; }

    static Fraction abs(const Fraction&);

    // Convert string to fraction by shifting the decimal point: e.g. 14.5 in 145/10
    static Fraction fromString(const std::string&);

    static Fraction::value_type max_denominator();

    operator double() const { return static_cast<double>(top_) / static_cast<double>(bottom_); }

    operator value_type() const;

    Fraction operator-() const { return {-top_, bottom_}; }

    value_type integralPart() const { return top_ / bottom_; }

    Fraction decimalPart() const { return *this - integralPart(); }

    Fraction inverse() const;

    void hash(eckit::MD5&) const;

    value_type numerator() const { return top_; }

    value_type denominator() const { return bottom_; }

    Fraction operator+(const Fraction&) const;

    Fraction operator-(const Fraction&) const;

    Fraction operator/(const Fraction&) const;

    Fraction operator*(const Fraction&) const;

    bool operator==(const Fraction&) const;

    bool operator<(const Fraction&) const;

    bool operator<=(const Fraction&) const;

    bool operator!=(const Fraction&) const;

    bool operator>(const Fraction&) const;

    bool operator>=(const Fraction&) const;

    Fraction& operator+=(const Fraction& f) {
        *this = (*this) + f;
        return *this;
    }

    Fraction& operator-=(const Fraction& f) {
        *this = (*this) - f;
        return *this;
    }

    Fraction& operator/=(const Fraction& f) {
        *this = (*this) / f;
        return *this;
    }

    Fraction& operator*=(const Fraction& f) {
        *this = (*this) * f;
        return *this;
    }

    template <class T>
    Fraction operator+(T other) const {
        return *this + Fraction(other);
    }

    template <class T>
    Fraction operator-(T other) const {
        return *this - Fraction(other);
    }

    template <class T>
    Fraction operator/(T other) const {
        return *this / Fraction(other);
    }

    template <class T>
    Fraction operator*(T other) const {
        return *this * Fraction(other);
    }

    //====================================

    template <class T>
    bool operator==(T other) const {
        return *this == Fraction(other);
    }

    template <class T>
    bool operator<(T other) const {
        return *this < Fraction(other);
    }

    template <class T>
    bool operator<=(T other) const {
        return *this <= Fraction(other);
    }

    template <class T>
    bool operator!=(T other) const {
        return *this != Fraction(other);
    }

    template <class T>
    bool operator>(T other) const {
        return *this > Fraction(other);
    }

    template <class T>
    bool operator>=(T other) const {
        return *this >= Fraction(other);
    }

    template <class T>
    Fraction& operator+=(T other) {
        return (*this) += Fraction(other);
    }

    template <class T>
    Fraction& operator-=(T other) {
        return (*this) -= Fraction(other);
    }

    template <class T>
    Fraction& operator/=(T other) {
        return (*this) /= Fraction(other);
    }

    template <class T>
    Fraction& operator*=(T other) {
        return (*this) *= Fraction(other);
    }

    //====================================

    // Return a version that can be converted to double and back
    Fraction stableVersion(size_t max = 1000000) const;

private:

    value_type top_;
    value_type bottom_;

    void print(std::ostream&) const;
    void encode(Stream&) const;
    void decode(Stream&);

    std::string to_string() const;

    friend std::ostream& operator<<(std::ostream& s, const Fraction& f) {
        f.print(s);
        return s;
    }

    friend Stream& operator<<(Stream& s, const Fraction& f) {
        f.encode(s);
        return s;
    }

    friend Stream& operator>>(Stream& s, Fraction& f) {
        f.decode(s);
        return s;
    }
};

template <>
bool Fraction::operator==(double) const;

template <>
bool Fraction::operator<(double) const;

template <>
bool Fraction::operator<=(double) const;

template <>
bool Fraction::operator!=(double) const;

template <>
bool Fraction::operator>(double) const;

template <>
bool Fraction::operator>=(double) const;

template <class T>
Fraction operator+(T n, const Fraction& f) {
    return f.operator+(n);
}

template <class T>
Fraction operator-(T n, const Fraction& f) {
    return (-f).operator+(n);
}

template <class T>
Fraction operator/(T n, const Fraction& f) {
    return f.inverse().operator*(n);
}

template <class T>
Fraction operator*(T n, const Fraction& f) {
    return f.operator*(n);
}

template <class T>
bool operator==(T n, const Fraction& f) {
    return f.operator==(n);
}

template <class T>
bool operator<(T n, const Fraction& f) {
    return f.operator>(n);
}

template <class T>
bool operator<=(T n, const Fraction& f) {
    return f.operator>=(n);
}

template <class T>
bool operator!=(T n, const Fraction& f) {
    return f.operator!=(n);
}

template <class T>
bool operator>(T n, const Fraction& f) {
    return f.operator<(n);
}

template <class T>
bool operator>=(T n, const Fraction& f) {
    return f.operator<=(n);
}

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
