// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_Plural_h
#define eckit_Plural_h

#include <iostream>

#include "eckit/log/BigNum.h"

namespace eckit {


class Plural {

public:  // methods

    Plural(int count, const std::string& s) : s_(s), count_(count) {}

    Plural(const Plural&)            = delete;
    Plural& operator=(const Plural&) = delete;
    Plural(Plural&&)                 = delete;
    Plural& operator=(Plural&&)      = delete;

    ~Plural() {}

protected:  // methods

    void print(std::ostream& s) const {
        s << BigNum(count_) << ' ' << s_;
        if (count_ > 1) {
            s << 's';
        }
    }

private:  // members

    friend std::ostream& operator<<(std::ostream& s, const Plural& p) {
        p.print(s);
        return s;
    }

private:  // members

    std::string s_;

    int count_;
};


}  // namespace eckit

#endif
