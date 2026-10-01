// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File BigNum.h
// Baudouin Raoult - ECMWF Nov 96

#ifndef eckit_BigNum_h
#define eckit_BigNum_h


#include <iosfwd>


namespace eckit {

/// Class used to print large numbers

class BigNum {

public:

    BigNum(long long v) : value_(v) {}

    BigNum(const BigNum&)            = delete;
    BigNum& operator=(const BigNum&) = delete;
    BigNum(BigNum&&)                 = delete;
    BigNum& operator=(BigNum&&)      = delete;

private:

    void print(std::ostream&) const;

    static void print(std::ostream&, long long);

private:  // members

    long long value_;

    friend std::ostream& operator<<(std::ostream& s, const BigNum& p) {
        p.print(s);
        return s;
    }
};

}  // namespace eckit

#endif
