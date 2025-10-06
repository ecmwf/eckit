/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_Plural_h
#define eckit_Plural_h

#include <iostream>

#include "eckit/log/BigNum.h"

#include "eckit/memory/NonCopyable.h"

namespace eckit {


class Plural : private eckit::NonCopyable {

public:  // methods

    Plural(int count, const std::string& s) : s_(s), count_(count) {}

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
