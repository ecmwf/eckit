/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
